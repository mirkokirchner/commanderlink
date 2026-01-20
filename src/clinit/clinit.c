#define _POSIX_C_SOURCE 200809L
/*
 * ============================================================================
 * CommanderLink – clinit (Genesis/Initializer) – MERGE GATE Stage 1 (SSOT)
 * Datei: src/clinit/clinit.c
 * ============================================================================
 *
 * Pflichten:
 *   - KEIN shm_unlink im Normalpfad (nur --destroy/--recreate manuell)
 *   - Root ohne IDs (IDs nur Identity-Segment)
 *   - TOC entry epoch = 0 (UNINITIALIZED)
 *   - Bulk-Entries IMMER im TOC; Bulk absent => presence RECLAIMED
 *   - Pflicht resident: Service + Identity + FlowCmd
 *   - Oracle optional: als RECLAIMABLE (Bulk), RECLAIMED wenn Bulk absent
 *   - Genesis-Gate: stride/alignment/bounds
 *
 * SSOT:
 *   - RECLAIMABLE => offset_bytes relativ zur Bulk-Base
 *   - RESIDENT    => offset_bytes relativ zur Core-Base
 *   - toc.header.total_bytes = tatsächlich gemappter Gesamtbereich
 * ============================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdatomic.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>

#include "cl_runtime/cl_runtime.h"
#include "cl_runtime/cl_abi_fingerprint.h" /* NEW: shared ABI fingerprint */

/* ---------------- Zeit ---------------- */
static uint64_t now_ns(void) {
    struct timespec ts;
    (void)clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

/* ---------------- Align ---------------- */
static size_t align_up(size_t v, size_t a) {
    if (a == 0) return v;
    size_t r = v % a;
    return r ? (v + (a - r)) : v;
}

/* ---------------- SHM helpers ---------------- */
static int shm_destroy_only(const char *name) {
    if (shm_unlink(name) != 0) {
        if (errno == ENOENT) return 0;
        return -1;
    }
    return 0;
}

static int shm_create_map_rw(const char *name, size_t sz, void **out_base, int *out_fd, int recreate) {
    if (recreate) {
        (void)shm_destroy_only(name);
    }

    int fd = shm_open(name, O_CREAT | O_RDWR, 0660);
    if (fd < 0) return -1;

    (void)fchmod(fd, 0660);

    if (ftruncate(fd, (off_t)sz) != 0) { close(fd); return -2; }

    void *p = mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) { close(fd); return -3; }

    *out_base = p;
    *out_fd = fd;
    return 0;
}

static void shm_unmap_close(void *base, size_t sz, int fd) {
    if (base && sz) (void)munmap(base, sz);
    if (fd >= 0) close(fd);
}

/* ---------------- TOC helpers ---------------- */
static void toc_init(cl_toc_t *toc, uint64_t total_bytes) {
    memset(toc, 0, sizeof(*toc));
    toc->header.version = CL_TOC_VERSION;
    toc->header.entry_count = 0;
    toc->header.total_bytes = total_bytes;
    toc->header.build_id = 0;
    toc->header.epoch = 1; /* TOC existiert als Struktur */
}

/* SSOT: entry epoch bleibt 0, bis CORE published */
static void toc_add(cl_toc_t *toc,
                    uint16_t type, uint16_t version,
                    uint32_t flags,
                    uint32_t stride_bytes, uint32_t count, uint64_t offset_bytes,
                    uint16_t source_flags, uint16_t access_flags,
                    uint16_t presence_flags, uint16_t layout_flags)
{
    if (toc->header.entry_count >= CL_TOC_MAX_ENTRIES) return;

    cl_toc_entry_t *e = &toc->entries[toc->header.entry_count++];
    memset(e, 0, sizeof(*e));

    e->type = type;
    e->version = version;
    e->flags = flags;

    e->stride_bytes = stride_bytes;
    e->count = count;
    e->offset_bytes = offset_bytes;

    e->source_flags = source_flags;
    e->access_flags = access_flags;
    e->presence_flags = presence_flags;
    e->layout_flags = layout_flags;

    e->gates_possible = 0;
    e->gates_effective = 0;

    e->epoch = 0;      /* UNINITIALIZED */
    e->reserved0 = 0;
}

/* ---------------- Genesis gate ---------------- */
static int is_allowed_stride(uint32_t s) {
    return (s == 256u) || (s == 512u) || (s == 1024u) || (s == 4096u);
}
static int check_bounds_u64(uint64_t off, uint64_t len, uint64_t total) {
    if (off > total) return 0;
    if (len > total) return 0;
    if (off + len > total) return 0;
    return 1;
}
static int gate_root_toc(const cl_root_t *r, const cl_toc_t *toc,
                         uint64_t core_total, uint64_t bulk_total, int bulk_present)
{
    if (r->root_magic != CL_ROOT_MAGIC_U32) return 100;
    if (r->schema_version != CL_SCHEMA_VERSION_U32) return 101;
    if (r->endian_magic != CL_ENDIAN_MAGIC_U32) return 102;

    if (r->toc_size < sizeof(cl_toc_header_t)) return 105;
    if (!check_bounds_u64(r->toc_offset, r->toc_size, core_total)) return 104;

    if (toc->header.entry_count > CL_TOC_MAX_ENTRIES) return 107;

    uint64_t need = (uint64_t)sizeof(cl_toc_header_t) +
                    (uint64_t)toc->header.entry_count * (uint64_t)sizeof(cl_toc_entry_t);
    if (need > r->toc_size) return 108;

    for (uint32_t i = 0; i < toc->header.entry_count; i++) {
        const cl_toc_entry_t *e = &toc->entries[i];
        uint32_t stride = (uint32_t)e->stride_bytes;
        uint32_t count  = (uint32_t)e->count;
        uint64_t off    = (uint64_t)e->offset_bytes;

        if (!is_allowed_stride(stride)) return 120;
        if ((off % (uint64_t)stride) != 0u) return 121;

        uint64_t seg_bytes = (uint64_t)stride * (uint64_t)count;
        const int in_bulk = ((e->flags & CL_TOC_FLAG_RECLAIMABLE) != 0u);

        if (!in_bulk) {
            if (!check_bounds_u64(off, seg_bytes, core_total)) return 122;
        } else {
            if (bulk_present) {
                if (!check_bounds_u64(off, seg_bytes, bulk_total)) return 123;
            }
        }
    }
    return 0;
}

/* ---------------- CLI ---------------- */
static void usage(const char *argv0) {
    printf("Usage:\n");
    printf("  %s              Core-only Genesis (ZERO)\n", argv0);
    printf("  %s --bulk       Core + Bulk Genesis\n", argv0);
    printf("  %s --destroy    shm_unlink core/bulk (MANUELL)\n", argv0);
    printf("  %s --recreate   wie normal, aber core/bulk vorher unlink (MANUELL)\n", argv0);
}

int main(int argc, char **argv) {
    int want_bulk = 0;
    int destroy = 0;
    int recreate = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--bulk") == 0) want_bulk = 1;
        else if (strcmp(argv[i], "--destroy") == 0) destroy = 1;
        else if (strcmp(argv[i], "--recreate") == 0) recreate = 1;
        else if (strcmp(argv[i], "--help") == 0) { usage(argv[0]); return 0; }
        else { usage(argv[0]); return 1; }
    }

    if (destroy) {
        (void)shm_destroy_only(CL_CORE_SHM_NAME);
        (void)shm_destroy_only(CL_BULK_SHM_NAME);
        printf("OK: destroyed %s and %s\n", CL_CORE_SHM_NAME, CL_BULK_SHM_NAME);
        return 0;
    }

    const uint64_t boot_ns = now_ns();

    const size_t toc_off = CL_SHM_PAGE_BYTES;
    const size_t toc_sz  = CL_SHM_PAGE_BYTES;

    size_t off = toc_off + toc_sz;
    off = align_up(off, 256);

    const uint32_t cpu_count = 1;
    const uint32_t nic_count = 1;
    const uint32_t neighbor_count = 32;
    const uint32_t peer_count = 32;

    /* Core resident 256 blocks */
    size_t off_link      = align_up(off, 256); off += 256;
    size_t off_budget    = align_up(off, 256); off += 256;
    size_t off_mem       = align_up(off, 256); off += 256;
    size_t off_overlay   = align_up(off, 256); off += 256;
    size_t off_watchdog  = align_up(off, 256); off += 256;
    size_t off_time      = align_up(off, 256); off += 256;
    size_t off_pcie      = align_up(off, 256); off += 256;

    /* Pflicht resident */
    size_t off_service   = align_up(off, 256); off += 256;
    size_t off_identity  = align_up(off, 256); off += 256;
    size_t off_flow_cmd  = align_up(off, 256); off += 256;

    /* CPU/NIC/BOARD */
    size_t off_cpu       = align_up(off, 1024); off = off_cpu + (size_t)cpu_count * 1024;
    size_t off_nic       = align_up(off, 512);  off = off_nic + (size_t)nic_count * 512;
    size_t off_board     = align_up(off, 512);  off = off_board + 512;

    /* Mesh arrays */
    size_t off_neighbor  = align_up(off, 256);  off = off_neighbor + (size_t)neighbor_count * 256;
    size_t off_peer      = align_up(off, 256);  off = off_peer     + (size_t)peer_count * 256;

    /* ZFS resident */
    size_t off_zfs       = align_up(off, 4096); off = off_zfs + 4096;

    const size_t core_total = align_up(off, 4096);

    /* Bulk layout offsets: always defined */
    size_t bulk_total = 0;
    size_t bulk_off_dma  = 0;
    size_t bulk_off_hist = 0;
    size_t bulk_off_for  = 0;
    size_t bulk_off_oracle = 0;

    {
        size_t boff = 0;
        boff = align_up(boff, 4096);
        bulk_off_dma = align_up(boff, 4096);  boff = bulk_off_dma + 4096;
        bulk_off_hist = align_up(boff, 4096); boff = bulk_off_hist + 4096;
        bulk_off_for = align_up(boff, 4096);  boff = bulk_off_for + 4096;
        bulk_off_oracle = align_up(boff, 256); boff = bulk_off_oracle + 256;
        bulk_total = align_up(boff, 4096);
    }

    void *core_base = NULL;
    int core_fd = -1;
    int rc = shm_create_map_rw(CL_CORE_SHM_NAME, core_total, &core_base, &core_fd, recreate);
    if (rc != 0) {
        printf("FAIL: create/map core shm (%s): rc=%d errno=%d (%s)\n",
               CL_CORE_SHM_NAME, rc, errno, strerror(errno));
        return 2;
    }

    void *bulk_base = NULL;
    int bulk_fd = -1;
    if (want_bulk) {
        rc = shm_create_map_rw(CL_BULK_SHM_NAME, bulk_total, &bulk_base, &bulk_fd, recreate);
        if (rc != 0) {
            printf("FAIL: create/map bulk shm (%s): rc=%d errno=%d (%s)\n",
                   CL_BULK_SHM_NAME, rc, errno, strerror(errno));
            shm_unmap_close(core_base, core_total, core_fd);
            return 3;
        }
    }

    memset(core_base, 0, core_total);
    if (want_bulk) memset(bulk_base, 0, bulk_total);

    cl_root_t *root = (cl_root_t*)core_base;
    cl_toc_t  *toc  = (cl_toc_t*)((uint8_t*)core_base + toc_off);

    /* Root: Geometrie/Validierung + ABI Fingerprint */
    root->root_magic = CL_ROOT_MAGIC_U32;
    root->root_flags = CL_ROOT_FLAG_FAILFAST_ABI;
    root->schema_version = CL_SCHEMA_VERSION_U32;
    root->endian_magic = CL_ENDIAN_MAGIC_U32;

    root->abi_layout_checksum = cl_abi_fingerprint_u64(); /* NEW: real ABI checksum */
    atomic_store(&root->seq_cnt, 0u);
    atomic_store(&root->uptime_ns, 0u);
    atomic_store(&root->bulk_epoch, 0u);

    root->boot_id_ns = boot_ns;
    root->toc_offset = (uint64_t)toc_off;
    root->toc_size   = (uint64_t)toc_sz;

    atomic_store(&root->bulk_present, want_bulk ? 1u : 0u);

    /* SSOT: total_bytes = actually mapped total bytes */
    toc_init(toc, (uint64_t)(core_total + (want_bulk ? bulk_total : 0u)));
    toc->header.build_id = root->abi_layout_checksum; /* helpful for tools */

    /* Core resident entries */
    toc_add(toc, (uint16_t)CL_LINK_SEG_256, 1,
            CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_HOT | CL_TOC_FLAG_CONTROL,
            256, 1, (uint64_t)off_link,
            CL_TOC_SOURCE_DIRECT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
            CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE);

    toc_add(toc, (uint16_t)CL_BUDGET_SEG_256, 1,
            CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_HOT | CL_TOC_FLAG_CONTROL,
            256, 1, (uint64_t)off_budget,
            CL_TOC_SOURCE_DIRECT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
            CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE);

    toc_add(toc, (uint16_t)CL_MEM_SEG_256, 1,
            CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_WARM | CL_TOC_FLAG_DATA,
            256, 1, (uint64_t)off_mem,
            CL_TOC_SOURCE_BEST_EFFORT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
            CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE);

    toc_add(toc, (uint16_t)CL_OVERLAY_SEG_256, 1,
            CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_WARM | CL_TOC_FLAG_CONTROL,
            256, 1, (uint64_t)off_overlay,
            CL_TOC_SOURCE_BEST_EFFORT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
            CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE);

    toc_add(toc, (uint16_t)CL_WATCHDOG_SEG_256, 1,
            CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_HOT | CL_TOC_FLAG_CONTROL | CL_TOC_FLAG_FORENSICS,
            256, 1, (uint64_t)off_watchdog,
            CL_TOC_SOURCE_DIRECT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE,
            CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE);

    toc_add(toc, (uint16_t)CL_TIME_SEG_256, 1,
            CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_WARM | CL_TOC_FLAG_CONTROL,
            256, 1, (uint64_t)off_time,
            CL_TOC_SOURCE_DIRECT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE,
            CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE);

    toc_add(toc, (uint16_t)CL_PCIE_SEG_256, 1,
            CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_WARM | CL_TOC_FLAG_DATA,
            256, 1, (uint64_t)off_pcie,
            CL_TOC_SOURCE_BEST_EFFORT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE,
            CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE);

    toc_add(toc, (uint16_t)CL_SERVICE_SEG_256, 1,
            CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_HOT | CL_TOC_FLAG_CONTROL | CL_TOC_FLAG_CRITICAL,
            256, 1, (uint64_t)off_service,
            CL_TOC_SOURCE_DIRECT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
            CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE);

    toc_add(toc, (uint16_t)CL_IDENTITY_SEG_256, 1,
            CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_HOT | CL_TOC_FLAG_CONTROL | CL_TOC_FLAG_CRITICAL,
            256, 1, (uint64_t)off_identity,
            CL_TOC_SOURCE_DIRECT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
            CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE);

    toc_add(toc, (uint16_t)CL_FLOW_CMD_SEG_256, 1,
            CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_HOT | CL_TOC_FLAG_CONTROL | CL_TOC_FLAG_CRITICAL,
            256, 1, (uint64_t)off_flow_cmd,
            CL_TOC_SOURCE_DIRECT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
            CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE);

    toc_add(toc, (uint16_t)CL_CPU_SEG_1024, 1,
            CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_HOT | CL_TOC_FLAG_DATA | CL_TOC_FLAG_FIXED_COUNT,
            1024, cpu_count, (uint64_t)off_cpu,
            CL_TOC_SOURCE_DIRECT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
            CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE);

    toc_add(toc, (uint16_t)CL_NIC_SEG_512, 1,
            CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_HOT | CL_TOC_FLAG_DATA | CL_TOC_FLAG_FIXED_COUNT,
            512, nic_count, (uint64_t)off_nic,
            CL_TOC_SOURCE_BEST_EFFORT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
            CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE);

    toc_add(toc, (uint16_t)CL_BOARD_SEG_512, 1,
            CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_WARM | CL_TOC_FLAG_DATA,
            512, 1, (uint64_t)off_board,
            CL_TOC_SOURCE_BEST_EFFORT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE,
            CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE);

    toc_add(toc, (uint16_t)CL_MESH_NEIGHBOR_SEG_256, 1,
            CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_HOT | CL_TOC_FLAG_DATA | CL_TOC_FLAG_FIXED_COUNT,
            256, neighbor_count, (uint64_t)off_neighbor,
            CL_TOC_SOURCE_BEST_EFFORT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
            CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE | CL_TOC_LAYOUT_ARRAY);

    toc_add(toc, (uint16_t)CL_MESH_PEER_SEG_256, 1,
            CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_HOT | CL_TOC_FLAG_DATA | CL_TOC_FLAG_FIXED_COUNT,
            256, peer_count, (uint64_t)off_peer,
            CL_TOC_SOURCE_BEST_EFFORT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
            CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE | CL_TOC_LAYOUT_ARRAY);

    toc_add(toc, (uint16_t)CL_ZFS_SEG_4096, 1,
            CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_COLD | CL_TOC_FLAG_DATA,
            4096, 1, (uint64_t)off_zfs,
            CL_TOC_SOURCE_BEST_EFFORT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
            CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE);

    /* Bulk entries always present in TOC */
    const uint16_t bulk_presence = want_bulk ? CL_TOC_PRESENT : CL_TOC_RECLAIMED;

    toc_add(toc, (uint16_t)CL_DMA_SEG_4096, 1,
            CL_TOC_FLAG_RECLAIMABLE | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_HOT | CL_TOC_FLAG_DATA,
            4096, 1, (uint64_t)bulk_off_dma,
            CL_TOC_SOURCE_DIRECT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
            bulk_presence, CL_TOC_LAYOUT_CACHELINE);

    toc_add(toc, (uint16_t)CL_HISTORY_SEG_4096, 1,
            CL_TOC_FLAG_RECLAIMABLE | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_COLD | CL_TOC_FLAG_FORENSICS,
            4096, 1, (uint64_t)bulk_off_hist,
            CL_TOC_SOURCE_BEST_EFFORT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE,
            bulk_presence, CL_TOC_LAYOUT_CACHELINE);

    toc_add(toc, (uint16_t)CL_FORENSICS_SEG_4096, 1,
            CL_TOC_FLAG_RECLAIMABLE | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_COLD | CL_TOC_FLAG_FORENSICS,
            4096, 1, (uint64_t)bulk_off_for,
            CL_TOC_SOURCE_BEST_EFFORT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE,
            bulk_presence, CL_TOC_LAYOUT_CACHELINE);

    /* Oracle optional: reclaimable; presence=RECLAIMED when bulk absent */
    toc_add(toc, (uint16_t)CL_ORACLE_SEG_256, 1,
            CL_TOC_FLAG_RECLAIMABLE | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_COLD | CL_TOC_FLAG_DATA,
            256, 1, (uint64_t)bulk_off_oracle,
            CL_TOC_SOURCE_BEST_EFFORT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE,
            bulk_presence, CL_TOC_LAYOUT_CACHELINE);

    msync(core_base, core_total, MS_SYNC);
    if (want_bulk) msync(bulk_base, bulk_total, MS_SYNC);

    int gate = gate_root_toc(root, toc, (uint64_t)core_total, (uint64_t)bulk_total, want_bulk);
    if (gate != 0) {
        printf("FAIL: clinit gate failed: code=%d\n", gate);
        shm_unmap_close(core_base, core_total, core_fd);
        if (want_bulk) shm_unmap_close(bulk_base, bulk_total, bulk_fd);
        return 10;
    }

    const uint32_t toc_entries = toc->header.entry_count;

    shm_unmap_close(core_base, core_total, core_fd);
    if (want_bulk) shm_unmap_close(bulk_base, bulk_total, bulk_fd);

    printf("OK: clinit created core=%s size=%zu\n", CL_CORE_SHM_NAME, core_total);
    if (want_bulk) printf("OK: clinit created bulk=%s size=%zu\n", CL_BULK_SHM_NAME, bulk_total);
    printf("TOC entries: %u\n", (unsigned)toc_entries);
    return 0;
}


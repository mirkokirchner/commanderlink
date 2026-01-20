/*
 * ============================================================================
 * CommanderLink – ABI / Layout Report Tool (Truth Meter) – FINAL Endausbau
 * Datei: tools/cl_abi_report/cl_abi_report.c
 * ============================================================================
 *
 * Zweck:
 *   - Compile-time ABI: Größen/Alignments/Offsets prüfen (muss immer grün)
 *   - Runtime ABI (optional): Root + TOC + Segment-Geometrie gegen SHM prüfen
 *
 * Maschinenregeln:
 *   - Keine Heuristik. Keine Policy. Nur Verifikation.
 *   - Wenn Root FAILFAST setzt, ist ABI-Mismatch hartes Fail.
 *
 * Plattform:
 *   - Linux / BSD / OmniOS (POSIX shm_open + mmap)
 * ============================================================================
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdalign.h>
#include <stdatomic.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "../../include/cl_runtime/cl_runtime.h"
#include "../../include/cl_runtime/cl_abi_fingerprint.h"

/* --------------------------- Hash (FNV-1a 64) ---------------------------- */

static uint64_t fnv1a64_init(void) { return 1469598103934665603ull; }
static uint64_t fnv1a64_u64(uint64_t h, uint64_t v) {
    for (int i = 0; i < 8; i++) {
        uint8_t b = (uint8_t)((v >> (i * 8)) & 0xffu);
        h ^= (uint64_t)b;
        h *= 1099511628211ull;
    }
    return h;
}
static uint64_t fnv1a64_u32(uint64_t h, uint32_t v) { return fnv1a64_u64(h, (uint64_t)v); }
static uint64_t fnv1a64_sz(uint64_t h, size_t v) { return fnv1a64_u64(h, (uint64_t)v); }

/* ABI-Fingerprint: compile-time Wahrheiten
 * NOTE: kept for forensic cross-checks; not used in Stage-1 default flow.
 */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((unused))
#endif
static uint64_t cl_compute_abi_fingerprint(void) {
    uint64_t h = fnv1a64_init();

    /* Schema */
    h = fnv1a64_u32(h, (uint32_t)CL_SCHEMA_VERSION_U32);
    h = fnv1a64_u32(h, (uint32_t)CL_ENDIAN_MAGIC_U32);
    h = fnv1a64_u32(h, (uint32_t)CL_SHM_PAGE_BYTES);

    /* Root/TOC sizes */
    h = fnv1a64_sz(h, sizeof(cl_root_t));
    h = fnv1a64_sz(h, sizeof(cl_toc_header_t));
    h = fnv1a64_sz(h, sizeof(cl_toc_entry_t));
    h = fnv1a64_u32(h, (uint32_t)CL_TOC_MAX_ENTRIES);
    h = fnv1a64_u32(h, (uint32_t)CL_TOC_VERSION);

    /* Segment sizes (SSOT = struct sizes) */
    h = fnv1a64_sz(h, sizeof(cl_cpu_seg_1024_t));
    h = fnv1a64_sz(h, sizeof(cl_budget_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_mem_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_link_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_overlay_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_watchdog_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_pcie_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_time_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_mesh_neighbor_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_mesh_peer_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_oracle_seg_256_t));

    h = fnv1a64_sz(h, sizeof(cl_nic_seg_512_t));
    h = fnv1a64_sz(h, sizeof(cl_board_seg_512_t));

    h = fnv1a64_sz(h, sizeof(cl_dma_seg_4096_t));
    h = fnv1a64_sz(h, sizeof(cl_history_seg_4096_t));
    h = fnv1a64_sz(h, sizeof(cl_forensics_seg_4096_t));
    h = fnv1a64_sz(h, sizeof(cl_zfs_seg_4096_t));

    /* NEW: Service / Identity / FlowCmd (critical) */
    h = fnv1a64_sz(h, sizeof(cl_service_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_identity_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_flow_cmd_seg_256_t));

    /* Alignments (selected) */
    h = fnv1a64_sz(h, (size_t)alignof(cl_root_t));
    h = fnv1a64_sz(h, (size_t)alignof(cl_cpu_seg_1024_t));
    h = fnv1a64_sz(h, (size_t)alignof(cl_dma_seg_4096_t));
    h = fnv1a64_sz(h, (size_t)alignof(cl_zfs_seg_4096_t));

    /* NEW: alignments for critical 256 segments */
    h = fnv1a64_sz(h, (size_t)alignof(cl_service_seg_256_t));
    h = fnv1a64_sz(h, (size_t)alignof(cl_identity_seg_256_t));
    h = fnv1a64_sz(h, (size_t)alignof(cl_flow_cmd_seg_256_t));

    /* Critical offsets: Root */
    h = fnv1a64_sz(h, offsetof(cl_root_t, abi_layout_checksum));
    h = fnv1a64_sz(h, offsetof(cl_root_t, toc_offset));
    h = fnv1a64_sz(h, offsetof(cl_root_t, toc_size));
    h = fnv1a64_sz(h, offsetof(cl_root_t, schema_version));
    h = fnv1a64_sz(h, offsetof(cl_root_t, endian_magic));
    h = fnv1a64_sz(h, offsetof(cl_root_t, root_magic));
    h = fnv1a64_sz(h, offsetof(cl_root_t, root_flags));
    h = fnv1a64_sz(h, offsetof(cl_root_t, bulk_present));
    h = fnv1a64_sz(h, offsetof(cl_root_t, bulk_epoch));

    /* Critical offsets: TOC entry */
    h = fnv1a64_sz(h, offsetof(cl_toc_entry_t, type));
    h = fnv1a64_sz(h, offsetof(cl_toc_entry_t, flags));
    h = fnv1a64_sz(h, offsetof(cl_toc_entry_t, stride_bytes));
    h = fnv1a64_sz(h, offsetof(cl_toc_entry_t, count));
    h = fnv1a64_sz(h, offsetof(cl_toc_entry_t, offset_bytes));
    h = fnv1a64_sz(h, offsetof(cl_toc_entry_t, source_flags));
    h = fnv1a64_sz(h, offsetof(cl_toc_entry_t, access_flags));
    h = fnv1a64_sz(h, offsetof(cl_toc_entry_t, presence_flags));
    h = fnv1a64_sz(h, offsetof(cl_toc_entry_t, layout_flags));
    h = fnv1a64_sz(h, offsetof(cl_toc_entry_t, gates_possible));
    h = fnv1a64_sz(h, offsetof(cl_toc_entry_t, gates_effective));
    h = fnv1a64_sz(h, offsetof(cl_toc_entry_t, epoch));

    /* NEW: Critical offsets: Service global truth/control */
    h = fnv1a64_sz(h, offsetof(cl_service_seg_256_t, g0.desired_profile));
    h = fnv1a64_sz(h, offsetof(cl_service_seg_256_t, g0.desired_redirect));
    h = fnv1a64_sz(h, offsetof(cl_service_seg_256_t, g0.effective_redirect));
    h = fnv1a64_sz(h, offsetof(cl_service_seg_256_t, g0.redirect_allowed));

    return h;
}

/* ---------------------------- Helpers ----------------------------------- */

static int is_allowed_stride(uint32_t s) {
    return (s == 256u) || (s == 512u) || (s == 1024u) || (s == 4096u);
}

static int check_bounds_u64(uint64_t off, uint64_t len, uint64_t total) {
    if (off > total) return 0;
    if (len > total) return 0;
    if (off + len > total) return 0;
    return 1;
}

static int map_shm_ro(const char *name, void **out_base, size_t *out_sz) {
    int fd = shm_open(name, O_RDONLY, 0);
    if (fd < 0) return -1;

    struct stat st;
    if (fstat(fd, &st) != 0) {
        close(fd);
        return -2;
    }
    if (st.st_size <= 0) {
        close(fd);
        return -3;
    }

    void *p = mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);
    if (p == MAP_FAILED) return -4;

    *out_base = p;
    *out_sz = (size_t)st.st_size;
    return 0;
}

static void unmap_shm(void *base, size_t sz) {
    if (base && sz) (void)munmap(base, sz);
}

/* ---------------------------- Reports ----------------------------------- */

static int report_compile_time_sizes(void) {
    int ok = 1;

    if (sizeof(cl_root_t) != 4096u) ok = 0;
    if (sizeof(cl_toc_header_t) != 64u) ok = 0;
    if (sizeof(cl_toc_entry_t) != 64u) ok = 0;

    /* 256 */
    if (sizeof(cl_budget_seg_256_t) != 256u) ok = 0;
    if (sizeof(cl_mem_seg_256_t) != 256u) ok = 0;
    if (sizeof(cl_link_seg_256_t) != 256u) ok = 0;
    if (sizeof(cl_time_seg_256_t) != 256u) ok = 0;
    if (sizeof(cl_overlay_seg_256_t) != 256u) ok = 0;
    if (sizeof(cl_watchdog_seg_256_t) != 256u) ok = 0;
    if (sizeof(cl_pcie_seg_256_t) != 256u) ok = 0;
    if (sizeof(cl_mesh_neighbor_seg_256_t) != 256u) ok = 0;
    if (sizeof(cl_mesh_peer_seg_256_t) != 256u) ok = 0;
    if (sizeof(cl_oracle_seg_256_t) != 256u) ok = 0;

    /* NEW: critical 256 */
    if (sizeof(cl_service_seg_256_t) != 256u) ok = 0;
    if (sizeof(cl_identity_seg_256_t) != 256u) ok = 0;
    if (sizeof(cl_flow_cmd_seg_256_t) != 256u) ok = 0;

    /* 512 */
    if (sizeof(cl_nic_seg_512_t) != 512u) ok = 0;
    if (sizeof(cl_board_seg_512_t) != 512u) ok = 0;

    /* 1024 */
    if (sizeof(cl_cpu_seg_1024_t) != 1024u) ok = 0;

    /* 4096 */
    if (sizeof(cl_dma_seg_4096_t) != 4096u) ok = 0;
    if (sizeof(cl_history_seg_4096_t) != 4096u) ok = 0;
    if (sizeof(cl_forensics_seg_4096_t) != 4096u) ok = 0;
    if (sizeof(cl_zfs_seg_4096_t) != 4096u) ok = 0;

    if (!ok) {
        printf("CT FAIL: size drift detected.\n");
        return 1;
    }

    printf("CT OK: sizes stable.\n");
    printf("ABI fingerprint (CT): 0x%016" PRIx64 "\n", cl_abi_fingerprint_u64());
    return 0;
}

/* RT FAIL error codes (v1.0)
 * 10 root magic
 * 11 endian magic
 * 12 schema version
 * 13 abi checksum mismatch + failfast
 * 14 toc bounds
 * 15 toc_size < toc_header
 * 16 entry_count > max
 * 17 toc entries don't fit
 * 18 stride illegal
 * 19 offset not stride-aligned
 * 20 core bounds
 * 21 bulk bounds
 * 22 toc.total_bytes mismatch
 * 23 bulk absent but not RECLAIMED
 * 24 bulk present but not PRESENT/PARTIAL
 * 25 bulk mapped but root.bulk_present==0
 * 26 bulk not mapped but root.bulk_present!=0
 */
static int report_runtime_shm(void *core_base, size_t core_sz,
                             void *bulk_base, size_t bulk_sz, int bulk_mapped) {
    (void)bulk_base;

    const cl_root_t *r = (const cl_root_t*)core_base;

    if (r->root_magic != CL_ROOT_MAGIC_U32) return 10;
    if (r->endian_magic != CL_ENDIAN_MAGIC_U32) return 11;
    if (r->schema_version != CL_SCHEMA_VERSION_U32) return 12;

    uint64_t want = cl_abi_fingerprint_u64();
    if (r->abi_layout_checksum != want) {
        if ((r->root_flags & CL_ROOT_FLAG_FAILFAST_ABI) != 0u) return 13;
    }

    if (!check_bounds_u64(r->toc_offset, r->toc_size, (uint64_t)core_sz)) return 14;
    if (r->toc_size < sizeof(cl_toc_header_t)) return 15;

    const cl_toc_t *toc = (const cl_toc_t*)((const uint8_t*)core_base + (size_t)r->toc_offset);
    if (toc->header.entry_count > CL_TOC_MAX_ENTRIES) return 16;

    uint64_t need = (uint64_t)sizeof(cl_toc_header_t) +
                    (uint64_t)toc->header.entry_count * (uint64_t)sizeof(cl_toc_entry_t);
    if (need > r->toc_size) return 17;

    /* SSOT: toc->header.total_bytes = actually mapped total bytes */
    uint64_t expected_total = bulk_mapped ? ((uint64_t)core_sz + (uint64_t)bulk_sz)
                                          : (uint64_t)core_sz;
    if (toc->header.total_bytes != expected_total) return 22;

    /* root.bulk_present must match bulk mapping state */
    uint32_t root_bulk = atomic_load_explicit(&r->bulk_present, memory_order_relaxed);
    if (bulk_mapped) {
        if (root_bulk == 0u) return 25;
    } else {
        if (root_bulk != 0u) return 26;
    }

    for (uint32_t i = 0; i < toc->header.entry_count; i++) {
        const cl_toc_entry_t *e = &toc->entries[i];
        uint32_t stride = (uint32_t)e->stride_bytes;
        uint32_t count  = (uint32_t)e->count;
        uint64_t off    = (uint64_t)e->offset_bytes;

        if (!is_allowed_stride(stride)) return 18;

        uint64_t seg_bytes = (uint64_t)stride * (uint64_t)count;
        if ((off % (uint64_t)stride) != 0u) return 19;

        const int in_bulk = ((e->flags & CL_TOC_FLAG_RECLAIMABLE) != 0u);
        if (!in_bulk) {
            if (!check_bounds_u64(off, seg_bytes, (uint64_t)core_sz)) return 20;
        } else {
            if (!bulk_mapped) {
                if ((e->presence_flags & CL_TOC_RECLAIMED) == 0u) return 23;
            } else {
                if ((e->presence_flags & (CL_TOC_PRESENT | CL_TOC_PARTIAL)) == 0u) return 24;
                if (!check_bounds_u64(off, seg_bytes, (uint64_t)bulk_sz)) return 21;
            }
        }
    }

    return 0;
}

int main(void) {
    /* compile-time ABI must be green */
    int ct = report_compile_time_sizes();
    if (ct != 0) return ct;

    /* runtime check is optional; if no SHM exists, exit clean */
    void *core_base = NULL;
    size_t core_sz = 0;
    int rc = map_shm_ro(CL_CORE_SHM_NAME, &core_base, &core_sz);
    if (rc != 0) {
        printf("RT INFO: core shm not mapped (%s): rc=%d errno=%d (%s); compile-time ABI is green.\n",
               CL_CORE_SHM_NAME, rc, errno, strerror(errno));
        return 0;
    }

    void *bulk_base = NULL;
    size_t bulk_sz = 0;
    int bulk_mapped = (map_shm_ro(CL_BULK_SHM_NAME, &bulk_base, &bulk_sz) == 0);

    int rt = report_runtime_shm(core_base, core_sz, bulk_base, bulk_sz, bulk_mapped);

    unmap_shm(core_base, core_sz);
    if (bulk_mapped) unmap_shm(bulk_base, bulk_sz);

    if (rt == 0) {
        printf("RT OK: root/toc/segments geometry validated.\n");
        return 0;
    }
    printf("RT FAIL: code=%d\n", rt);
    return rt;
}


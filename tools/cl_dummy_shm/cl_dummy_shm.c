/*
 * ============================================================================
 * CommanderLink – Dummy SHM Generator (Core-SHM) – FINAL Endausbau
 * Datei: tools/cl_dummy_shm/cl_dummy_shm.c
 * ============================================================================
 *
 * Zweck:
 *   - Erzeugt /cl_core_root (Core-SHM) mit Root + TOC + Segment-Records
 *   - Initialisiert einen minimalen, konsistenten Runtime-Snapshot
 *   - Dient NUR für Tests/Bringup (Monitor/ABI-Report/shm_dump), nicht produktiv
 *
 * Maschinenregel:
 *   - Host-Order im SHM
 *   - Keine packed Layouts
 *   - Stride/Offsets exakt wie in den Headern
 *   - TOC ist Header+Entries (keine Legacy-Felder)
 *
 * Hinweis:
 *   - Bulk-SHM wird hier NICHT erzeugt. bulk_present = 0.
 * ============================================================================
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>

#include "../../include/cl_runtime/cl_runtime.h"

/* --------------------------------------------------------------------------
 * Zeit
 * -------------------------------------------------------------------------- */
static uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

/* --------------------------------------------------------------------------
 * Hash (FNV-1a 64) – identisch zum ABI-Report
 * -------------------------------------------------------------------------- */
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

static uint64_t cl_compute_abi_fingerprint(void) {
    uint64_t h = fnv1a64_init();

    /* Schema */
    h = fnv1a64_u32(h, (uint32_t)CL_SCHEMA_VERSION_U32);
    h = fnv1a64_u32(h, (uint32_t)CL_ENDIAN_MAGIC_U32);
    h = fnv1a64_u32(h, (uint32_t)CL_SHM_PAGE_BYTES);

    /* Root / TOC */
    h = fnv1a64_sz(h, sizeof(cl_root_t));
    h = fnv1a64_sz(h, sizeof(cl_toc_header_t));
    h = fnv1a64_sz(h, sizeof(cl_toc_entry_t));
    h = fnv1a64_u32(h, (uint32_t)CL_TOC_MAX_ENTRIES);
    h = fnv1a64_u32(h, (uint32_t)CL_TOC_VERSION);

    /* Segmentgrößen (Strides) */
    h = fnv1a64_sz(h, sizeof(cl_seg_cpu_1024_t));
    h = fnv1a64_sz(h, sizeof(cl_budget_seg_256_t));     /* Budget */
    h = fnv1a64_sz(h, sizeof(cl_seg_mem_256_t));        /* Memory */
    h = fnv1a64_sz(h, sizeof(cl_seg_nic_512_t));        /* NIC */
    h = fnv1a64_sz(h, sizeof(cl_seg_link_256_t));       /* Link Summary */
    h = fnv1a64_sz(h, sizeof(cl_overlay_seg_256_t));    /* Overlay */
    h = fnv1a64_sz(h, sizeof(cl_watchdog_seg_256_t));   /* Watchdog */
    h = fnv1a64_sz(h, sizeof(cl_seg_pcie_256_t));       /* PCIe */
    h = fnv1a64_sz(h, sizeof(cl_seg_board_512_t));      /* Board */
    h = fnv1a64_sz(h, sizeof(cl_seg_time_256_t));       /* Timebase */
    h = fnv1a64_sz(h, sizeof(cl_seg_mesh_neighbor_256_t)); /* Hot Neighbor */
    h = fnv1a64_sz(h, sizeof(cl_seg_zfs_4096_t));       /* ZFS Summary */

    /* Alignments */
    h = fnv1a64_sz(h, (size_t)alignof(cl_root_t));
    h = fnv1a64_sz(h, (size_t)alignof(cl_seg_cpu_1024_t));
    h = fnv1a64_sz(h, (size_t)alignof(cl_seg_zfs_4096_t));

    /* Root Offsets (kritisch) */
    h = fnv1a64_sz(h, offsetof(cl_root_t, abi_layout_checksum));
    h = fnv1a64_sz(h, offsetof(cl_root_t, toc_offset));
    h = fnv1a64_sz(h, offsetof(cl_root_t, toc_size));
    h = fnv1a64_sz(h, offsetof(cl_root_t, schema_version));
    h = fnv1a64_sz(h, offsetof(cl_root_t, endian_magic));
    h = fnv1a64_sz(h, offsetof(cl_root_t, root_magic));
    h = fnv1a64_sz(h, offsetof(cl_root_t, root_flags));
    h = fnv1a64_sz(h, offsetof(cl_root_t, bulk_present));
    h = fnv1a64_sz(h, offsetof(cl_root_t, bulk_epoch));

    /* TOC Entry Offsets */
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

    return h;
}

/* --------------------------------------------------------------------------
 * Alignment Helper
 * -------------------------------------------------------------------------- */
static size_t align_up(size_t v, size_t a) {
    if (a == 0) return v;
    size_t r = v % a;
    return r ? (v + (a - r)) : v;
}

/* --------------------------------------------------------------------------
 * TOC Helper (neues Layout)
 * -------------------------------------------------------------------------- */
static void toc_init(cl_toc_t *toc, uint64_t total_bytes, uint64_t build_id) {
    memset(toc, 0, sizeof(*toc));
    toc->header.version = CL_TOC_VERSION;
    toc->header.entry_count = 0;
    toc->header.total_bytes = total_bytes;
    toc->header.build_id = build_id;
    toc->header.epoch = 1;
}

static cl_toc_entry_t* toc_add(cl_toc_t *toc,
                               cl_segment_type_t type,
                               uint16_t seg_version,
                               cl_toc_flags_t flags,
                               uint32_t stride_bytes,
                               uint32_t count,
                               uint64_t offset_bytes,
                               uint16_t source_flags,
                               uint16_t access_flags,
                               uint16_t presence_flags,
                               uint16_t layout_flags,
                               uint64_t gates_possible,
                               uint64_t gates_effective,
                               uint64_t epoch)
{
    if (toc->header.entry_count >= CL_TOC_MAX_ENTRIES) return NULL;

    cl_toc_entry_t *e = &toc->entries[toc->header.entry_count++];
    memset(e, 0, sizeof(*e));

    e->type = (uint16_t)type;
    e->version = seg_version;
    e->flags = (uint32_t)flags;

    e->stride_bytes = stride_bytes;
    e->count = count;
    e->offset_bytes = offset_bytes;

    e->source_flags = source_flags;
    e->access_flags = access_flags;
    e->presence_flags = presence_flags;
    e->layout_flags = layout_flags;

    e->gates_possible = (cl_gate_mask_t)gates_possible;
    e->gates_effective = (cl_gate_mask_t)gates_effective;

    e->epoch = epoch;
    e->reserved0 = 0;

    return e;
}

/* --------------------------------------------------------------------------
 * Minimal Fill Helpers (keine Heuristik)
 * -------------------------------------------------------------------------- */
static void fill_budget(cl_budget_seg_256_t *b) {
    /* Alle Felder deterministisch: nur ein Snapshot */
    b->cl0_ident.v.epoch = 1;
    b->cl0_ident.v.tsc_last_update = 0;
    b->cl0_ident.v.window_ns = 250000000ull; /* 250ms window als Beispiel */
    b->cl0_ident.v.window_start_tsc = 0;

    b->cl1_capacity.v.cpu_possible_ns = 250000000ull;
    b->cl1_capacity.v.cpu_effective_ns = 250000000ull;
    b->cl1_capacity.v.net_possible_ns = 250000000ull;
    b->cl1_capacity.v.net_effective_ns = 250000000ull;

    b->cl2_usage.v.cpu_used_ns = 0;
    b->cl2_usage.v.net_used_ns = 0;
    b->cl2_usage.v.stall_events = 0;
    b->cl2_usage.v.interactive_events = 0;

    b->cl3_state.v.gates.possible = 0;
    b->cl3_state.v.gates.effective = 0;
    b->cl3_state.v.gates_seen = 0;
    b->cl3_state.v.reclaim_events = 0;
}

static void fill_mem(cl_seg_mem_256_t *m) {
    atomic_store(&m->hot.total_bytes, 32ull * 1024ull * 1024ull * 1024ull);
    atomic_store(&m->hot.used_bytes,  8ull * 1024ull * 1024ull * 1024ull);
    atomic_store(&m->hot.free_bytes, 24ull * 1024ull * 1024ull * 1024ull);
    atomic_store(&m->hot.ecc_err_cnt, 0u);

    atomic_store(&m->hot.pressure_permille, 50u);
    atomic_store(&m->hot.reason_code, 0u);

    m->hot.src = 0;
    m->hot.qual = 0;

    atomic_store(&m->warm.bw_read_bps, 0u);
    atomic_store(&m->warm.bw_write_bps, 0u);
    atomic_store(&m->warm.timing_tcl, 0u);
    atomic_store(&m->warm.timing_trcd, 0u);
    atomic_store(&m->warm.timing_trp, 0u);
    atomic_store(&m->warm.timing_tras, 0u);
    atomic_store(&m->warm.channel_state, 0u);
    atomic_store(&m->warm.reclaim_events, 0u);
    atomic_store(&m->warm.last_update_ns, now_ns());

    memset(m->cold.dimm_mode, 0, sizeof(m->cold.dimm_mode));
    memset(m->cold.channel_mode, 0, sizeof(m->cold.channel_mode));
    strncpy(m->cold.dimm_mode, "DDR", sizeof(m->cold.dimm_mode)-1);
    strncpy(m->cold.channel_mode, "DUAL", sizeof(m->cold.channel_mode)-1);
    m->cold.src = 0;
    m->cold.qual = 0;

    atomic_store(&m->rsv.gates_possible, 0u);
    atomic_store(&m->rsv.gates_effective, 0u);
    atomic_store(&m->rsv.gates_seen, 0u);
    atomic_store(&m->rsv.oom_events, 0u);
    atomic_store(&m->rsv.reclaim_failures, 0u);
    atomic_store(&m->rsv.epoch, 1u);
    atomic_store(&m->rsv.last_gate_change_ns, now_ns());
}

static void fill_overlay(cl_overlay_seg_256_t *o) {
    o->cl0_ident.v.mesh_id = 0;
    o->cl0_ident.v.node_id = 0;
    o->cl0_ident.v.overlay_ipv4 = 0;
    o->cl0_ident.v.overlay_prefix = 32;
    o->cl0_ident.v.collision_offset = 0;
    o->cl0_ident.v.active = 0;

    o->cl1_trust.v.trust_state = 0;
    o->cl1_trust.v.join_epoch = 0;
    o->cl1_trust.v.grants_active = 0;
    o->cl1_trust.v.grants_revoked = 0;

    o->cl2_routing.v.routes_installed = 0;
    o->cl2_routing.v.routes_failed = 0;
    o->cl2_routing.v.fail_open = 0;
    o->cl2_routing.v.unreachable_peers = 0;

    o->cl3_state.v.gates.possible = 0;
    o->cl3_state.v.gates.effective = 0;
    o->cl3_state.v.gates_seen = 0;
    o->cl3_state.v.security_events = 0;
}

static void fill_watchdog(cl_watchdog_seg_256_t *w) {
    atomic_store(&w->cl0_liveness.v.heartbeat_seq, 1u);
    atomic_store(&w->cl0_liveness.v.last_heartbeat_tsc, 0u);
    atomic_store(&w->cl0_liveness.v.timeout_ns, 1000000000ull);
    atomic_store(&w->cl0_liveness.v.epoch, 1u);

    atomic_store(&w->cl1_faults.v.fault_flags, 0u);
    atomic_store(&w->cl1_faults.v.fault_epoch, 0u);
    atomic_store(&w->cl1_faults.v.escalation_level, 0u);
    atomic_store(&w->cl1_faults.v.escalation_count, 0u);

    atomic_store(&w->cl2_recovery.v.soft_restarts, 0u);
    atomic_store(&w->cl2_recovery.v.hard_restarts, 0u);
    atomic_store(&w->cl2_recovery.v.last_restart_tsc, 0u);
    atomic_store(&w->cl2_recovery.v.recovery_failures, 0u);

    w->cl3_state.v.gates.possible = 0;
    w->cl3_state.v.gates.effective = 0;
    w->cl3_state.v.gates_seen = 0;
    w->cl3_state.v.fatal_events = 0;
}

/* --------------------------------------------------------------------------
 * Main
 * -------------------------------------------------------------------------- */
int main(void) {
    const uint64_t boot_ns = now_ns();
    const uint64_t abi_fp = cl_compute_abi_fingerprint();

    /* Root(4096) + TOC(4096) + Segmente … */
    size_t toc_offset = CL_SHM_PAGE_BYTES;
    size_t toc_size   = CL_SHM_PAGE_BYTES;

    /* Start of segments after TOC page */
    size_t off = toc_offset + toc_size;
    off = align_up(off, 256);

    /* Segment Layout (Core-only in Dummy) */
    const uint32_t cpu_count = 1;
    const uint32_t nic_count = 1;
    const uint32_t neighbor_count = 32; /* Hot neighbors Ziel */

    /* Offsets: align to stride */
    size_t off_link      = align_up(off, 256);  off = off_link + sizeof(cl_seg_link_256_t);
    size_t off_budget    = align_up(off, 256);  off = off_budget + sizeof(cl_budget_seg_256_t);
    size_t off_mem       = align_up(off, 256);  off = off_mem + sizeof(cl_seg_mem_256_t);
    size_t off_overlay   = align_up(off, 256);  off = off_overlay + sizeof(cl_overlay_seg_256_t);
    size_t off_watchdog  = align_up(off, 256);  off = off_watchdog + sizeof(cl_watchdog_seg_256_t);

    size_t off_cpu       = align_up(off, 1024); off = off_cpu + cpu_count * sizeof(cl_seg_cpu_1024_t);
    size_t off_nic       = align_up(off, 512);  off = off_nic + nic_count * sizeof(cl_seg_nic_512_t);

    size_t off_pcie      = align_up(off, 256);  off = off_pcie + sizeof(cl_seg_pcie_256_t);
    size_t off_board     = align_up(off, 512);  off = off_board + sizeof(cl_seg_board_512_t);
    size_t off_time      = align_up(off, 256);  off = off_time + sizeof(cl_seg_time_256_t);

    size_t off_neighbor  = align_up(off, 256);  off = off_neighbor + neighbor_count * sizeof(cl_seg_mesh_neighbor_256_t);

    size_t off_zfs       = align_up(off, 4096); off = off_zfs + sizeof(cl_seg_zfs_4096_t);

    size_t core_total = align_up(off, 4096);

    /* Create SHM */
    shm_unlink(CL_CORE_SHM_NAME);
    int fd = shm_open(CL_CORE_SHM_NAME, O_CREAT | O_RDWR, 0600);
    if (fd < 0) {
        printf("FAIL: shm_open(%s): errno=%d (%s)\n", CL_CORE_SHM_NAME, errno, strerror(errno));
        return 2;
    }
    if (ftruncate(fd, (off_t)core_total) != 0) {
        printf("FAIL: ftruncate core_total=%zu: errno=%d (%s)\n", core_total, errno, strerror(errno));
        close(fd);
        return 3;
    }
    void *base = mmap(NULL, core_total, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (base == MAP_FAILED) {
        printf("FAIL: mmap core: errno=%d (%s)\n", errno, strerror(errno));
        return 4;
    }

    memset(base, 0, core_total);

    /* Root */
    cl_root_t *root = (cl_root_t*)base;
    root->abi_layout_checksum = abi_fp;

    atomic_store(&root->seq_cnt, 1u);
    atomic_store(&root->uptime_ns, 0u);

    atomic_store(&root->bulk_epoch, 0u);
    root->boot_id_ns = boot_ns;

    root->mesh_tag64 = 0;
    root->node_tag64 = 0;

    root->core_total_bytes = (uint64_t)core_total;
    root->bulk_total_bytes = 0;

    root->toc_offset = (uint64_t)toc_offset;
    root->toc_size   = (uint64_t)toc_size;

    atomic_store(&root->gates_possible, 0u);
    atomic_store(&root->gates_effective, 0u);

    root->root_magic = CL_ROOT_MAGIC_U32;
    root->schema_version = CL_SCHEMA_VERSION_U32;
    root->endian_magic = CL_ENDIAN_MAGIC_U32;

    root->root_flags = CL_ROOT_FLAG_FAILFAST_ABI | CL_ROOT_FLAG_BULK_OPTIONAL;

    atomic_store(&root->bulk_present, 0u);

    /* TOC */
    cl_toc_t *toc = (cl_toc_t*)((uint8_t*)base + toc_offset);
    toc_init(toc, (uint64_t)core_total, abi_fp);

    /* Add segments (Core) */
    (void)toc_add(toc, CL_SEG_LINK_SUMMARY, 1,
                  CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_HOT | CL_TOC_FLAG_CONTROL,
                  256, 1, (uint64_t)off_link,
                  CL_TOC_SOURCE_DIRECT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
                  CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE, 0, 0, 1);

    (void)toc_add(toc, CL_SEG_BUDGET, 1,
                  CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_HOT | CL_TOC_FLAG_CONTROL,
                  256, 1, (uint64_t)off_budget,
                  CL_TOC_SOURCE_DIRECT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
                  CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE, 0, 0, 1);

    (void)toc_add(toc, CL_SEG_MEMORY, 1,
                  CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_WARM | CL_TOC_FLAG_DATA,
                  256, 1, (uint64_t)off_mem,
                  CL_TOC_SOURCE_BEST_EFFORT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
                  CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE, 0, 0, 1);

    (void)toc_add(toc, CL_SEG_OVERLAY, 1,
                  CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_WARM | CL_TOC_FLAG_CONTROL,
                  256, 1, (uint64_t)off_overlay,
                  CL_TOC_SOURCE_BEST_EFFORT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
                  CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE, 0, 0, 1);

    (void)toc_add(toc, CL_SEG_WATCHDOG, 1,
                  CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_HOT | CL_TOC_FLAG_CONTROL | CL_TOC_FLAG_FORENSICS,
                  256, 1, (uint64_t)off_watchdog,
                  CL_TOC_SOURCE_DIRECT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
                  CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE, 0, 0, 1);

    (void)toc_add(toc, CL_SEG_CPU, 1,
                  CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_HOT | CL_TOC_FLAG_DATA | CL_TOC_FLAG_FIXED_COUNT,
                  1024, cpu_count, (uint64_t)off_cpu,
                  CL_TOC_SOURCE_DIRECT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
                  CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE, 0, 0, 1);

    (void)toc_add(toc, CL_SEG_NIC, 1,
                  CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_HOT | CL_TOC_FLAG_DATA | CL_TOC_FLAG_FIXED_COUNT,
                  512, nic_count, (uint64_t)off_nic,
                  CL_TOC_SOURCE_BEST_EFFORT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
                  CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE, 0, 0, 1);

    (void)toc_add(toc, CL_SEG_PCIe, 1,
                  CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_WARM | CL_TOC_FLAG_DATA,
                  256, 1, (uint64_t)off_pcie,
                  CL_TOC_SOURCE_BEST_EFFORT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
                  CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE, 0, 0, 1);

    (void)toc_add(toc, CL_SEG_BOARD, 1,
                  CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_WARM | CL_TOC_FLAG_DATA,
                  512, 1, (uint64_t)off_board,
                  CL_TOC_SOURCE_BEST_EFFORT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
                  CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE, 0, 0, 1);

    (void)toc_add(toc, CL_SEG_TIMEBASE, 1,
                  CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_WARM | CL_TOC_FLAG_CONTROL,
                  256, 1, (uint64_t)off_time,
                  CL_TOC_SOURCE_DIRECT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
                  CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE, 0, 0, 1);

    (void)toc_add(toc, CL_SEG_MESH_NEIGHBOR, 1,
                  CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_HOT | CL_TOC_FLAG_DATA | CL_TOC_FLAG_FIXED_COUNT,
                  256, neighbor_count, (uint64_t)off_neighbor,
                  CL_TOC_SOURCE_BEST_EFFORT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
                  CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE | CL_TOC_LAYOUT_ARRAY, 0, 0, 1);

    (void)toc_add(toc, CL_SEG_ZFS_SUMMARY, 1,
                  CL_TOC_FLAG_RESIDENT | CL_TOC_FLAG_OBSERVABLE | CL_TOC_FLAG_COLD | CL_TOC_FLAG_DATA,
                  4096, 1, (uint64_t)off_zfs,
                  CL_TOC_SOURCE_BEST_EFFORT, CL_TOC_ACCESS_READ | CL_TOC_ACCESS_WRITE | CL_TOC_ACCESS_ATOMIC,
                  CL_TOC_PRESENT, CL_TOC_LAYOUT_CACHELINE, 0, 0, 1);

    /* Fill segments minimal */
    cl_seg_link_256_t *link = (cl_seg_link_256_t*)((uint8_t*)base + off_link);
    memset(link, 0, sizeof(*link)); /* Link wird in Monitor ohnehin gelesen */
    /* Optional: könnte man deterministisch setzen, aber nicht nötig fürs ABI */

    cl_budget_seg_256_t *budget = (cl_budget_seg_256_t*)((uint8_t*)base + off_budget);
    memset(budget, 0, sizeof(*budget));
    fill_budget(budget);

    cl_seg_mem_256_t *mem = (cl_seg_mem_256_t*)((uint8_t*)base + off_mem);
    memset(mem, 0, sizeof(*mem));
    fill_mem(mem);

    cl_overlay_seg_256_t *ovl = (cl_overlay_seg_256_t*)((uint8_t*)base + off_overlay);
    memset(ovl, 0, sizeof(*ovl));
    fill_overlay(ovl);

    cl_watchdog_seg_256_t *wd = (cl_watchdog_seg_256_t*)((uint8_t*)base + off_watchdog);
    memset(wd, 0, sizeof(*wd));
    fill_watchdog(wd);

    /* CPU/NIC/PCIE/BOARD/TIME/MESH/ZFS werden hier bewusst nur genullt:
     * Dummy ist Testharness; der Aufbau zählt, nicht „Simulation“. */
    memset((uint8_t*)base + off_cpu, 0, cpu_count * sizeof(cl_seg_cpu_1024_t));
    memset((uint8_t*)base + off_nic, 0, nic_count * sizeof(cl_seg_nic_512_t));
    memset((uint8_t*)base + off_pcie, 0, sizeof(cl_seg_pcie_256_t));
    memset((uint8_t*)base + off_board, 0, sizeof(cl_seg_board_512_t));
    memset((uint8_t*)base + off_time, 0, sizeof(cl_seg_time_256_t));
    memset((uint8_t*)base + off_neighbor, 0, neighbor_count * sizeof(cl_seg_mesh_neighbor_256_t));
    memset((uint8_t*)base + off_zfs, 0, sizeof(cl_seg_zfs_4096_t));

    /* Fertig */
    msync(base, core_total, MS_SYNC);
    munmap(base, core_total);

    printf("OK: created CORE SHM %s size=%zu\n", CL_CORE_SHM_NAME, core_total);
    printf("    toc_offset=%zu toc_size=%zu entries=%u\n",
           toc_offset, toc_size, (unsigned)toc->header.entry_count);
    printf("    abi_fingerprint=0x%016" PRIx64 "\n", abi_fp);

    return 0;
}


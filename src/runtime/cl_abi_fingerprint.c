#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <stddef.h>
#include <stdalign.h>

#include "../../include/cl_runtime/cl_runtime.h"
#include "../../include/cl_runtime/cl_abi_fingerprint.h"

/* FNV-1a 64 */
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
static uint64_t fnv1a64_sz(uint64_t h, size_t v)    { return fnv1a64_u64(h, (uint64_t)v); }

uint64_t cl_abi_fingerprint_u64(void) {
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

    /* Segment sizes (critical + used) */
    h = fnv1a64_sz(h, sizeof(cl_service_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_identity_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_flow_cmd_seg_256_t));

    h = fnv1a64_sz(h, sizeof(cl_link_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_overlay_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_watchdog_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_budget_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_mem_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_time_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_pcie_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_cpu_seg_1024_t));
    h = fnv1a64_sz(h, sizeof(cl_nic_seg_512_t));
    h = fnv1a64_sz(h, sizeof(cl_board_seg_512_t));

    h = fnv1a64_sz(h, sizeof(cl_mesh_neighbor_seg_256_t));
    h = fnv1a64_sz(h, sizeof(cl_mesh_peer_seg_256_t));

    h = fnv1a64_sz(h, sizeof(cl_dma_seg_4096_t));
    h = fnv1a64_sz(h, sizeof(cl_history_seg_4096_t));
    h = fnv1a64_sz(h, sizeof(cl_forensics_seg_4096_t));
    h = fnv1a64_sz(h, sizeof(cl_zfs_seg_4096_t));

    /* Oracle suggest seg */
    h = fnv1a64_sz(h, sizeof(cl_oracle_seg_256_t));

    /* Selected alignments */
    h = fnv1a64_sz(h, (size_t)alignof(cl_root_t));
    h = fnv1a64_sz(h, (size_t)alignof(cl_service_seg_256_t));
    h = fnv1a64_sz(h, (size_t)alignof(cl_identity_seg_256_t));
    h = fnv1a64_sz(h, (size_t)alignof(cl_flow_cmd_seg_256_t));

    /* Critical offsets */
    h = fnv1a64_sz(h, offsetof(cl_root_t, abi_layout_checksum));
    h = fnv1a64_sz(h, offsetof(cl_root_t, toc_offset));
    h = fnv1a64_sz(h, offsetof(cl_root_t, toc_size));
    h = fnv1a64_sz(h, offsetof(cl_root_t, bulk_present));
    h = fnv1a64_sz(h, offsetof(cl_root_t, bulk_epoch));

    h = fnv1a64_sz(h, offsetof(cl_toc_entry_t, type));
    h = fnv1a64_sz(h, offsetof(cl_toc_entry_t, flags));
    h = fnv1a64_sz(h, offsetof(cl_toc_entry_t, stride_bytes));
    h = fnv1a64_sz(h, offsetof(cl_toc_entry_t, count));
    h = fnv1a64_sz(h, offsetof(cl_toc_entry_t, offset_bytes));
    h = fnv1a64_sz(h, offsetof(cl_toc_entry_t, presence_flags));
    h = fnv1a64_sz(h, offsetof(cl_toc_entry_t, epoch));

    h = fnv1a64_sz(h, offsetof(cl_service_seg_256_t, g0.desired_profile));
    h = fnv1a64_sz(h, offsetof(cl_service_seg_256_t, g0.desired_redirect));
    h = fnv1a64_sz(h, offsetof(cl_service_seg_256_t, g0.effective_redirect));
    h = fnv1a64_sz(h, offsetof(cl_service_seg_256_t, g0.redirect_allowed));

    return h;
}


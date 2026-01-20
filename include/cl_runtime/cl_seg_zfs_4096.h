#pragma once
/* ============================================================================
 * CommanderLink – Segment: ZFS Meta (4096B) – FINAL Endausbau
 * Datei: include/cl_runtime/cl_seg_zfs_4096.h
 * ============================================================================
 *
 * Zweck:
 *   - ZFS Presence/Health + ARC Summary + Bulk-Offsets zu Pools/VDEVs/Datasets.
 *
 * Maximalmodell:
 *   - Wenn ZFS nicht verfügbar: zfs_present=0, state=UNSUPPORTED/UNKNOWN.
 * ============================================================================
 */

#include <stdint.h>
#include <stdatomic.h>

#include "../cl_common/cl_align.h"
#include "../cl_manifest/cl_enums.h"
#include "cl_quality.h"

#define CL_ZFS_H_OK        (1u << 0)
#define CL_ZFS_H_DEGRADED  (1u << 1)
#define CL_ZFS_H_FAULTED   (1u << 2)
#define CL_ZFS_H_SCRUBBING (1u << 3)

/* Header 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint32_t) zfs_present;
        _Atomic(uint32_t) zfs_health_bits;

        _Atomic(uint16_t) zfs_state;           /* cl_feature_state_t */
        _Atomic(uint16_t) snapshots_enabled;   /* 0/1 */
        _Atomic(uint32_t) reserved0;

        _Atomic(uint32_t) pools_count;
        _Atomic(uint32_t) vdevs_count;

        _Atomic(uint32_t) datasets_count;
        _Atomic(uint32_t) zvols_count;

        uint8_t src;
        uint8_t qual;
        uint16_t reserved1;
        uint32_t reserved2;

        uint64_t reserved3;
    };
    uint8_t raw[64];
} cl_zfs_hdr_64_t;

CL_STATIC_ASSERT(sizeof(cl_zfs_hdr_64_t)==64, "zfs hdr 64");

/* ARC 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) arc_size;
        _Atomic(uint64_t) arc_max;
        _Atomic(uint64_t) arc_hits;
        _Atomic(uint64_t) arc_misses;
        _Atomic(uint64_t) l2_size;
        _Atomic(uint64_t) l2_hits;
        _Atomic(uint64_t) l2_misses;
        _Atomic(uint64_t) reserved0;
    };
    uint8_t raw[64];
} cl_zfs_arc_64_t;

CL_STATIC_ASSERT(sizeof(cl_zfs_arc_64_t)==64, "zfs arc 64");

/* Bulk offsets 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) off_pools;
        _Atomic(uint64_t) off_vdevs;
        _Atomic(uint64_t) off_datasets;
        _Atomic(uint64_t) off_zvols;

        _Atomic(uint64_t) size_pools_bytes;
        _Atomic(uint64_t) size_vdevs_bytes;
        _Atomic(uint64_t) size_datasets_bytes;
        _Atomic(uint64_t) size_zvols_bytes;
    };
    uint8_t raw[64];
} cl_zfs_bulk_64_t;

CL_STATIC_ASSERT(sizeof(cl_zfs_bulk_64_t)==64, "zfs bulk 64");

typedef union CL_ALIGNAS(8) { struct { uint64_t r[8]; }; uint8_t raw[64]; } cl_zfs_rsv_64_t;

typedef struct CL_ALIGNAS(4096) {
    cl_zfs_hdr_64_t  hdr;
    cl_zfs_arc_64_t  arc;
    cl_zfs_bulk_64_t bulk;
    cl_zfs_rsv_64_t  rsv[61];
} cl_zfs_seg_4096_t;

CL_STATIC_ASSERT(sizeof(cl_zfs_seg_4096_t)==4096, "zfs seg 4096");


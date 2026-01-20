#pragma once
/* ============================================================================
 * CommanderLink – Segment: Overlay (256B) – FINAL Endausbau
 * Datei: include/cl_runtime/cl_seg_overlay_256.h
 * ============================================================================
 *
 * Maximalmodell:
 * - Overlay ist Truth für Monitor: IP, ifname, prefix, state, routes, collision.
 * ============================================================================
 */

#include <stdint.h>
#include <stdatomic.h>

#include "../cl_common/cl_align.h"
#include "../cl_manifest/cl_enums.h"
#include "cl_quality.h"

/* HOT 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint32_t) overlay_state;     /* cl_feature_state_t */
        _Atomic(uint32_t) route_state;       /* cl_feature_state_t (global) */

        _Atomic(uint32_t) overlay_ip_host_u32;
        _Atomic(uint32_t) prefixlen;

        _Atomic(uint32_t) mtu_effective;
        _Atomic(uint32_t) active_routes;

        _Atomic(uint32_t) collision_offset;
        _Atomic(uint32_t) failopen_events;

        _Atomic(uint64_t) last_update_ns;
    };
    uint8_t raw[64];
} cl_overlay_hot_64_t;

CL_STATIC_ASSERT(sizeof(cl_overlay_hot_64_t) == 64, "overlay hot 64");

/* WARM 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint32_t) routes_added_total;
        _Atomic(uint32_t) routes_removed_total;
        _Atomic(uint32_t) orphan_routes_total;
        _Atomic(uint32_t) collision_events_total;

        _Atomic(uint64_t) reserved0;
        _Atomic(uint64_t) reserved1;
        _Atomic(uint64_t) reserved2;
        _Atomic(uint64_t) reserved3;
    };
    uint8_t raw[64];
} cl_overlay_warm_64_t;

CL_STATIC_ASSERT(sizeof(cl_overlay_warm_64_t) == 64, "overlay warm 64");

/* COLD 64 (if name) */
typedef union CL_ALIGNAS(8) {
    struct {
        char if_name[16];

        uint8_t src;
        uint8_t qual;
        uint16_t reserved0;
        uint32_t reserved1;

        uint64_t reserved2;
        uint64_t reserved3;
        uint64_t reserved4;
        uint64_t reserved5;
    };
    uint8_t raw[64];
} cl_overlay_cold_64_t;

CL_STATIC_ASSERT(sizeof(cl_overlay_cold_64_t) == 64, "overlay cold 64");

typedef union CL_ALIGNAS(8) { struct { uint64_t r[8]; }; uint8_t raw[64]; } cl_overlay_rsv_64_t;

typedef struct CL_ALIGNAS(256) {
    cl_overlay_hot_64_t  hot;
    cl_overlay_warm_64_t warm;
    cl_overlay_cold_64_t cold;
    cl_overlay_rsv_64_t  rsv;
} cl_overlay_seg_256_t;

CL_STATIC_ASSERT(sizeof(cl_overlay_seg_256_t) == 256, "overlay seg 256");


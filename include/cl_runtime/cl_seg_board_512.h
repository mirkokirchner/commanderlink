#pragma once
/* ============================================================================
 * CommanderLink – Segment: Board/VRM/Power (512B) – FINAL Endausbau
 * Datei: include/cl_runtime/cl_seg_board_512.h
 * ============================================================================
 *
 * Maximalmodell:
 * - Board/VRM/Power sind Safety Cutoff Inputs, nicht Hot-Pacing Inputs.
 * - Alle Werte haben state/source/quality.
 * - Hersteller/Name/Serial ist in COLD enthalten (Monitor-Anforderung).
 * ============================================================================
 */

#include <stdint.h>
#include <stdatomic.h>

#include "../cl_common/cl_align.h"
#include "../cl_manifest/cl_enums.h"
#include "cl_quality.h"

/* HOT A 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint32_t) board_therm_state; /* cl_thermal_state_t */
        _Atomic(uint32_t) board_cap_flags;   /* constraint bits (VRM/POWER/THERM) */

        _Atomic(uint32_t) vrm_temp_c_x10;
        _Atomic(uint32_t) pch_temp_c_x10;

        _Atomic(uint32_t) pwr_package_mw;
        _Atomic(uint32_t) pwr_limit_mw;

        _Atomic(uint32_t) vrm_ripple_mv_x10;
        _Atomic(uint32_t) power_unstable;    /* 0/1 */

        _Atomic(uint64_t) last_update_ns;
    };
    uint8_t raw[64];
} cl_board_hot_64a_t;

CL_STATIC_ASSERT(sizeof(cl_board_hot_64a_t) == 64, "board hot a 64");

/* HOT B 64 (smoothed outputs + events) */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint32_t) vrm_temp_avg_c_x10;
        _Atomic(uint32_t) pwr_pkg_avg_mw;
        _Atomic(uint32_t) ripple_avg_mv_x10;
        _Atomic(uint32_t) vrm_trend_permille;

        _Atomic(uint64_t) cap_events_total;
        _Atomic(uint64_t) power_events_total;
        _Atomic(uint64_t) thermal_events_total;

        uint64_t reserved0;
    };
    uint8_t raw[64];
} cl_board_hot_64b_t;

CL_STATIC_ASSERT(sizeof(cl_board_hot_64b_t) == 64, "board hot b 64");

/* WARM A 64 (fans/rails) */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint32_t) fan_rpm_0;
        _Atomic(uint32_t) fan_rpm_1;
        _Atomic(uint32_t) fan_rpm_2;
        _Atomic(uint32_t) fan_rpm_3;

        _Atomic(uint32_t) v12_mv;
        _Atomic(uint32_t) v5_mv;

        _Atomic(uint32_t) vrm_supported;
        _Atomic(uint32_t) ripple_supported;

        uint64_t reserved0;
    };
    uint8_t raw[64];
} cl_board_warm_64a_t;

CL_STATIC_ASSERT(sizeof(cl_board_warm_64a_t) == 64, "board warm a 64");

/* WARM B 64 (feature states) */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint16_t) vrm_state;     /* cl_feature_state_t */
        _Atomic(uint16_t) ripple_state;  /* cl_feature_state_t */
        _Atomic(uint16_t) pwr_state;     /* cl_feature_state_t */
        _Atomic(uint16_t) therm_state;   /* cl_feature_state_t */

        _Atomic(uint32_t) reserved0;
        _Atomic(uint64_t) reserved1;
        _Atomic(uint64_t) reserved2;
        _Atomic(uint64_t) reserved3;
        _Atomic(uint64_t) reserved4;
    };
    uint8_t raw[64];
} cl_board_warm_64b_t;

CL_STATIC_ASSERT(sizeof(cl_board_warm_64b_t) == 64, "board warm b 64");

/* COLD 64 (Inventory strings: vendor/model/serial) */
typedef union CL_ALIGNAS(8) {
    struct {
        char vendor[16];
        char model[16];
        char serial[16];

        uint8_t src;
        uint8_t qual;
        uint16_t reserved0;

        uint32_t reserved1;
        uint64_t reserved2;
    };
    uint8_t raw[64];
} cl_board_cold_64_t;

CL_STATIC_ASSERT(sizeof(cl_board_cold_64_t) == 64, "board cold 64");

/* RSV lines */
typedef union CL_ALIGNAS(8) { struct { uint64_t r[8]; }; uint8_t raw[64]; } cl_board_rsv_64_t;

typedef struct CL_ALIGNAS(512) {
    cl_board_hot_64a_t  hot_a;
    cl_board_hot_64b_t  hot_b;
    cl_board_warm_64a_t warm_a;
    cl_board_warm_64b_t warm_b;
    cl_board_cold_64_t  cold;
    cl_board_rsv_64_t   rsv0;
    cl_board_rsv_64_t   rsv1;
    cl_board_rsv_64_t   rsv2;
} cl_board_seg_512_t;

CL_STATIC_ASSERT(sizeof(cl_board_seg_512_t) == 512, "board seg 512");


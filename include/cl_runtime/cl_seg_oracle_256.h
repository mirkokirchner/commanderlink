#pragma once
/* ============================================================================
 * CommanderLink – Segment: Oracle Suggest (256B) – FINAL (Oracle Mode)
 * Datei: include/cl_runtime/cl_seg_oracle_256.h
 * ============================================================================
 *
 * SSOT:
 * - ORACLE ist optional (Mode).
 * - ORACLE schreibt keine Wahrheit, keine Gates, keine Redirects.
 * - ORACLE liefert ausschließlich Hints/Clamps + Top-5 Reasons (P4/P5).
 * - Abwesenheit: oracle_epoch==0 => OFF.
 *
 * Writer: ORACLE
 * Readers: Monitor (immer), CORE/FLOW (nur Hint an Cut-Points)
 * ============================================================================
 */

#include <stdint.h>
#include <stdatomic.h>

#include "../cl_common/cl_align.h"
#include "../cl_manifest/cl_enums.h"
#include "cl_quality.h"

/* HOT 64: state + freshness + confidence */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint32_t) oracle_epoch;      /* 0 => OFF */
        _Atomic(uint32_t) oracle_age_ms;     /* Anzeige: time since last update */

        _Atomic(uint8_t)  oracle_quality;   /* cl_quality_t */
        _Atomic(uint8_t)  oracle_confidence;/* 0..100 */
        uint8_t reserved0[2];

        _Atomic(uint32_t) suggest_mask;     /* bitmask: thermal/loss/mtu/budget... */

        /* bounded ceilings (never authority) */
        _Atomic(uint32_t) max_streams;      /* ceiling */
        _Atomic(uint32_t) max_lz4_rung;     /* ceiling */
        _Atomic(uint32_t) max_fec_rung;     /* ceiling */
        _Atomic(uint32_t) max_send_rate_frac_permille; /* 0..1000 */

        _Atomic(uint32_t) defer_mtu_probe;  /* 0/1 */
        _Atomic(uint32_t) reserved1;
    };
    uint8_t raw[64];
} cl_oracle_hot_64_t;

CL_STATIC_ASSERT(sizeof(cl_oracle_hot_64_t) == 64, "oracle hot 64");

/* WARM 128: Top-5 reasons + quantized scores */
typedef union CL_ALIGNAS(8) {
    struct {
        /* Top-5 reasons (P4/P5 only), strongest first */
        _Atomic(uint32_t) reason_code[5];
        _Atomic(uint16_t) reason_strength[5]; /* 0..65535 */
        uint16_t reserved0;

        /* Quantized scores for display (optional) */
        _Atomic(int16_t) thermal_slope_q8;       /* e.g. C/min * 256 */
        _Atomic(uint16_t) loss_burst_score;      /* 0..65535 */
        _Atomic(uint16_t) peer_jitter_score;     /* 0..65535 */
        _Atomic(uint16_t) budget_pressure_score; /* 0..65535 */

        _Atomic(uint16_t) reserved1;
        _Atomic(uint32_t) reserved2;
        _Atomic(uint64_t) reserved3;
        _Atomic(uint64_t) reserved4;
        _Atomic(uint64_t) reserved5;
        _Atomic(uint64_t) reserved6;
        _Atomic(uint64_t) reserved7;
    };
    uint8_t raw[128];
} cl_oracle_warm_128_t;

CL_STATIC_ASSERT(sizeof(cl_oracle_warm_128_t) == 128, "oracle warm 128");

/* COLD 64: caps (immutable after genesis) */
typedef union CL_ALIGNAS(8) {
    struct {
        uint32_t oracle_tick_ms;     /* suggestion update interval */
        uint32_t reserved0;

        uint32_t backend_allowed_mask_u32;
        uint32_t comp_allowed_mask_u32;
        uint32_t crc_allowed_mask_u32;
        uint32_t fec_allowed_mask_u32;

        uint64_t reserved1;
        uint64_t reserved2;
        uint64_t reserved3;
    };
    uint8_t raw[64];
} cl_oracle_cold_64_t;

CL_STATIC_ASSERT(sizeof(cl_oracle_cold_64_t) == 64, "oracle cold 64");

/* SEG 256 */
typedef union CL_ALIGNAS(64) {
    struct {
        cl_oracle_hot_64_t  hot;
        cl_oracle_warm_128_t warm;
        cl_oracle_cold_64_t cold;
    };
    uint8_t raw[256];
} cl_oracle_seg_256_t;

CL_STATIC_ASSERT(sizeof(cl_oracle_seg_256_t) == 256, "oracle seg 256");


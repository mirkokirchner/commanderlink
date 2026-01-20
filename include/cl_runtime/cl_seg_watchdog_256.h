#pragma once
/*
 * ============================================================================
 * CommanderLink – WATCHDOG Segment (256B)
 * Datei: include/cl_runtime/cl_seg_watchdog_256.h
 * ============================================================================
 *
 * ZWECK
 *   - Selbstheilung: erkennt Stall/Panic und löst CL-Restart aus.
 *   - OS bleibt stabil (Fail-open): non-persistent Overlay, keine Default-Route.
 *
 * PHYSIK / ABI
 *   - Segment ist exakt 256B.
 *   - 4 Cacheline-Normflächen (64B) -> HOT_A, HOT_B, WARM, COLD.
 *   - Keine packed-Strukturen im SHM.
 *
 * OWNERSHIP
 *   - Writer: CORE aggregiert Zustände; Module schreiben nur ihre Tickmarker.
 *   - Reader: Monitor2 / Oracle.
 * ============================================================================
 */

#include <stdint.h>
#include <stdatomic.h>

#include "../cl_common/cl_align.h"
#include "cl_quality.h"

/* -------------------------------------------------------------------------- */
/* Module IDs (für Stall/Restart Reason) – append-only                         */
/* -------------------------------------------------------------------------- */
typedef enum {
    CL_WD_MOD_NONE   = 0,
    CL_WD_MOD_HAL    = 1,
    CL_WD_MOD_CORE   = 2,
    CL_WD_MOD_FLOW   = 3,
    CL_WD_MOD_LINK   = 4,
    CL_WD_MOD_ORACLE = 5
} cl_wd_module_t;

/* -------------------------------------------------------------------------- */
/* Watchdog State                                                             */
/* -------------------------------------------------------------------------- */
typedef enum {
    CL_WD_OK    = 0,
    CL_WD_WARN  = 1,
    CL_WD_STALL = 2,
    CL_WD_PANIC = 3
} cl_wd_state_t;

/* -------------------------------------------------------------------------- */
/* Watchdog Reason Codes (append-only)                                        */
/* -------------------------------------------------------------------------- */
typedef enum {
    CL_WD_R_NONE = 0,

    /* Zeitbasierter Stall */
    CL_WD_R_STALL_HAL,
    CL_WD_R_STALL_CORE,
    CL_WD_R_STALL_FLOW,
    CL_WD_R_STALL_LINK,
    CL_WD_R_STALL_ORACLE,

    /* Integritäts-/Konsistenzverletzung (harte Gründe) */
    CL_WD_R_EPOCH_REGRESSION,
    CL_WD_R_GRANT_REPLAY,
    CL_WD_R_CREDITS_NEGATIVE,
    CL_WD_R_BACKEND_PANIC,
    CL_WD_R_INVARIANT_BREACH
} cl_wd_reason_t;

/* ============================================================================
 * HOT_A 64B: Liveness Marker (letztes Lebenszeichen je Modul)
 * ============================================================================
 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) hal_tick_ns;
        _Atomic(uint64_t) core_tick_ns;
        _Atomic(uint64_t) flow_tick_ns;
        _Atomic(uint64_t) link_tick_ns;
        _Atomic(uint64_t) oracle_tick_ns;

        uint8_t  src;
        uint8_t  qual;
        uint16_t reserved0;
        uint32_t reserved1;
    };
    uint8_t raw[64];
} cl_wd_hot_64a_t;

CL_STATIC_ASSERT(sizeof(cl_wd_hot_64a_t) == 64, "watchdog hot_a 64");

/* ============================================================================
 * HOT_B 64B: Progress Marker (Fortschritt, nicht nur Liveness)
 * ============================================================================
 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) last_progress_ns;      /* monotonic ns */
        _Atomic(uint64_t) last_commit_epoch;     /* Link commit epoch */
        _Atomic(uint64_t) last_root_seq_cnt;     /* Root seq snapshot */

        _Atomic(uint32_t) wd_state;              /* cl_wd_state_t */
        _Atomic(uint32_t) wd_reason;             /* cl_wd_reason_t */

        _Atomic(uint32_t) restart_count_total;   /* seit Boot */
        _Atomic(uint32_t) last_stall_module;     /* cl_wd_module_t */

        uint8_t  src;
        uint8_t  qual;
        uint16_t reserved0;
        uint32_t reserved1;
    };
    uint8_t raw[64];
} cl_wd_hot_64b_t;

CL_STATIC_ASSERT(sizeof(cl_wd_hot_64b_t) == 64, "watchdog hot_b 64");

/* ============================================================================
 * WARM 64B: Restart Summary (persistente Liste kommt über Log + Forensics)
 * ============================================================================
 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) last_restart_ns;
        _Atomic(uint32_t) last_restart_reason;     /* cl_wd_reason_t */
        _Atomic(uint32_t) last_restart_module;     /* cl_wd_module_t */

        _Atomic(uint64_t) last_restart_commit_epoch;

        _Atomic(uint64_t) reserved0;
        _Atomic(uint64_t) reserved1;

        uint8_t  src;
        uint8_t  qual;
        uint16_t reserved2;
        uint32_t reserved3;
    };
    uint8_t raw[64];
} cl_wd_warm_64_t;

CL_STATIC_ASSERT(sizeof(cl_wd_warm_64_t) == 64, "watchdog warm 64");

/* ============================================================================
 * COLD 64B: Policy/Thresholds (sichtbar, konservativ)
 * ============================================================================
 */
typedef union CL_ALIGNAS(8) {
    struct {
        uint32_t stall_ns_threshold;   /* z.B. 2_000_000_000 (2s) */
        uint32_t warn_ns_threshold;    /* z.B. 250_000_000 (250ms) */

        uint32_t max_restart_per_min;  /* Rate limit */
        uint32_t cooldown_s;           /* Sperrzeit nach Restart */

        uint32_t log_enabled;          /* 1=watchdog log schreiben */
        uint32_t reserved0;

        uint8_t  src;
        uint8_t  qual;
        uint16_t reserved1;
        uint32_t reserved2;

        uint64_t reserved3;
    };
    uint8_t raw[64];
} cl_wd_cold_64_t;

CL_STATIC_ASSERT(sizeof(cl_wd_cold_64_t) == 64, "watchdog cold 64");

/* ============================================================================
 * Segment 256B
 * ============================================================================
 */
typedef struct CL_ALIGNAS(256) {
    cl_wd_hot_64a_t hot_a;
    cl_wd_hot_64b_t hot_b;
    cl_wd_warm_64_t warm;
    cl_wd_cold_64_t cold;
} cl_watchdog_seg_256_t;

CL_STATIC_ASSERT(sizeof(cl_watchdog_seg_256_t) == 256, "watchdog seg 256");


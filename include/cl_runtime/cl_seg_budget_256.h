#pragma once
/* ============================================================================
 * CommanderLink – Segment: Budget (256B) – FINAL Endausbau
 * Datei: include/cl_runtime/cl_seg_budget_256.h
 * ============================================================================
 *
 * Maximalmodell:
 * - zeigt verfügbares RAM, staging caps, pressure, reclaim events
 * - zeigt, ob CL gerade aggressiv Ressourcen nutzt oder zurückzieht
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
        _Atomic(uint32_t) budget_state;          /* cl_budget_state_t */
        _Atomic(uint32_t) interactive_hint;      /* 0/1 */

        _Atomic(uint32_t) cpu_pressure_permille; /* 0..1000 */
        _Atomic(uint32_t) mem_pressure_permille; /* 0..1000 */

        _Atomic(uint64_t) total_ram_bytes;       /* best-effort */
        _Atomic(uint64_t) free_ram_bytes;        /* best-effort */

        _Atomic(uint64_t) bulk_bytes_current;
        _Atomic(uint64_t) bulk_bytes_cap;
    };
    uint8_t raw[64];
} cl_budget_hot_64_t;

CL_STATIC_ASSERT(sizeof(cl_budget_hot_64_t) == 64, "budget hot 64");

/* WARM 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) reclaim_events;
        _Atomic(uint64_t) shadow_events;
        _Atomic(uint64_t) user_activity_events;

        _Atomic(uint64_t) mem_reclaim_ns;

        _Atomic(uint32_t) io_pressure_permille;  /* best-effort */
        _Atomic(uint32_t) storage_pressure_permille; /* disk sink/backlog */

        _Atomic(uint64_t) last_pressure_ts_ns;
        _Atomic(uint64_t) reserved0;
    };
    uint8_t raw[64];
} cl_budget_warm_64_t;

CL_STATIC_ASSERT(sizeof(cl_budget_warm_64_t) == 64, "budget warm 64");

/* COLD 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        uint8_t src;
        uint8_t qual;
        uint16_t reserved0;

        uint32_t reserved1;
        uint64_t reserved2;
        uint64_t reserved3;
        uint64_t reserved4;
        uint64_t reserved5;
        uint64_t reserved6;
    };
    uint8_t raw[64];
} cl_budget_cold_64_t;

CL_STATIC_ASSERT(sizeof(cl_budget_cold_64_t) == 64, "budget cold 64");

/* RSV 64 (append-only space) */
typedef union CL_ALIGNAS(8) { struct { uint64_t r[8]; }; uint8_t raw[64]; } cl_budget_rsv_64_t;

typedef struct CL_ALIGNAS(256) {
    cl_budget_hot_64_t  hot;
    cl_budget_warm_64_t warm;
    cl_budget_cold_64_t cold;
    cl_budget_rsv_64_t  rsv;
} cl_budget_seg_256_t;

CL_STATIC_ASSERT(sizeof(cl_budget_seg_256_t) == 256, "budget seg 256");


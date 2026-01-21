#pragma once
/* ============================================================================
 * CommanderLink – Segment: Service-State (256B) – FINAL
 * Datei: include/cl_runtime/cl_service_seg_256.h
 * ============================================================================
 *
 * Zweck:
 *   SSOT für Boot-/Supervisor-Zustand.
 *   ZERO-konform: keine Telemetrie, keine Policy – nur Prozessstatus.
 *
 * Writer:
 *   - clboot / später cld: expected/running/pid/crash/exit/signal
 *   - jeder Prozess (core0/hal0/link0/flow0/oracle0): heartbeat_ns
 *
 * Layout:
 *   256B = 4 Cachelines à 64B
 * ============================================================================
 */

#include <stdint.h>
#include <stdatomic.h>

#include "../cl_common/cl_align.h"

/* Service IDs (Slot-Index; SSOT) */
typedef enum cl_service_id {
    CL_SVC_CORE0   = 0,
    CL_SVC_HAL0    = 1,
    CL_SVC_LINK0   = 2,
    CL_SVC_FLOW0   = 3,
    CL_SVC_ORACLE0 = 4,
    CL_SVC_MONITOR = 5, /* optional */
    CL_SVC_MAX     = 6
} cl_service_id_t;

/* 32B Slot: bewusst klein, deterministisch */
typedef struct CL_ALIGNAS(8) cl_service_slot_32 {
    _Atomic(uint32_t) expected;          /* 0/1 */
    _Atomic(uint32_t) running;           /* 0/1 */
    _Atomic(uint32_t) pid;               /* Prozess-ID */
    _Atomic(uint32_t) crash_count;       /* Restart-Zähler */

    _Atomic(uint64_t) last_heartbeat_ns; /* Prozess schreibt (MONOTONIC ns) */

    _Atomic(int32_t)  last_exit_code;    /* Supervisor schreibt */
    _Atomic(int32_t)  last_signal;       /* Supervisor schreibt */
} cl_service_slot_32_t;

CL_STATIC_ASSERT(sizeof(cl_service_slot_32_t) == 32, "service slot muss 32B sein");

/* CL0: Globaler Boot/Redirect-Status */
typedef union CL_ALIGNAS(8) {
struct {
    _Atomic(uint64_t) boot_epoch;
    _Atomic(uint64_t) boot_id_ns;

    _Atomic(uint32_t) desired_profile;      /* Default: ACCELERATE */
    _Atomic(uint32_t) desired_redirect;     /* Wunsch: 0/1 (nur REDIRECT scope) */
    _Atomic(uint32_t) effective_redirect;   /* LINK setzt effektiv */

    _Atomic(uint32_t) redirect_allowed;   /* CORE truth: 0/1 */
    _Atomic(uint32_t) reserved_redirect0;

    _Atomic(uint32_t) last_error_code;
    _Atomic(uint32_t) service_count;

    _Atomic(uint64_t) reserved0;            /* future */
};

    uint8_t raw[64];
} cl_service_global_64_t;

CL_STATIC_ASSERT(sizeof(cl_service_global_64_t) == 64, "service global muss 64B sein");

/* CL1: Slots 0–1 */
typedef union CL_ALIGNAS(8) {
    struct { cl_service_slot_32_t s0; cl_service_slot_32_t s1; };
    uint8_t raw[64];
} cl_service_slots01_64_t;

CL_STATIC_ASSERT(sizeof(cl_service_slots01_64_t) == 64, "service slots01 muss 64B sein");

/* CL2: Slots 2–3 */
typedef union CL_ALIGNAS(8) {
    struct { cl_service_slot_32_t s2; cl_service_slot_32_t s3; };
    uint8_t raw[64];
} cl_service_slots23_64_t;

CL_STATIC_ASSERT(sizeof(cl_service_slots23_64_t) == 64, "service slots23 muss 64B sein");

/* CL3: Slots 4–5 */
typedef union CL_ALIGNAS(8) {
    struct { cl_service_slot_32_t s4; cl_service_slot_32_t s5; };
    uint8_t raw[64];
} cl_service_slots45_64_t;

CL_STATIC_ASSERT(sizeof(cl_service_slots45_64_t) == 64, "service slots45 muss 64B sein");

/* Segment 256B */
typedef struct CL_ALIGNAS(256) cl_service_seg_256 {
    cl_service_global_64_t  g0;
    cl_service_slots01_64_t g1;
    cl_service_slots23_64_t g2;
    cl_service_slots45_64_t g3;
} cl_service_seg_256_t;

CL_STATIC_ASSERT(sizeof(cl_service_seg_256_t) == 256, "service seg muss 256B sein");
/* SSOT: service id -> slot pointer (single source of truth)
 * Returns NULL for unknown IDs. Eliminates slot drift across components.
 */
static inline cl_service_slot_32_t*
cl_service_slot(cl_service_seg_256_t *svc, cl_service_id_t id)
{
    if (!svc) return NULL;
    switch (id) {
        case CL_SVC_CORE0:   return &svc->g1.s0;
        case CL_SVC_HAL0:    return &svc->g1.s1;
        case CL_SVC_LINK0:   return &svc->g2.s2;
        case CL_SVC_FLOW0:   return &svc->g2.s3;
        case CL_SVC_ORACLE0: return &svc->g3.s4;
        case CL_SVC_MONITOR: return &svc->g3.s5;
        default: return NULL;
    }
}

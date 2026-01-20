#pragma once
/* ============================================================================
 * CommanderLink – Segment: Forensics (4096B) – FINAL Endausbau
 * Datei: include/cl_runtime/cl_seg_forensics_4096.h
 * ============================================================================
 *
 * Zweck:
 *   - Ringbuffer seltener, hochrelevanter Events.
 *   - Maximalmodell: commit_epoch + grant_id + reason/constraints immer dabei.
 * ============================================================================
 */

#include <stdint.h>
#include <stdatomic.h>

#include "../cl_common/cl_align.h"
#include "../cl_manifest/cl_enums.h"
#include "cl_quality.h"

/* Forensics Event IDs (append-only) */
typedef enum {
    CL_FX_NONE = 0,

    /* Discovery / Join */
    CL_FX_BEACON_SEEN = 100,
    CL_FX_HELLO_SENT = 101,
    CL_FX_HELLO_ACK_RX = 102,
    CL_FX_AUTH_FAIL = 110,
    CL_FX_REPLAY_DETECTED = 111,
    CL_FX_VERSION_MISMATCH = 112,
    CL_FX_MESH_MISMATCH = 113,
    CL_FX_IP_COLLISION = 114,
    CL_FX_TRUSTED_ACTIVE = 120,
    CL_FX_DENIED = 121,

    /* Grants / Credits */
    CL_FX_GRANT_RX = 200,
    CL_FX_GRANT_EXPIRED = 201,
    CL_FX_CREDIT_STARVATION = 202,

    /* Autopath */
    CL_FX_BACKEND_UPGRADE = 300,
    CL_FX_BACKEND_DOWNGRADE = 301,
    CL_FX_TCP_FALLBACK = 302
} cl_forensics_event_id_t;

/* Event 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) ts_ns;
        _Atomic(uint64_t) commit_epoch;
        _Atomic(uint64_t) grant_id;
        _Atomic(uint64_t) param1;

        _Atomic(uint32_t) event_id;
        _Atomic(uint32_t) reason_code;

        _Atomic(uint32_t) constraint_flags;
        _Atomic(uint32_t) aux;

        _Atomic(uint64_t) param2;
        _Atomic(uint64_t) param3;
    };
    uint8_t raw[64];
} cl_forensics_event_64_t;

CL_STATIC_ASSERT(sizeof(cl_forensics_event_64_t)==64, "forensics event 64");

/* Header 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint32_t) head;
        _Atomic(uint32_t) tail;
        _Atomic(uint32_t) mask;      /* 63 */
        _Atomic(uint32_t) dropped;

        _Atomic(uint64_t) last_event_ns;

        uint8_t src;
        uint8_t qual;
        uint16_t reserved0;
        uint32_t reserved1;

        uint64_t reserved2;
        uint64_t reserved3;
        uint64_t reserved4;
    };
    uint8_t raw[64];
} cl_forensics_hdr_64_t;

CL_STATIC_ASSERT(sizeof(cl_forensics_hdr_64_t)==64, "forensics hdr 64");

typedef struct CL_ALIGNAS(4096) {
    cl_forensics_hdr_64_t hdr;
    cl_forensics_event_64_t ev[63];
} cl_forensics_seg_4096_t;

CL_STATIC_ASSERT(sizeof(cl_forensics_seg_4096_t)==4096, "forensics seg 4096");


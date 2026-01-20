#pragma once
/* ============================================================================
 * CommanderLink – Segment: FLOW Command/Intent (256B) – FINAL Endausbau
 * Datei: include/cl_runtime/cl_seg_flow_cmd_256.h
 *
 * Zweck:
 *   SSOT-Kanal FLOW → LINK. Definiert Autopacing/Credits-Policy (nicht Reality).
 *
 * Writer: FLOW
 * Reader: LINK (Apply-Cut-Point), Monitor (read-only)
 *
 * Commit:
 *   - FLOW schreibt Felder, dann cmd_version++ (Release).
 *   - LINK liest stable cmd_version (Acquire).
 * ============================================================================
 */

#include <stdint.h>
#include <stdatomic.h>

#include "../cl_common/cl_align.h"
#include "../cl_manifest/cl_enums.h"
#include "cl_quality.h"

typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint32_t) cmd_version;           /* commit marker */
        _Atomic(uint32_t) cmd_reason_mask;       /* why clamp/scale (bitmask, extend-only) */

        _Atomic(uint64_t) send_rate_target_bps;  /* policy */
        _Atomic(uint64_t) pacing_quantum_ns;     /* policy */

        _Atomic(uint64_t) max_inflight_bytes;    /* POLICY credits cap */
        _Atomic(uint64_t) burst_cap_bytes;       /* bounded burst */

        _Atomic(uint32_t) stream_count;          /* multistream */
        _Atomic(uint32_t) stream_policy;         /* enum */

        _Atomic(uint32_t) backend_intent;        /* cl_backend_active_t (prefer) */
        _Atomic(uint32_t) mtu_intent;            /* 1500/4096/9000 */

        _Atomic(uint32_t) compression_rung;      /* LZ4 ladder */
        _Atomic(uint32_t) integrity_rung;        /* CRC/FEC ladder */
    };
    uint8_t raw[64];
} cl_flow_cmd_hot_64_t;

CL_STATIC_ASSERT(sizeof(cl_flow_cmd_hot_64_t) == 64, "flow cmd hot 64");

typedef union CL_ALIGNAS(8) { struct { uint64_t r[8]; }; uint8_t raw[64]; } cl_flow_cmd_rsv_64_t;

typedef struct CL_ALIGNAS(256) {
    cl_flow_cmd_hot_64_t hot;
    cl_flow_cmd_rsv_64_t rsv1;
    cl_flow_cmd_rsv_64_t rsv2;
    cl_flow_cmd_rsv_64_t rsv3;
} cl_flow_cmd_seg_256_t;

CL_STATIC_ASSERT(sizeof(cl_flow_cmd_seg_256_t) == 256, "flow cmd seg 256");


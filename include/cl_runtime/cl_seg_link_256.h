#pragma once
/* ============================================================================
 * CommanderLink – Segment: Link Summary (256B) – FINAL Endausbau
 * Datei: include/cl_runtime/cl_seg_link_256.h
 * ============================================================================
 *
 * Record = 4 Cachelines à 64B:
 *   HOT   : effektiver Pfad/Backend/Mode/Profile + pacing/tick/mtu/batch + reasons
 *   WARM A: commit epochs + throughput totals + starvation + partials
 *   WARM B: Zero-Transform Geometrie + Accept + FEC/CRC/PHV + Compression + States
 *   COLD  : desired targets (policy) + src/qual
 *
 * Maximalmodell:
 *   - possible/effective/state für Integrity (CRC/FEC) und Zero-Transform
 *   - keine impliziten Defaults
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
        /* 8× u32 = 32B */
        _Atomic(uint32_t) path_state;
        _Atomic(uint32_t) backend_active;
        _Atomic(uint32_t) backend_detail;
        _Atomic(uint32_t) profile_effective;

        _Atomic(uint32_t) mode_active;
        _Atomic(uint32_t) constraint_flags;
        _Atomic(uint32_t) reason_code;
        _Atomic(uint32_t) global_health;

        /* pacing + pressure + audit (alles u32, keine u64 hier) */
        _Atomic(uint32_t) pacing_actual_bps;
        _Atomic(uint32_t) pacing_target_bps;

        _Atomic(uint32_t) rule_set_version;          /* monotonic */
        _Atomic(uint32_t) last_redirect_reason;      /* cl_reason_code_t (P0–P3) */

        _Atomic(uint32_t) tx_queue_pressure_permille;/* 0..1000 */
        _Atomic(uint32_t) batch_eff_permille;        /* 0..1000 */

        _Atomic(uint16_t) mtu_effective;             /* negotiated/effective */
        _Atomic(uint16_t) batch_target;              /* batching target */

        _Atomic(uint32_t) reserved0;                 /* pad to 64B */
    };
    uint8_t raw[64];
} cl_link_hot_64_t;

CL_STATIC_ASSERT(sizeof(cl_link_hot_64_t) == 64, "link hot 64");

/* WARM A 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        /* APPLY/AUDIT */
        _Atomic(uint64_t) apply_epoch;         /* u64 monotonic; LINK apply cut-point */
        _Atomic(uint64_t) tx_inflight_bytes;   /* real inflight (credits reality) */

        /* keep a small, bounded set of counters (64B total) */
        _Atomic(uint64_t) last_commit_epoch;   /* existing semantic */
        _Atomic(uint64_t) credit_starvation_ns;

        _Atomic(uint64_t) bytes_sent_total;
        _Atomic(uint64_t) pkts_sent_total;

        _Atomic(uint64_t) inject_partial_cnt;
        _Atomic(uint64_t) prepare_expired_cnt;
    };
    uint8_t raw[64];
} cl_link_warm_64a_t;

CL_STATIC_ASSERT(sizeof(cl_link_warm_64a_t) == 64, "link warm a 64");


CL_STATIC_ASSERT(sizeof(cl_link_warm_64a_t) == 64, "link warm a 64");

/* WARM B 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        /* Zero-Transform / Atom-Geometry (Truth) */
        _Atomic(uint32_t) payload_quantum_bytes;   /* default 64 */
        _Atomic(uint32_t) payload_headroom_bytes;  /* negotiated/setup */

        /* Transfer Semantik */
        _Atomic(uint32_t) accept_mode;             /* cl_accept_mode_t */
        _Atomic(uint32_t) durable_granularity_bytes;

        /* Integrity: possible/effective/state */
        _Atomic(uint32_t) crc_mode_possible;       /* bitmask over cl_crc_mode_t */
        _Atomic(uint32_t) fec_mode_possible;       /* bitmask over cl_fec_mode_t */

        _Atomic(uint32_t) crc_mode_effective;      /* cl_crc_mode_t */
        _Atomic(uint32_t) fec_mode_effective;      /* cl_fec_mode_t */

        /* FEC parameters */
        _Atomic(uint16_t) fec_n;                   /* atoms per block */
        _Atomic(uint16_t) fec_k;                   /* parity atoms */
        _Atomic(uint16_t) crc_state;               /* cl_feature_state_t (fits u16) */
        _Atomic(uint16_t) fec_state;               /* cl_feature_state_t */

        /* Predict-Verify Gate state (optional) */
        _Atomic(uint16_t) phv_state;               /* cl_feature_state_t */
        uint16_t reserved0;

        /* Compression ladder (effective) */
        _Atomic(uint32_t) comp_mode;               /* cl_comp_mode_t */
    };
    uint8_t raw[64];
} cl_link_warm_64b_t;

CL_STATIC_ASSERT(sizeof(cl_link_warm_64b_t) == 64, "link warm b 64");

/* COLD 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        uint32_t profile_desired;
        uint32_t backend_desired;
        uint32_t mode_desired;
        uint32_t reserved0;

        uint8_t src;
        uint8_t qual;
        uint16_t reserved1;

        uint32_t reserved2;
        uint64_t reserved3;
        uint64_t reserved4;
        uint64_t reserved5;
    };
    uint8_t raw[64];
} cl_link_cold_64_t;

CL_STATIC_ASSERT(sizeof(cl_link_cold_64_t) == 64, "link cold 64");

typedef struct CL_ALIGNAS(256) {
    cl_link_hot_64_t   hot;
    cl_link_warm_64a_t warm_a;
    cl_link_warm_64b_t warm_b;
    cl_link_cold_64_t  cold;
} cl_link_seg_256_t;

CL_STATIC_ASSERT(sizeof(cl_link_seg_256_t) == 256, "link seg 256");


#pragma once
/* ============================================================================
 * CommanderLink – Segment: Mesh Peer (256B) – FINAL Endausbau
 * Datei: include/cl_runtime/cl_seg_mesh_peer_256.h
 * ============================================================================
 *
 * Maximalmodell:
 * - Trust (state + last_event + denied_reason)
 * - Negotiation (geometry + integrity)
 * - Credits (pkts/bytes) + durable progress
 * ============================================================================
 */

#include <stdint.h>
#include <stdatomic.h>

#include "../cl_common/cl_align.h"
#include "../cl_manifest/cl_enums.h"
#include "../cl_manifest/cl_ids.h"
#include "../cl_manifest/cl_trust.h"
#include "cl_quality.h"

/* HOT 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) last_seen_ns;
        _Atomic(uint64_t) last_oob_rx_ns;

        _Atomic(uint32_t) trust_state;       /* cl_peer_trust_state_t */
        _Atomic(uint32_t) peer_state;        /* cl_peer_state_t */

        _Atomic(uint32_t) peer_overlay_ip;
        _Atomic(uint32_t) route_state;       /* cl_overlay_route_state_t (numeric) */

        _Atomic(uint32_t) constraint_flags;
        _Atomic(uint32_t) reason_code;

        _Atomic(uint32_t) trust_last_event;   /* cl_trust_event_id_t */
        _Atomic(uint32_t) trust_denied_reason;/* cl_reason_code_t subset */

        uint64_t reserved0;
    };
    uint8_t raw[64];
} cl_mesh_peer_hot_64_t;

CL_STATIC_ASSERT(sizeof(cl_mesh_peer_hot_64_t) == 64, "mesh peer hot 64");

/* WARM A 64 (negotiated) */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) rx_window_bytes;
        _Atomic(uint64_t) drain_bps;

        _Atomic(uint32_t) mtu_negotiated;
        _Atomic(uint32_t) ports_count;

        _Atomic(uint32_t) profile_allowed_mask;
        _Atomic(uint32_t) backend_allowed_mask;

        _Atomic(uint32_t) payload_quantum_bytes;
        _Atomic(uint32_t) payload_headroom_bytes;
    };
    uint8_t raw[64];
} cl_mesh_peer_warm_64a_t;

CL_STATIC_ASSERT(sizeof(cl_mesh_peer_warm_64a_t) == 64, "mesh peer warm a 64");

/* WARM B 64 (credits + accept/integrity progress) */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) grant_id;
        _Atomic(uint64_t) refill_epoch;

        _Atomic(uint32_t) grant_ttl_ms;
        _Atomic(uint32_t) grant_age_ms;

        _Atomic(uint64_t) credits_pkts;
        _Atomic(uint64_t) credits_bytes;

        _Atomic(uint32_t) accept_mode;        /* cl_accept_mode_t */
        _Atomic(uint32_t) durable_lag_bytes;

        _Atomic(uint64_t) durable_commit_epoch;
        _Atomic(uint64_t) reserved0;
    };
    uint8_t raw[64];
} cl_mesh_peer_warm_64b_t;

CL_STATIC_ASSERT(sizeof(cl_mesh_peer_warm_64b_t) == 64, "mesh peer warm b 64");

/* WARM C 64 (Join/Replay/Rate-Limit – gedämpft, monitor-sichtbar) */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint32_t) last_oob_epoch_seen;  /* lower 32 bits of last accepted epoch */
        _Atomic(uint32_t) replay_hits_total;

        _Atomic(uint32_t) join_attempts_total;
        _Atomic(uint32_t) join_rejects_total;

        _Atomic(uint32_t) rate_limited_total;
        _Atomic(uint32_t) reserved0;

        _Atomic(uint64_t) reserved1;
        _Atomic(uint64_t) reserved2;
        _Atomic(uint64_t) reserved3;
        _Atomic(uint64_t) reserved4;
        _Atomic(uint64_t) reserved5;
    };
    uint8_t raw[64];
} cl_mesh_peer_warm_64c_t;

CL_STATIC_ASSERT(sizeof(cl_mesh_peer_warm_64c_t) == 64, "mesh peer warm c 64");


/* COLD 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        cl_mesh_tag64_t mesh_tag64;
        cl_node_tag64_t node_tag64;

        uint8_t src;
        uint8_t qual;
        uint16_t reserved0;
        uint32_t reserved1;

        uint64_t reserved2;
        uint64_t reserved3;
    };
    uint8_t raw[64];
} cl_mesh_peer_cold_64_t;

CL_STATIC_ASSERT(sizeof(cl_mesh_peer_cold_64_t) == 64, "mesh peer cold 64");

typedef struct CL_ALIGNAS(256) {
    cl_mesh_peer_hot_64_t   hot;
    cl_mesh_peer_warm_64a_t warm_a;
    cl_mesh_peer_warm_64b_t warm_b;
    cl_mesh_peer_cold_64_t  cold;
} cl_mesh_peer_seg_256_t;

CL_STATIC_ASSERT(sizeof(cl_mesh_peer_seg_256_t) == 256, "mesh peer seg 256");


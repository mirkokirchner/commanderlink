#pragma once
/* ============================================================================
 * CommanderLink – Segment: Mesh Neighbor HOT (256B) – FINAL Endausbau
 * Datei: include/cl_runtime/cl_seg_mesh_neighbor_256.h
 * ============================================================================
 *
 * 32 Hot Neighbor Records (CL_MESH_HOT_NEIGHBORS) sind Ziel.
 * Maximalmodell: trust/state/negotiated/why + source/quality.
 * ============================================================================
 */

#include <stdint.h>
#include <stdatomic.h>

#include "../cl_common/cl_align.h"
#include "../cl_manifest/cl_enums.h"
#include "../cl_manifest/cl_ids.h"
#include "cl_quality.h"

/* HOT 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) last_seen_ns;

        _Atomic(uint32_t) trust_state;     /* cl_peer_trust_state_t */
        _Atomic(uint32_t) peer_state;      /* cl_peer_state_t */

        uint32_t peer_overlay_ip;
        uint32_t mtu_negotiated;

        uint32_t backend_active;           /* cl_backend_active_t */
        uint32_t profile_effective;        /* cl_transfer_profile_t */

        uint32_t loss_ppm;
        uint32_t jitter_p99_us;

        uint8_t src;
        uint8_t qual;
        uint16_t reserved0;
        uint32_t reserved1;
    };
    uint8_t raw[64];
} cl_mesh_neighbor_hot_64_t;

CL_STATIC_ASSERT(sizeof(cl_mesh_neighbor_hot_64_t) == 64, "mesh neighbor hot 64");

/* WARM 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        uint32_t ports_count;
        uint32_t link_speed_mbps;
        uint64_t rx_window_bytes;
        uint64_t drain_bps;

        uint32_t constraint_flags;
        uint32_t reason_code;

        uint16_t payload_quantum_bytes;
        uint16_t payload_headroom_bytes;
        uint16_t fec_n;
        uint16_t fec_k;

        uint32_t crc_mode_effective;
        uint32_t fec_mode_effective;
    };
    uint8_t raw[64];
} cl_mesh_neighbor_warm_64_t;

CL_STATIC_ASSERT(sizeof(cl_mesh_neighbor_warm_64_t) == 64, "mesh neighbor warm 64");

/* COLD 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        cl_node_tag64_t node_tag64;

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
} cl_mesh_neighbor_cold_64_t;

CL_STATIC_ASSERT(sizeof(cl_mesh_neighbor_cold_64_t) == 64, "mesh neighbor cold 64");

/* RSV 64 */
typedef union CL_ALIGNAS(8) { struct { uint64_t r[8]; }; uint8_t raw[64]; } cl_mesh_neighbor_rsv_64_t;

typedef struct CL_ALIGNAS(256) {
    cl_mesh_neighbor_hot_64_t  hot;
    cl_mesh_neighbor_warm_64_t warm;
    cl_mesh_neighbor_cold_64_t cold;
    cl_mesh_neighbor_rsv_64_t  rsv;
} cl_mesh_neighbor_seg_256_t;

CL_STATIC_ASSERT(sizeof(cl_mesh_neighbor_seg_256_t) == 256, "mesh neighbor seg 256");


#pragma once
/* ============================================================================
 * CommanderLink – OOB Wire (UDP/4343) – FINAL
 * Datei: include/cl_manifest/cl_oob_wire.h
 * ============================================================================
 *
 * Regeln:
 *   - Wire ist packed
 *   - Zahlenfelder sind *_le (explizit little-endian)
 *   - Beacon64 ist untrusted (kein auth)
 *   - Hello128 und Grant128 sind paranoid-auth (auth_tag16 Pflicht)
 * ============================================================================
 */

#include <stdint.h>

#include "../cl_common/cl_endian.h"
#include "cl_enums.h"

#define CL_OOB_PORT   4343u
#define CL_OOB_VER_1  1u
#define CL_OOB_MAGIC  0xC10Bu

typedef enum {
    CL_OOB_BEACON    = 1,
    CL_OOB_HELLO     = 2,
    CL_OOB_HELLO_ACK = 3,
    CL_OOB_GRANT     = 4,
    CL_OOB_BYE       = 5
} cl_oob_type_t;

#define CL_OOB_F_ACK_REQ      (1u << 0)
#define CL_OOB_F_AUTH_PRESENT (1u << 1)

/* Header 32B (universal) – FINAL, size-locked */
typedef struct __attribute__((packed)) {
    uint16_t magic_le;        /* CL_OOB_MAGIC */
    uint8_t  ver;             /* CL_OOB_VER_1 */
    uint8_t  type;            /* cl_oob_type_t */
    uint32_t msg_len_le;      /* sizeof(message) */

    uint16_t flags_le;        /* CL_OOB_F_* */
    uint8_t  hdr_crc8;        /* optional: 0 if unused */
    uint8_t  reserved0;       /* must be 0 */

    uint32_t epoch_le;        /* monotonic per sender (replay gate), u32 */

    uint64_t mesh_tag64_le;   /* domain */
    uint64_t node_tag64_le;   /* identity */
} cl_oob_hdr32_t;

_Static_assert(sizeof(cl_oob_hdr32_t) == 32, "oob hdr must be 32B");

typedef struct __attribute__((packed)) {
    cl_oob_hdr32_t h;              /* 32 */

    uint32_t caps_summary32_le;    /* 4 */
    uint16_t mtu_max_le;           /* 2 */
    uint8_t  profile_allowed_mask; /* 1 */
    uint8_t  backend_allowed_mask; /* 1 */

    uint32_t health_bits_le;       /* 4 */
    uint32_t build_id_le;          /* 4 */

    uint64_t mono_ts_ns_le;        /* 8 */

    uint64_t reserved0_le;         /* 8 -> padding to 64 */
} cl_oob_beacon64_t;

_Static_assert(sizeof(cl_oob_beacon64_t) == 64, "beacon 64");


/* ---------------- HELLO128 / HELLOACK128 (auth required) ---------------- */
typedef struct __attribute__((packed)) {
    cl_oob_hdr32_t h;              /* 32 */

    uint32_t caps_full32_le;       /* detailed caps summary */
    uint32_t proto_compat_le;      /* compat flags / schema family */

    uint32_t overlay_ip_le;        /* host order encoded as LE u32 */
    uint8_t  overlay_prefixlen;    /* usually 10 */
    uint8_t  rx_gate;              /* cl_rx_gate_t */
    uint8_t  tx_gate;              /* cl_tx_gate_t */
    uint8_t  trust_policy;         /* cl_join_mode_t / policy selector */

    uint16_t payload_quantum_le;   /* 64 (ZT atom) */
    uint16_t payload_headroom_le;  /* 64 default */
    uint16_t mtu_min_le;           /* path MTU min */
    uint16_t mtu_max_le;           /* path MTU max */

    uint32_t link_speed_mbps_le;   /* best-effort; 0=unknown; u32 reicht bis ~4 Tbps */
    uint16_t ports_count_le;       /* Anzahl physischer Ports (1..N) */
    uint16_t reserved_ports0;      /* muss 0 sein (Reserve/Alignment-Konstanz) */

    uint64_t rx_window_bytes_le;   /* receiver window */
    uint64_t drain_bps_le;         /* receiver drain rate */

    uint32_t grant_ttl_ms_le;      /* default 200ms */
    uint32_t prepare_ttl_ms_le;    /* default 50ms */

    uint32_t reason_code_le;       /* cl_reason_code_t */
    uint32_t constraint_flags_le;  /* cl_constraint_flags_t */

    uint32_t crc_mode_possible_le; /* bitmask over cl_crc_mode_t */
    uint32_t fec_mode_possible_le; /* bitmask over cl_fec_mode_t */

    uint32_t crc_mode_effective_le;/* chosen (optional hint) */
    uint32_t fec_nk_le;            /* high16=N low16=K */

    uint8_t  auth_tag16[16];       /* REQUIRED: HMAC/tag truncated */
} cl_oob_hello128_t;

_Static_assert(sizeof(cl_oob_hello128_t) == 128, "hello 128");
typedef cl_oob_hello128_t cl_oob_hello_ack128_t;

/* ---------------- GRANT128 (auth required) ---------------- */
typedef struct __attribute__((packed)) {
    cl_oob_hdr32_t h;              /* 32 */

    uint64_t grant_id_le;          /* 8 */
    uint64_t refill_epoch_le;      /* 8 */

    uint32_t grant_ttl_ms_le;      /* 4 */
    uint32_t session_id32_le;      /* 4 */

    uint32_t credits_pkts_le;      /* 4 */
    uint32_t credits_bytes_le;     /* 4 */

    uint32_t reason_code_le;       /* 4 */
    uint32_t constraint_flags_le;  /* 4 */

    uint16_t payload_quantum_le;   /* 2 */
    uint16_t payload_headroom_le;  /* 2 */
    uint16_t mtu_effective_le;     /* 2 */
    uint16_t reserved0;            /* 2 */

    uint16_t fec_n_le;             /* 2 */
    uint16_t fec_k_le;             /* 2 */
    uint16_t crc_mode_le;          /* 2 */
    uint16_t fec_mode_le;          /* 2 */

    uint32_t accept_mode_le;       /* 4 */
    uint32_t durable_granularity_le; /* 4 */

    uint8_t  auth_tag16[16];       /* 16 */

    uint8_t  pad[16];              /* 24 -> total 128 */
} cl_oob_grant128_t;

_Static_assert(sizeof(cl_oob_grant128_t) == 128, "grant 128");


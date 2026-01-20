#pragma once
/* ============================================================================
 * CommanderLink – Segment: Identity (256B, resident) – FINAL SSOT
 * Datei: include/cl_runtime/cl_identity_seg_256.h
 * ============================================================================
 *
 * SSOT-Vertrag:
 *   - Identity liefert ausschließlich cld (Persistenz: identity.bin + CRC32C).
 *   - CORE ist Authority über identity_state (VALID/INVALID/QUARANTINED).
 *   - HAL/LINK/FLOW/MONITOR erzeugen keine IDs und schreiben keine identity.bin.
 *
 * Root enthält KEINE IDs (NodeTag/MeshTag).
 * IDs sind Segment-Wahrheit (Identity-Segment).
 *
 * Layout:
 *   256B = 4 Cachelines à 64B
 * ============================================================================
 */

#include <stdint.h>
#include <stdatomic.h>

#include "../cl_common/cl_align.h"
#include "../cl_manifest/cl_ids.h"
#include "../cl_manifest/cl_enums.h"
#include "cl_quality.h"

/* Identity State (P0) */
typedef enum cl_identity_state {
    CL_IDENTITY_UNINITIALIZED = 0,
    CL_IDENTITY_VALID         = 1,
    CL_IDENTITY_MISSING       = 2,
    CL_IDENTITY_INVALID       = 3,
    CL_IDENTITY_QUARANTINED   = 4
} cl_identity_state_t;

/* CL0 (HOT 64): aktuelle Identity-Wahrheit */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) identity_epoch;     /* bump bei jedem valid publish */

        _Atomic(uint32_t) identity_state;     /* cl_identity_state_t */
        _Atomic(uint32_t) reason_code;        /* P0 reason code (SSOT reason order) */

        cl_node_tag64_t node_tag64;           /* 0 wenn nicht VALID */
        cl_mesh_tag64_t mesh_tag64;           /* 0 wenn nicht VALID */

        _Atomic(uint32_t) flags;              /* identity flags (z.B. domain-join später) */
        _Atomic(uint32_t) reserved0;

        uint8_t src;                          /* SOURCE (DIRECT/BEST_EFFORT/...) */
        uint8_t qual;                         /* QUALITY */
        uint16_t reserved1;

        uint32_t reserved2;
    };
    uint8_t raw[64];
} cl_identity_hot_64_t;

CL_STATIC_ASSERT(sizeof(cl_identity_hot_64_t) == 64, "identity hot 64");

/* CL1 (WARM 64): Persistenz-/Forensik-Meta (rein beschreibend) */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) file_epoch;         /* bump wenn identity.bin neu geschrieben */
        _Atomic(uint64_t) created_time_ns;    /* aus identity.bin (optional) */

        _Atomic(uint32_t) crc32c_ok;          /* 1 wenn CRC ok, sonst 0 */
        _Atomic(uint32_t) reserved0;

        _Atomic(uint64_t) last_load_ns;       /* wann cld zuletzt geladen hat */
        _Atomic(uint64_t) last_write_ns;      /* wann cld zuletzt geschrieben hat */
    };
    uint8_t raw[64];
} cl_identity_warm_64_t;

CL_STATIC_ASSERT(sizeof(cl_identity_warm_64_t) == 64, "identity warm 64");

/* CL2 (COLD 64): Salt-Hash (nur SSOT Sicht; keine Rohdatenexfiltration) */
typedef union CL_ALIGNAS(8) {
    struct {
        uint64_t salt_hash64;                 /* Hash(node_salt[32]) – optional */
        uint64_t fp_hash64;                   /* Hash(hw_fingerprint) – optional */
        uint64_t reserved0;
        uint64_t reserved1;
        uint64_t reserved2;
        uint64_t reserved3;
        uint64_t reserved4;
        uint64_t reserved5;
    };
    uint8_t raw[64];
} cl_identity_cold_64_t;

CL_STATIC_ASSERT(sizeof(cl_identity_cold_64_t) == 64, "identity cold 64");

/* CL3 (RSV 64): Reserve */
typedef union CL_ALIGNAS(8) {
    struct { uint64_t r[8]; };
    uint8_t raw[64];
} cl_identity_rsv_64_t;

CL_STATIC_ASSERT(sizeof(cl_identity_rsv_64_t) == 64, "identity rsv 64");

/* Segment 256 */
typedef struct CL_ALIGNAS(256) cl_identity_seg_256 {
    cl_identity_hot_64_t  hot;
    cl_identity_warm_64_t warm;
    cl_identity_cold_64_t cold;
    cl_identity_rsv_64_t  rsv;
} cl_identity_seg_256_t;

CL_STATIC_ASSERT(sizeof(cl_identity_seg_256_t) == 256, "identity seg 256");


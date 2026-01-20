#pragma once
/* ============================================================================
 * CommanderLink – Identity Contract (Manifest) – FINAL
 * Datei: include/cl_manifest/cl_ids.h
 * ============================================================================
 */

#include <stdint.h>

typedef uint64_t cl_mesh_tag64_t;
typedef uint64_t cl_node_tag64_t;
typedef uint64_t cl_session_id64_t;

typedef struct { uint8_t b[16]; } cl_uuid16_t;

/* Darstellung (Monitor/Forensik), deterministisch */
static inline uint32_t cl_id_short8_u32(uint64_t tag64) {
    uint64_t x = tag64 ^ (tag64 >> 32);
    return (uint32_t)x;
}


#pragma once
/* ============================================================================
 * CommanderLink – Overlay Manifest (v0.1) – FINAL
 * Datei: include/cl_manifest/cl_overlay.h
 * ============================================================================
 *
 * Ziel:
 *   Overlaynetz 100.64.0.0/10, deterministisch, Zero-ARP (Host-Routen /32).
 *
 * Maschinenregeln:
 *   - Host-Order im SHM.
 *   - Fail-Open: Interface nicht persistent, stirbt mit Daemon.
 * ============================================================================
 */

#include <stdint.h>

#include "../cl_common/cl_align.h"
#include "cl_enums.h"

#define CL_OVERLAY_PREFIXLEN            10u
#define CL_OVERLAY_NET_BASE_U32         0x64400000u
#define CL_OVERLAY_NET_MASK_U32         0xFFC00000u
#define CL_OVERLAY_HOST_MASK_U32        0x003FFFFFu

#define CL_OVERLAY_MAX_COLLISION_RETRIES 16u

#if defined(__linux__)
  #define CL_OVERLAY_IF_NAME_DEFAULT "cl0"
#elif defined(__sun__)
  #define CL_OVERLAY_IF_NAME_DEFAULT "cltun0"
#else
  #define CL_OVERLAY_IF_NAME_DEFAULT "tun0"
#endif

typedef enum {
    CL_ROUTE_SYNCED    = 0,
    CL_ROUTE_ORPHAN    = 1,
    CL_ROUTE_PENDING   = 2,
    CL_ROUTE_NONE      = 3,
    CL_ROUTE_COLLISION = 4
} cl_overlay_route_state_t;

static inline uint32_t cl_overlay_calc_ip_host_u32(uint64_t node_tag64,
                                                   uint64_t mesh_tag64,
                                                   uint8_t collision_offset)
{
    uint64_t mixed = node_tag64 ^ mesh_tag64 ^ (node_tag64 >> 22) ^ (mesh_tag64 << 5);
    uint32_t h22 = (uint32_t)((mixed ^ (mixed >> 22) ^ (mixed >> 44)) & CL_OVERLAY_HOST_MASK_U32);
    h22 = (h22 + (uint32_t)collision_offset) & CL_OVERLAY_HOST_MASK_U32;
    if (h22 == 0u) h22 = 1u;
    return (CL_OVERLAY_NET_BASE_U32 | h22);
}


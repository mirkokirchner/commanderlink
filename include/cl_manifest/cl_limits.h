#pragma once
/* ============================================================================
 * CommanderLink – Limits (Manifest) – FINAL
 * Datei: include/cl_manifest/cl_limits.h
 * ============================================================================
 */

#include <stdint.h>

#define CL_MAX_CORES                 512u
#define CL_MAX_NET_IFACES            16u
#define CL_MAX_GPUS                  8u
#define CL_MAX_MEM_CHANNELS          32u
#define CL_MAX_PCIE_LANES            256u

#define CL_MESH_HOT_NEIGHBORS        32u
#define CL_SWARM_MAX_TRACKED_PEERS   16384u
#define CL_ZFS_MAX_DATASETS          4096u

#define CL_DMA_SLOT_BYTES            4096u

#define CL_OOB_GRANT_TTL_MS_DEFAULT        200u
#define CL_INJECT_PREPARE_TTL_MS_DEFAULT   50u

#define CL_OVERLAY_PREFIXLEN_DEFAULT       10u

#define CL_ATOM_BYTES_DEFAULT              64u
#define CL_HEADROOM_DEFAULT_BYTES          64u


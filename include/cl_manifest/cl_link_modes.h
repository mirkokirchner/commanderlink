#pragma once
/* ============================================================================
 * CommanderLink – Link Modes (Manifest) – FINAL
 * Datei: include/cl_manifest/cl_link_modes.h
 * ============================================================================
 *
 * Zweck:
 *   Stabiler Include-Punkt für Link-Modi. Die Wahrheit liegt in cl_enums.h.
 * ============================================================================
 */

#include "cl_enums.h"

#define CL_LINK_MODE_RAW          CL_LINK_RAW
#define CL_LINK_MODE_FLOW         CL_LINK_FLOW
#define CL_LINK_MODE_FLOW_LZ4     CL_LINK_FLOW_LZ4
#define CL_LINK_MODE_FLOW_FEC     CL_LINK_FLOW_FEC
#define CL_LINK_MODE_FLOW_LZ4_FEC CL_LINK_FLOW_LZ4_FEC
#define CL_LINK_MODE_OOB          CL_LINK_OOB
#define CL_LINK_MODE_SYNC         CL_LINK_SYNC


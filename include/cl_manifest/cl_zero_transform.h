#pragma once
/* ============================================================================
 * CommanderLink – Zero-Transformation Standard (Manifest) – FINAL
 * Datei: include/cl_manifest/cl_zero_transform.h
 * ============================================================================
 */

#include <stdint.h>

#define CL_ATOM_BYTES 64u
#define CL_HEADROOM_DEFAULT_BYTES 64u

#define CL_ZT_FORBID_SHIFT_REPACK 1
#define CL_ZT_FORBID_HOT_MEMCPY   1
#define CL_ZT_FORBID_HOT_ENDIAN   1
#define CL_ZT_FORBID_HOT_ATOMICS  1

/* Wire payload muss NICHT zwingend multiple-of-64 sein (Alltagstauglichkeit) */
#define CL_ZT_WIRE_PAYLOAD_MUST_BE_MULTIPLE_OF_64 0


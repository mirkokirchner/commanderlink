#pragma once
/* ============================================================================
 * CommanderLink – Source/Quality (Runtime) – FINAL
 * Datei: include/cl_runtime/cl_quality.h
 * ============================================================================
 *
 * Zweck:
 *   - Herkunft und Güte von Telemetrie (Maximalmodell).
 *   - Monitor zeigt Source/Quality lückenlos.
 *   - Oracle/Flow entscheiden anhand Quality, ob Werte kontrollwirksam sind.
 * ============================================================================
 */

#include <stdint.h>

typedef enum {
    CL_SRC_UNSPECIFIED = 0,
    CL_SRC_DIRECT_HW   = 1,
    CL_SRC_OS_IFACE    = 2,
    CL_SRC_TOOL        = 3,
    CL_SRC_INFERRED    = 4,
    CL_SRC_SIMULATED   = 5
} cl_source_t;

typedef enum {
    CL_QUAL_UNSUPPORTED = 0,
    CL_QUAL_ESTIMATED   = 1,
    CL_QUAL_BEST_EFFORT = 2,
    CL_QUAL_DIRECT      = 3
} cl_quality_t;

static inline int cl_quality_control_ok(cl_quality_t q) {
    return (q == CL_QUAL_DIRECT) || (q == CL_QUAL_BEST_EFFORT);
}


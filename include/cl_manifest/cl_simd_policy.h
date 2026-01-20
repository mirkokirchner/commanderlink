#pragma once
/* ============================================================================
 * CommanderLink – SIMD/ISA Policy (Manifest) – FINAL
 * Datei: include/cl_manifest/cl_simd_policy.h
 * ============================================================================
 */

#include <stdint.h>
#include "cl_enums.h"
#include "cl_features.h"

typedef enum {
    CL_SIMD_EFF_SCALAR = 0,
    CL_SIMD_EFF_SSE42  = 1,
    CL_SIMD_EFF_AVX2   = 2,
    CL_SIMD_EFF_AVX512 = 3,
    CL_SIMD_EFF_AMX    = 4
} cl_simd_effective_t;

typedef enum {
    CL_AVX512_RISK_UNKNOWN = 0,
    CL_AVX512_RISK_SAFE    = 1,
    CL_AVX512_RISK_OFFSET  = 2
} cl_avx512_risk_t;

typedef uint32_t cl_simd_policy_flags_t;

#define CL_SIMD_PF_NONE                   0u
#define CL_SIMD_PF_ALLOW_AVX2             (1u << 0)
#define CL_SIMD_PF_ALLOW_AVX512           (1u << 1)
#define CL_SIMD_PF_ALLOW_AMX              (1u << 2)
#define CL_SIMD_PF_AVX512_REQUIRE_GENESIS (1u << 8)

static inline cl_simd_effective_t cl_simd_choose_effective(uint32_t cpu_isa_possible_mask,
                                                           cl_avx512_risk_t avx512_risk,
                                                           cl_budget_state_t budget_state,
                                                           uint32_t constraint_flags,
                                                           cl_simd_policy_flags_t policy_flags)
{
    if ((cpu_isa_possible_mask & CL_ISA_SSE42) == 0u) return CL_SIMD_EFF_SCALAR;

    if (budget_state == CL_BUDGET_SHADOW || budget_state == CL_BUDGET_RECLAIM) return CL_SIMD_EFF_SSE42;

    if (constraint_flags & (CL_CF_THERMAL_CAP | CL_CF_BOARD_VRM_CAP | CL_CF_POWER_UNSTABLE)) return CL_SIMD_EFF_SSE42;

    if ((policy_flags & CL_SIMD_PF_ALLOW_AVX2) && (cpu_isa_possible_mask & CL_ISA_AVX2)) {
        if ((policy_flags & CL_SIMD_PF_ALLOW_AVX512) &&
            (cpu_isa_possible_mask & CL_ISA_AVX512) &&
            (avx512_risk == CL_AVX512_RISK_SAFE))
        {
            return CL_SIMD_EFF_AVX512;
        }
        return CL_SIMD_EFF_AVX2;
    }

    return CL_SIMD_EFF_SSE42;
}


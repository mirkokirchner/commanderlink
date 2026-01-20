#pragma once
/* ============================================================================
 * CommanderLink – Features/Caps (Manifest) – FINAL (SSOT-konform)
 * Datei: include/cl_manifest/cl_features.h
 * ============================================================================
 *
 * Zweck:
 *   - Beschreibt Host- und System-Capabilities als Manifest-Datenmodell.
 *   - Wird von Runtime/Tools genutzt (ABI-Report, cld, Monitor etc.).
 *
 * SSOT-Regel:
 *   - Dieser Header muss in jeder Include-Reihenfolge kompilieren.
 *   - Keine "phantom typedefs": jeder verwendete Typ wird hier oder via SSOT-Header
 *     eindeutig definiert.
 *
 * Hinweis:
 *   - constraint_flags ist eine Bitmaske (u32), Bits sind in cl_enums.h definiert
 *     (CL_CONSTRAINT_*).
 * ============================================================================
 */

#include <stdint.h>
#include "cl_enums.h"

/* ---------------------------------------------------------------------------
 * Constraint Flags Typ (lokal, SSOT-sicher)
 *   - Bits: CL_CONSTRAINT_* aus cl_enums.h
 *   - Typ: u32 (bitmask)
 * ---------------------------------------------------------------------------
 */
typedef uint32_t cl_constraint_flags_t;

/* ---------------------------------------------------------------------------
 * CPU ISA Mask
 * ---------------------------------------------------------------------------
 */
typedef uint32_t cl_cpu_isa_mask_t;

#define CL_ISA_SSE42   (1u << 0)
#define CL_ISA_AVX2    (1u << 1)
#define CL_ISA_AVX512  (1u << 2)
#define CL_ISA_AMX     (1u << 3)
#define CL_ISA_AESNI   (1u << 4)
#define CL_ISA_PCLMUL  (1u << 5)

typedef enum {
    CL_ISA_EFF_SSE42  = 0,
    CL_ISA_EFF_AVX2   = 1,
    CL_ISA_EFF_AVX512 = 2,
    CL_ISA_EFF_AMX    = 3
} cl_cpu_isa_effective_t;

/* ---------------------------------------------------------------------------
 * Backend Possible Mask
 * ---------------------------------------------------------------------------
 */
typedef uint32_t cl_backend_mask_t;

#define CL_BE_M_BYPASS (1u << 0)
#define CL_BE_M_UDP    (1u << 1)
#define CL_BE_M_TCP    (1u << 2)

/* ---------------------------------------------------------------------------
 * Security caps
 * ---------------------------------------------------------------------------
 */
typedef struct {
    uint8_t openssl_present;
    uint8_t openssl_version_ok;
    uint8_t auth_required;
    uint8_t reserved0;
} cl_security_caps_t;

/* ---------------------------------------------------------------------------
 * Memory caps
 * ---------------------------------------------------------------------------
 */
typedef struct {
    uint64_t total_ram_bytes;
    uint64_t staging_cap_max_bytes;
    uint64_t staging_cap_current_bytes;
    uint64_t rx_window_current_bytes;

    uint8_t hugepages_supported;
    uint8_t hugepages_active;
    uint16_t reserved0;
} cl_mem_caps_t;

/* ---------------------------------------------------------------------------
 * NIC caps
 * ---------------------------------------------------------------------------
 */
typedef struct {
    uint16_t ports_count;
    uint16_t queues_count;
    uint32_t link_speed_mbps;

    uint16_t mtu_min;
    uint16_t mtu_max;

    cl_backend_mask_t backend_possible_mask;
} cl_nic_caps_t;

/* ---------------------------------------------------------------------------
 * Board caps
 * ---------------------------------------------------------------------------
 */
typedef struct {
    uint8_t vrm_supported;
    uint8_t ripple_supported;
    uint8_t pwr_limits_supported;
    uint8_t reserved0;

    cl_thermal_state_t board_therm_state;

    uint8_t power_unstable;
    uint16_t reserved1;
} cl_board_caps_t;

/* ---------------------------------------------------------------------------
 * Host snapshot (possible/effective + reasons)
 * ---------------------------------------------------------------------------
 */
typedef struct {
    /* Possible (Capabilities) */
    cl_cpu_isa_mask_t cpu_isa_possible;
    cl_backend_mask_t backend_possible;
    uint32_t profile_possible_mask;

    /* Effective (gewählte/aktive Stufe) */
    cl_cpu_isa_effective_t cpu_isa_effective;
    cl_backend_active_t backend_effective;
    cl_transfer_profile_t profile_effective;

    /* Constraints / Reason (SSOT) */
    cl_constraint_flags_t constraint_flags; /* Bitmask: CL_CONSTRAINT_* */
    cl_reason_code_t reason_code;

    /* Subsystems */
    cl_security_caps_t security;
    cl_mem_caps_t mem;
    cl_nic_caps_t nic;
    cl_board_caps_t board;

    uint32_t reserved2;
} cl_host_caps_t;


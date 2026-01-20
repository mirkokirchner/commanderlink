#pragma once
/* ============================================================================
 * CommanderLink – Enums/Flags (Manifest) – FINAL, EINZIGE WAHRHEIT
 * Datei: include/cl_manifest/cl_enums.h
 * ============================================================================
 *
 * Maschinenregel:
 *   - Alle Zustände/Flags/Reason Codes hier, keine Doppeldefinitionen.
 *   - „Maximalmodell“: UNSUPPORTED/DISABLED/IDLE/ACTIVE/UNKNOWN/FAULT sind Pflicht.
 * ============================================================================
 */

#include <stdint.h>

/* ---------------- Globale Health Bits ---------------- */
#define CL_HEALTH_OK               (1u << 0)
#define CL_HEALTH_WARN             (1u << 1)
#define CL_HEALTH_DEGRADED         (1u << 2)
#define CL_HEALTH_FAULT            (1u << 3)
#define CL_HEALTH_EMERGENCY        (1u << 4)

/* ---------------- Feature State ---------------- */
typedef enum {
    CL_FEAT_UNSUPPORTED = 0,
    CL_FEAT_DISABLED    = 1,
    CL_FEAT_IDLE        = 2,
    CL_FEAT_ACTIVE      = 3,
    CL_FEAT_UNKNOWN     = 4,
    CL_FEAT_FAULT       = 5
} cl_feature_state_t;

/* ---------------- Budget State ---------------- */
typedef enum {
    CL_BUDGET_UNKNOWN = 0,
    CL_BUDGET_OK,
    CL_BUDGET_WARN,
    CL_BUDGET_LIMIT,
    CL_BUDGET_HARD
} cl_budget_state_t;

/* ---------------- Backend Active ---------------- */
typedef enum {
    CL_BACKEND_NONE = 0,
    CL_BACKEND_UDP  = 1,
    CL_BACKEND_TCP  = 2,
    CL_BACKEND_XDP  = 3,
    CL_BACKEND_NETMAP = 4
} cl_backend_active_t;

/* ---------------- Path State ---------------- */
typedef enum {
    CL_PATH_BOOT = 0,
    CL_PATH_PASSIVE_MONITORING,
    CL_PATH_OOB_DISCOVERY,
    CL_PATH_NEGOTIATING,
    CL_PATH_ACTIVE_BYPASS,
    CL_PATH_ACTIVE_UDP,
    CL_PATH_DEGRADED,
    CL_PATH_TCP_FALLBACK
} cl_path_state_t;

/* RX/TX Gates */
typedef enum { CL_RX_NONE=0, CL_RX_OOB_ONLY=1, CL_RX_FULL=2 } cl_rx_gate_t;
typedef enum { CL_TX_NONE=0, CL_TX_OOB_ONLY=1, CL_TX_FULL=2 } cl_tx_gate_t;

/* ---------------- Link Mode ---------------- */
typedef enum {
    CL_LINK_MODE_NONE = 0,
    CL_LINK_MODE_OS_STACK,
    CL_LINK_MODE_TUN_INGRESS,
    CL_LINK_MODE_VECTOR_UDP,
    CL_LINK_MODE_VECTOR_BYPASS,
    CL_LINK_MODE_VECTOR_TCP
} cl_link_mode_t;

/* ---------------- Profiles ---------------- */
typedef enum { CL_PROFILE_BASIC=0, CL_PROFILE_BALANCED=1, CL_PROFILE_HPP_PLUS=2 } cl_transfer_profile_t;

/* ---------------- Operational Profiles (Traffic Steering) ----------------
 * SSOT:
 *   Dieses Profil steuert den Traffic-Scope (ACCELERATE vs REDIRECT) und ist
 *   strikt getrennt von cl_transfer_profile_t (BASIC/BALANCED/HPP+).
 *
 *   - ACCELERATE: selektive Umleitung (Whitelist), Management bleibt OS-nativ.
 *   - REDIRECT: total redirect (Expert), nur nach Safe-Cutover.
 */
typedef enum {
    CL_OP_PROFILE_OFF         = 0,
    CL_OP_PROFILE_ACCELERATE  = 1,
    CL_OP_PROFILE_REDIRECT    = 2
} cl_operational_profile_t;

/* Alias-Makros für Service-State desired_profile (lesbarer Code, keine Kollisionen) */
#define CL_DESIRED_PROFILE_OFF        CL_OP_PROFILE_OFF
#define CL_DESIRED_PROFILE_ACCELERATE CL_OP_PROFILE_ACCELERATE
#define CL_DESIRED_PROFILE_REDIRECT   CL_OP_PROFILE_REDIRECT

/* Thermal */
typedef enum { CL_THERM_OPTIMAL=0, CL_THERM_WARM=1, CL_THERM_HOT=2, CL_THERM_CRITICAL=3 } cl_thermal_state_t;

/* Peer online */
typedef enum { CL_PEER_UNKNOWN=0, CL_PEER_ONLINE=1, CL_PEER_OFFLINE=2 } cl_peer_state_t;

/* Trust */
typedef enum {
    CL_TRUST_UNKNOWN=0,
    CL_TRUST_SEEN_UNTRUSTED=1,
    CL_TRUST_NEGOTIATING_AUTH=2,
    CL_TRUST_TRUSTED_ACTIVE=3,
    CL_TRUST_DENIED=4
} cl_peer_trust_state_t;

/* Constraint flags (global) */
#define CL_CONSTRAINT_NONE            0u
#define CL_CONSTRAINT_THERMAL         (1u << 0)
#define CL_CONSTRAINT_VRM             (1u << 1)
#define CL_CONSTRAINT_POWER           (1u << 2)
#define CL_CONSTRAINT_TRUST           (1u << 3)
#define CL_CONSTRAINT_WATCHDOG        (1u << 4)
#define CL_CONSTRAINT_BACKEND         (1u << 5)
#define CL_CONSTRAINT_INTERACTIVE     (1u << 6)

/* Reason codes (minimal set; erweitert später) */
typedef enum {
    CL_REASON_NONE = 0,
    CL_REASON_P0_IDENTITY_INVALID = 1,
    CL_REASON_P0_IDENTITY_QUARANTINE = 2,
    CL_REASON_P1_TRUST_BLOCKED = 10,
    CL_REASON_P2_WATCHDOG = 20,
    CL_REASON_P3_BACKEND = 30,
    CL_REASON_P4_BUDGET = 40,
    CL_REASON_P5_QUALITY = 50
} cl_reason_code_t;


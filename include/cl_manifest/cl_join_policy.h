#pragma once
/* ============================================================================
 * CommanderLink – Join/Trust Policy (Manifest) – FINAL
 * Datei: include/cl_manifest/cl_join_policy.h
 * ============================================================================
 */

#include <stdint.h>
#include "cl_enums.h"
#include "cl_trust.h"

typedef enum { CL_JOIN_LAB_PERMISSIVE=0, CL_JOIN_PARANOID_STRICT=1 } cl_join_mode_t;

#define CL_JOIN_MODE_DEFAULT CL_JOIN_PARANOID_STRICT
#define CL_JOIN_REQUIRE_AUTH 1

#define CL_JOIN_HELLO_TIMEOUT_MS  500u
#define CL_JOIN_RETRY_MAX         3u
#define CL_JOIN_REPLAY_WINDOW_DEFAULT 64u


#pragma once
/* ============================================================================
 * CommanderLink – Trust / Join State Machine (Manifest) – FINAL
 * Datei: include/cl_manifest/cl_trust.h
 * ============================================================================
 */

#include <stdint.h>
#include "cl_enums.h"

static inline int cl_trust_allows_routes(cl_peer_trust_state_t st) { return st == CL_TRUST_TRUSTED_ACTIVE; }
static inline int cl_trust_allows_grants(cl_peer_trust_state_t st) { return st == CL_TRUST_TRUSTED_ACTIVE; }
static inline int cl_trust_allows_dataplane(cl_peer_trust_state_t st) { return st == CL_TRUST_TRUSTED_ACTIVE; }

typedef enum {
    CL_TR_EVT_NONE = 0,
    CL_TR_EVT_SEEN_BEACON = 1,
    CL_TR_EVT_SEEN_HELLO  = 2,
    CL_TR_EVT_AUTH_OK     = 10,
    CL_TR_EVT_AUTH_FAIL   = 11,
    CL_TR_EVT_REPLAY      = 20,
    CL_TR_EVT_TTL_EXPIRED = 21,
    CL_TR_EVT_TRUSTED     = 30,
    CL_TR_EVT_DENIED      = 31,
    CL_TR_EVT_ROUTE_BLOCKED = 40,
    CL_TR_EVT_GRANT_BLOCKED = 41
} cl_trust_event_id_t;

typedef struct {
    uint64_t last_epoch;
    uint64_t window_mask;
    uint64_t window_base;
} cl_replay_window_t;

static inline void cl_replay_reset(cl_replay_window_t *w) {
    w->last_epoch = 0;
    w->window_mask = 0;
    w->window_base = 0;
}


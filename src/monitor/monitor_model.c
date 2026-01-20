#define _POSIX_C_SOURCE 200809L
#include "monitor_model.h"

#include <string.h>
#include <time.h>
#include <stdatomic.h>

/* SSOT runtime segments used in Stage-1 model */
#include <cl_runtime/cl_service_seg_256.h>
#include <cl_runtime/cl_identity_seg_256.h>
#include <cl_runtime/cl_seg_link_256.h>
#include <cl_runtime/cl_shm_layout.h>

static uint64_t now_ns_monotonic(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) return 0;
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

void ui_model_update_from_shm(cl_monitor_state_t *st) {
    if (!st) return;

    cl_ui_model_t *m = &st->model;

    /* Always reflect current SSOT connection status */
    m->mapped_core = st->status.mapped_core;
    m->mapped_bulk = st->status.mapped_bulk;
    m->root_ok     = st->status.root_ok;
    m->toc_ok      = st->status.toc_ok;
    m->last_errno  = st->status.last_errno;
    m->error_text  = st->status.error_text;

    m->connected = (st->status.mapped_core && st->status.root_ok && st->status.toc_ok);

    if (!m->connected) {
        /* keep rest (previous) or zero; here we keep it stable */
        return;
    }

    const cl_root_4096_t *root = st->shm.root;

    m->uptime_ns          = atomic_load(&root->uptime_ns);
    m->abi_layout_checksum= (uint64_t)root->abi_layout_checksum;
    m->bulk_present       = atomic_load(&root->bulk_present);

    /* --- Service (CORE0 slot) --- */
    const cl_service_seg_256_t *svc =
        (const cl_service_seg_256_t*)mon_shm_get_segment(&st->shm, (uint16_t)CL_SERVICE_SEG_256, 0);

    if (svc) {
        const cl_service_slot_32_t *core0 = &svc->g1.s0; /* SSOT: CORE0 slot */

        m->core0_pid     = atomic_load(&core0->pid);
        m->core0_running = atomic_load(&core0->running);
        m->core0_hb_ns   = atomic_load(&core0->last_heartbeat_ns);

        uint64_t now = now_ns_monotonic();
        if (now && m->core0_hb_ns && now > m->core0_hb_ns) {
            m->core0_hb_age_ms = (now - m->core0_hb_ns) / 1000000ull;
        } else {
            m->core0_hb_age_ms = 0;
        }
    } else {
        m->core0_pid = 0;
        m->core0_running = 0;
        m->core0_hb_ns = 0;
        m->core0_hb_age_ms = 0;
    }

    /* --- Identity --- */
    const cl_identity_seg_256_t *ids =
        (const cl_identity_seg_256_t*)mon_shm_get_segment(&st->shm, (uint16_t)CL_IDENTITY_SEG_256, 0);

    if (ids) {
        m->identity_state  = atomic_load(&ids->hot.identity_state);
        m->identity_reason = atomic_load(&ids->hot.reason_code);
        m->node_tag64      = (uint64_t)ids->hot.node_tag64;
        m->mesh_tag64      = (uint64_t)ids->hot.mesh_tag64;
    } else {
        m->identity_state = 0;
        m->identity_reason = 0;
        m->node_tag64 = 0;
        m->mesh_tag64 = 0;
    }

    /* --- Link summary (optional) --- */
    const cl_link_seg_256_t *lnk =
        (const cl_link_seg_256_t*)mon_shm_get_segment(&st->shm, (uint16_t)CL_LINK_SEG_256, 0);

    if (lnk) {
        m->link_backend = atomic_load(&lnk->hot.backend_active);
        m->link_path    = atomic_load(&lnk->hot.path_state);
        m->link_health  = atomic_load(&lnk->hot.global_health);
        m->link_reason  = atomic_load(&lnk->hot.reason_code);
    } else {
        m->link_backend = 0;
        m->link_path = 0;
        m->link_health = 0;
        m->link_reason = 0;
    }
}


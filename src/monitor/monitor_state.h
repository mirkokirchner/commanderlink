#ifndef CL_MONITOR_STATE_H
#define CL_MONITOR_STATE_H

#include <stddef.h>
#include <stdint.h>

#include "monitor_shm.h"   /* mon_shm_t + mon_shm_status_t */

typedef enum {
    TAB_DASHBOARD = 0,
    TAB_CPU,
    TAB_MEM,
    TAB_COUNT
} cl_mon_tab_t;

/* SSOT UI Model (nur Cache, keine Policy) */
typedef struct {
    /* connection / validity */
    int connected;          /* mapped_core && root_ok && toc_ok */
    int root_ok;
    int toc_ok;
    int mapped_core;
    int mapped_bulk;
    int last_errno;
    const char *error_text;

    /* root */
    uint64_t uptime_ns;
    uint64_t abi_layout_checksum;
    uint32_t bulk_present;

    /* core0 (service slot) */
    uint32_t core0_pid;
    uint32_t core0_running;
    uint64_t core0_hb_ns;
    uint64_t core0_hb_age_ms;

    /* identity */
    uint32_t identity_state;
    uint32_t identity_reason;
    uint64_t node_tag64;
    uint64_t mesh_tag64;

    /* link summary (optional stage-1) */
    uint32_t link_backend;
    uint32_t link_path;
    uint32_t link_health;
    uint32_t link_reason;

} cl_ui_model_t;

typedef struct {
    cl_mon_tab_t active_tab;
    int running;

    mon_shm_t shm;
    mon_shm_status_t status;

    cl_ui_model_t model;
} cl_monitor_state_t;

cl_monitor_state_t* monitor_state(void);

/* Model update (SSOT read → cache) */
void ui_model_update_from_shm(cl_monitor_state_t *st);

#endif /* CL_MONITOR_STATE_H */


#include "monitor_state.h"

static cl_monitor_state_t g_state = {
    .active_tab = TAB_DASHBOARD,
    .running = 1
};

cl_monitor_state_t* monitor_state(void) {
    return &g_state;
}


#ifndef CL_MONITOR_H
#define CL_MONITOR_H

#include <stdint.h>

typedef enum {
    CL_MON_TAB_DASH = 0,
    CL_MON_TAB_INV  = 1,
    CL_MON_TAB_CPU  = 2,
    CL_MON_TAB_MEM  = 3,
    CL_MON_TAB_NIC  = 4,
    CL_MON_TAB_MESH = 5,
    CL_MON_TAB_DMA  = 6,
    CL_MON_TAB_ZFS  = 7,
    CL_MON_TAB_PCIE = 8,
    CL_MON_TAB_GPU  = 9,
    CL_MON_TAB_ORA  = 10,
    CL_MON_TAB_AUD  = 11
} cl_mon_tab_t;

/* Startet den Monitor (ncurses loop).
 * Rückgabe: exit code.
 */
int cl_monitor_run(void);

#endif


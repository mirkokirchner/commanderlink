#include "monitor_curses.h"
#include "monitor_state.h"
#include "tabs.h"

void monitor_handle_input(int key) {
    cl_monitor_state_t *st = monitor_state();
    switch (key) {
        /* Global Navigation */
        case '\t': 
        case KEY_RIGHT:
            st->active_tab = (st->active_tab + 1) % TAB_COUNT;
            break;

        case KEY_BTAB:
        case KEY_LEFT:
            st->active_tab = (st->active_tab - 1 + TAB_COUNT) % TAB_COUNT;
            break;

        case KEY_F(1): st->active_tab = TAB_DASHBOARD; break;
        case KEY_F(2): st->active_tab = TAB_CPU; break;
        case KEY_F(3): st->active_tab = TAB_MEM; break;
        
        case KEY_F(10): 
        case 27: /* ESC */
            st->running = 0;
            break;

        default:
            /* Pass-through an Tab */
            if (st->active_tab == TAB_DASHBOARD) {
                tab_dashboard_input(key);
            }
            break;
    }
}

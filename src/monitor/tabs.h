#ifndef CL_TABS_H
#define CL_TABS_H

#include "monitor_curses.h"

/* Jeder Tab muss diese Signatur haben */
typedef void (*tab_draw_fn)(int h, int w);
typedef void (*tab_input_fn)(int key);

/* Dashboard Tab (Demo) */
void tab_dashboard_draw(int h, int w);
void tab_dashboard_input(int key);

#endif

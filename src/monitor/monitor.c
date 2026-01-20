#define _POSIX_C_SOURCE 200809L
/*
 * SSOT Monitor Loop (Raylib/clgui):
 * - Poll SHM (SSOT)
 * - Build Model (cache)
 * - Render (one window)
 */

#include <stdio.h>
#include <stdlib.h>

#include "../clgui/clgui.h"
#include "monitor_shm.h"
#include "monitor_state.h"
#include "monitor_model.h"
#include "raylib.h"

/* tab */
void tab_dashboard_draw(void);

int main(void) {
    cl_monitor_state_t *st = monitor_state();
    st->running = 1;
    st->active_tab = TAB_DASHBOARD;

    mon_shm_init(&st->shm);
    clgui_init();

    while (st->running && !clgui_should_close()) {

        /* A) SSOT poll */
        st->status = mon_shm_poll(&st->shm);

        /* B) SSOT -> model cache */
        ui_model_update_from_shm(st);

        /* C) Render */
        clgui_begin_frame();
        clgui_draw_background();

        /* single-window UI */
        tab_dashboard_draw();

        clgui_draw_taskbar(false);
        clgui_end_frame();

        /* Input (bounded) */
        if (IsKeyPressed(KEY_R)) {
            mon_shm_reset(&st->shm);
        }
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_F10)) {
            st->running = 0;
        }
    }

    mon_shm_close(&st->shm);
    clgui_shutdown();
    return 0;
}


#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "../../clgui/clgui.h"
#include "../monitor_state.h"

static void fmt_ns_hms(char *buf, size_t sz, uint64_t ns) {
    uint64_t s = ns / 1000000000ULL;
    uint64_t h = s / 3600ULL;
    uint64_t m = (s % 3600ULL) / 60ULL;
    uint64_t sec = s % 60ULL;
    snprintf(buf, sz, "%02" PRIu64 ":%02" PRIu64 ":%02" PRIu64, h, m, sec);
}

void tab_dashboard_draw(void) {
    cl_monitor_state_t *st = monitor_state();
    const cl_ui_model_t *m = &st->model;

    /* EIN Fenster – immer */
    clgui_draw_window(50, 50, 900, 600, "CommanderLink Monitor [SSOT]");

    char buf[160];

    /* Offline/Invalid: bleibt im selben Fenster, keine Extra-Overlays */
    if (!m->mapped_core) {
        snprintf(buf, sizeof(buf), "%s (errno=%d)", m->error_text ? m->error_text : "offline", m->last_errno);
        clgui_draw_label_value(80, 120, "STATE:", "OFFLINE");
        clgui_draw_label_value(80, 150, "DETAIL:", buf);
        clgui_draw_label_value(80, 180, "ACTION:", "Run: sudo clinit --recreate, then sudo cld");
        return;
    }
    if (!m->root_ok || !m->toc_ok) {
        clgui_draw_label_value(80, 120, "STATE:", "INVALID");
        clgui_draw_label_value(80, 150, "DETAIL:", m->error_text ? m->error_text : "root/toc invalid");
        clgui_draw_label_value(80, 180, "ACTION:", "Run: sudo clinit --recreate");
        return;
    }

    /* Online */
    clgui_draw_label_value(80, 120, "STATE:", "CONNECTED");

    /* Root */
    char tbuf[64];
    fmt_ns_hms(tbuf, sizeof(tbuf), m->uptime_ns);
    clgui_draw_label_value(80, 160, "Uptime:", tbuf);

    snprintf(buf, sizeof(buf), "0x%016" PRIx64, m->abi_layout_checksum);
    clgui_draw_label_value(80, 190, "ABI:", buf);

    clgui_draw_label_value(80, 220, "Bulk:", m->bulk_present ? "PRESENT" : "RECLAIMED");

    /* CORE0 */
    snprintf(buf, sizeof(buf), "%s (pid=%" PRIu32 ")", m->core0_running ? "RUNNING" : "DOWN", m->core0_pid);
    clgui_draw_label_value(80, 270, "CORE0:", buf);

    snprintf(buf, sizeof(buf), "%" PRIu64 " ms", m->core0_hb_age_ms);
    clgui_draw_label_value(80, 300, "HB Age:", buf);

    float frac = 0.0f;
    if (m->core0_hb_age_ms >= 5000) frac = 1.0f;
    else frac = (float)m->core0_hb_age_ms / 5000.0f;
    clgui_draw_progressbar(280, 300, 400, 20, frac, (m->core0_hb_age_ms >= 4000));

    /* Identity */
    snprintf(buf, sizeof(buf), "state=%" PRIu32 " reason=%" PRIu32, m->identity_state, m->identity_reason);
    clgui_draw_label_value(80, 350, "Identity:", buf);

    snprintf(buf, sizeof(buf), "0x%016" PRIx64, m->node_tag64);
    clgui_draw_label_value(80, 380, "NodeTag:", buf);

    snprintf(buf, sizeof(buf), "0x%016" PRIx64, m->mesh_tag64);
    clgui_draw_label_value(80, 410, "MeshTag:", buf);

    /* Link (optional stage-1) */
    snprintf(buf, sizeof(buf), "backend=%" PRIu32 " path=%" PRIu32, m->link_backend, m->link_path);
    clgui_draw_label_value(80, 460, "Link:", buf);

    snprintf(buf, sizeof(buf), "health=%" PRIu32 " reason=%" PRIu32, m->link_health, m->link_reason);
    clgui_draw_label_value(80, 490, "LinkStat:", buf);
}

void tab_dashboard_input(int key) { (void)key; }


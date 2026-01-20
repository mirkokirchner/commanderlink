/* src/monitor/monitor_ui.h */
#ifndef CL_MONITOR_UI_H
#define CL_MONITOR_UI_H

#define _XOPEN_SOURCE_EXTENDED 1
#include <ncurses.h>

/* --- FARB-DEFINITIONEN (Das Design) --- */
#define CL_PAIR_BG          1   /* Desktop: Weiß auf Blau */
#define CL_PAIR_HEADER      2   /* App-Titel: Schwarz auf Cyan */
#define CL_PAIR_STATUS      3   /* Statusbar: Schwarz auf Grau */

/* Fenster / Popup Design */
#define CL_PAIR_WIN_HEADER  10  /* Titelleiste: Weiß auf BLAU */
#define CL_PAIR_WIN_BODY    11  /* Inhalt: Weiß auf SCHWARZ */
#define CL_PAIR_WIN_BORDER  12  /* Rahmen: Cyan auf SCHWARZ */

/* Elemente */
#define CL_PAIR_TEXT        20  /* Normaler Text */
#define CL_PAIR_BAR_FILL    21  /* Ladebalken voll */
#define CL_PAIR_BAR_EMPTY   22  /* Ladebalken leer */
#define CL_PAIR_ALERT       23  /* Roter Alarm Text */

/* --- FUNKTIONEN --- */
void ui_init(void);
void ui_shutdown(void);

/* Basis */
void ui_draw_screen_bg(void);
void ui_draw_header(const char *title, const char *subtitle);
void ui_draw_statusbar(const char *left, const char *right);

/* Vorlagen (Templates) */
void ui_draw_box(int y, int x, int h, int w, const char *title);
void ui_draw_label_value(int y, int x, const char *label, const char *value);
void ui_draw_progressbar(int y, int x, int width, int permille, int is_alert);

/* DAS FENSTER (Die "Blaue Leiste" Vorlage) */
/* Automatisch zentriert, mit blauem Header und schwarzem Body */
void ui_draw_styled_window(const char *title, const char **lines, int line_count);

#endif

/* src/monitor/monitor_ui.c */
#include "monitor_ui.h"
#include <string.h>
#include <stdlib.h>

void ui_init(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
#ifdef NCURSES_VERSION
    use_default_colors();
#endif

/* --- THEME DEFINITION (Golden Edition) --- */
    
    /* 1. Desktop Hintergrund (Blau) */
    init_pair(CL_PAIR_BG,          COLOR_WHITE, COLOR_BLACK);
    
    /* 2. Fenster Header (Solid Blue Bar) */
    init_pair(CL_PAIR_WIN_HEADER,  COLOR_WHITE, COLOR_BLUE);
    
    /* 3. Fenster Body (Solid Black) */
    init_pair(CL_PAIR_WIN_BODY,    COLOR_WHITE, COLOR_BLACK);
    
    /* 4. Fenster Rahmen (Cyan Tech Look) */
    init_pair(CL_PAIR_WIN_BORDER,  COLOR_WHITE,  COLOR_BLACK);
    
    /* 5. Text & Elemente */
    init_pair(CL_PAIR_TEXT,        COLOR_WHITE, COLOR_BLACK);
    init_pair(CL_PAIR_ALERT,       COLOR_RED,   COLOR_BLACK);
}
void ui_shutdown(void) {
    endwin();
}

void ui_draw_screen_bg(void) {
    bkgd(COLOR_PAIR(CL_PAIR_BG));
    erase();
}

void ui_draw_header(const char *title, const char *subtitle) {
    int w = getmaxx(stdscr);
    attron(COLOR_PAIR(CL_PAIR_HEADER) | A_BOLD);
    mvprintw(0, 0, " %s", title);
    for(int i=strlen(title)+1; i<w; i++) addch(' ');
    
    mvprintw(1, 0, " %s", subtitle);
    for(int i=strlen(subtitle)+1; i<w; i++) addch(' ');
    attroff(COLOR_PAIR(CL_PAIR_HEADER) | A_BOLD);
}

void ui_draw_statusbar(const char *left, const char *right) {
    int h, w; getmaxyx(stdscr, h, w);
    attron(COLOR_PAIR(CL_PAIR_STATUS));
    move(h-1, 0);
    for(int i=0; i<w; i++) addch(' ');
    mvprintw(h-1, 1, "%s", left);
    if(right) mvprintw(h-1, w-strlen(right)-1, "%s", right);
    attroff(COLOR_PAIR(CL_PAIR_STATUS));
}

void ui_draw_box(int y, int x, int h, int w, const char *title) {
    /* LAYER 1: Body (Schwarzes Loch) */
    /* Wir füllen den Bereich mit schwarzen Leerzeichen, um den blauen Desktop zu übermalen */
    attron(COLOR_PAIR(CL_PAIR_WIN_BODY));
    for(int i = 0; i < h; i++) {
        mvhline(y + i, x, ' ', w);
    }
    attroff(COLOR_PAIR(CL_PAIR_WIN_BODY));

    /* LAYER 2: Rahmen (Cyan Wireframe) */
    attron(COLOR_PAIR(CL_PAIR_WIN_BORDER));
    /* Ecken */
    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + w - 1, ACS_URCORNER);
    mvaddch(y + h - 1, x, ACS_LLCORNER);
    mvaddch(y + h - 1, x + w - 1, ACS_LRCORNER);
    
    /* Linien */
    mvhline(y, x + 1, ACS_HLINE, w - 2);            /* Oben */
    mvhline(y + h - 1, x + 1, ACS_HLINE, w - 2);    /* Unten */
    mvvline(y + 1, x, ACS_VLINE, h - 2);            /* Links */
    mvvline(y + 1, x + w - 1, ACS_VLINE, h - 2);    /* Rechts */
    attroff(COLOR_PAIR(CL_PAIR_WIN_BORDER));

    /* LAYER 3: Titelleiste (Solid Blue Bar) */
    if (title) {
        /* Wir zeichnen einen blauen Balken in Zeile y+1 (innerhalb des Rahmens) */
        attron(COLOR_PAIR(CL_PAIR_WIN_HEADER) | A_BOLD);
        mvhline(y + 1, x + 1, ' ', w - 2); 
        
        /* Titel zentrieren */
        int text_len = strlen(title);
        int tx = x + 1 + ((w - 2) / 2) - (text_len / 2);
        mvprintw(y + 1, tx, "%s", title);
        attroff(COLOR_PAIR(CL_PAIR_WIN_HEADER) | A_BOLD);

        /* LAYER 4: Trennlinie (Separator) */
        /* Eine Linie unter dem Titel (Zeile y+2), verbunden mit dem Rahmen */
        attron(COLOR_PAIR(CL_PAIR_WIN_BORDER));
        mvaddch(y + 2, x, ACS_LTEE);              /* Linkes T-Stück */
        mvhline(y + 2, x + 1, ACS_HLINE, w - 2);  /* Linie */
        mvaddch(y + 2, x + w - 1, ACS_RTEE);      /* Rechtes T-Stück */
        attroff(COLOR_PAIR(CL_PAIR_WIN_BORDER));
    }
}
void ui_draw_label_value(int y, int x, const char *label, const char *value) {
    attron(COLOR_PAIR(CL_PAIR_TEXT));
    mvprintw(y, x, "%s:", label);
    attron(A_BOLD);
    mvprintw(y, x + strlen(label) + 2, "%s", value);
    attroff(A_BOLD | COLOR_PAIR(CL_PAIR_TEXT));
}

void ui_draw_progressbar(int y, int x, int width, int permille, int is_alert) {
    if (width < 2) return;
    int fill = (width * permille) / 1000;
    
    int pair = is_alert ? CL_PAIR_ALERT : CL_PAIR_BAR_FILL;
    
    attron(COLOR_PAIR(pair));
    for(int i=0; i<fill; i++) mvprintw(y, x+i, "#");
    attroff(COLOR_PAIR(pair));
    
    attron(COLOR_PAIR(CL_PAIR_BAR_EMPTY));
    for(int i=fill; i<width; i++) mvprintw(y, x+i, ".");
    attroff(COLOR_PAIR(CL_PAIR_BAR_EMPTY));
}

/* --- DAS STYLED WINDOW (Deine Vorlage) --- */
void ui_draw_styled_window(const char *title, const char **lines, int line_count) {
    int scr_h, scr_w;
    getmaxyx(stdscr, scr_h, scr_w);
    
    /* 1. Größe berechnen */
    int max_len = title ? strlen(title)+4 : 20;
    for(int i=0; i<line_count; i++) {
        int l = strlen(lines[i]);
        if(l > max_len) max_len = l;
    }
    int w = max_len + 6;
    int h = line_count + 6; /* Rahmen(2) + Header(1) + Trenner(1) + Padding(2) */
    
    int y = (scr_h - h) / 2;
    int x = (scr_w - w) / 2;
    
    /* 2. Body füllen (SCHWARZ) */
    attron(COLOR_PAIR(CL_PAIR_WIN_BODY));
    for(int i=0; i<h; i++) mvhline(y+i, x, ' ', w);
    attroff(COLOR_PAIR(CL_PAIR_WIN_BODY));
    
    /* 3. Rahmen zeichnen (CYAN auf SCHWARZ) */
    attron(COLOR_PAIR(CL_PAIR_WIN_BORDER));
    mvaddch(y, x, ACS_ULCORNER); mvaddch(y, x+w-1, ACS_URCORNER);
    mvaddch(y+h-1, x, ACS_LLCORNER); mvaddch(y+h-1, x+w-1, ACS_LRCORNER);
    for(int i=1; i<w-1; i++) { mvaddch(y, x+i, ACS_HLINE); mvaddch(y+h-1, x+i, ACS_HLINE); }
    for(int i=1; i<h-1; i++) { mvaddch(y+i, x, ACS_VLINE); mvaddch(y+i, x+w-1, ACS_VLINE); }
    attroff(COLOR_PAIR(CL_PAIR_WIN_BORDER));
    
    /* 4. Titelleiste (BLAUER BALKEN) */
    attron(COLOR_PAIR(CL_PAIR_WIN_HEADER) | A_BOLD);
    mvhline(y+1, x+1, ' ', w-2); /* Blauen Hintergrund malen */
    if (title) {
        int tx = x + 1 + ((w-2)/2) - (strlen(title)/2);
        mvprintw(y+1, tx, "%s", title);
    }
    attroff(COLOR_PAIR(CL_PAIR_WIN_HEADER) | A_BOLD);
    
    /* 5. Trennlinie unter dem Titel (Rahmenfarbe) */
    attron(COLOR_PAIR(CL_PAIR_WIN_BORDER));
    mvaddch(y+2, x, ACS_LTEE);
    mvhline(y+2, x+1, ACS_HLINE, w-2);
    mvaddch(y+2, x+w-1, ACS_RTEE);
    attroff(COLOR_PAIR(CL_PAIR_WIN_BORDER));
    
    /* 6. Text Inhalt (WEISS auf SCHWARZ) */
    attron(COLOR_PAIR(CL_PAIR_WIN_BODY) | A_BOLD);
    int start_txt = y + 4;
    for(int i=0; i<line_count; i++) {
        int tx = x + 1 + ((w-2)/2) - (strlen(lines[i])/2); /* Zentriert */
        mvprintw(start_txt+i, tx, "%s", lines[i]);
    }
    attroff(COLOR_PAIR(CL_PAIR_WIN_BODY) | A_BOLD);
    
    refresh();
}

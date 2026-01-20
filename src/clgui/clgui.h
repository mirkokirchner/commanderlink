/* **************************************************************************
 * DATEI: src/clgui/clgui.h
 * MODUL: CommanderLink GUI Framework (API)
 * ZWECK: Bereitstellung von grafischen Primitiven für den Monitor.
 * LOGIK: Der Monitor ruft diese Funktionen auf, um Daten zu visualisieren.
 * **************************************************************************/

#ifndef CLGUI_H
#define CLGUI_H

#include <stdbool.h>

/* --- 1. SYSTEM KONTROLLE --- */

/* Initialisiert das Grafiksystem (Raylib, OpenGL, Auflösung) */
void clgui_init(void);

/* Beendet das System und gibt Speicher frei */
void clgui_shutdown(void);

/* Prüft, ob der User das Fenster schließen will (ALT+F4 / ESC) */
bool clgui_should_close(void);

/* --- 2. FRAME KONTROLLE (Der Rahmen) --- */

/* Beginnt einen neuen Zeichen-Frame (Muss am Anfang der Schleife stehen) */
void clgui_begin_frame(void);

/* Beendet den Frame und schickt die Pixel an den Monitor (Muss am Ende stehen) */
void clgui_end_frame(void);

/* --- 3. INPUT SENSORIK --- */

/* Gibt zurück, ob ein bestimmter Bereich geklickt wurde */
/* x, y, w, h: Definition des klickbaren Bereichs */
bool clgui_is_clicked(int x, int y, int w, int h);


/* --- 4. ZEICHEN-BEFEHLE (Die Werkzeuge) --- */

/* Malt den Standard-Hintergrund (Blau + Gitter) */
void clgui_draw_background(void);

/* Malt die Taskleiste am unteren Rand inkl. Start-Button */
/* start_active: true, wenn das Menü gerade offen sein soll */
void clgui_draw_taskbar(bool start_active);

/* Malt ein einfaches Fenster (Container für Tab-Inhalte) */
/* x, y, w, h: Position und Größe */
/* title: Überschrift des Fensters */
void clgui_draw_window(int x, int y, int w, int h, const char *title);

/* Malt einen Text-Wert (Label: Value) */
void clgui_draw_label_value(int x, int y, const char *label, const char *value);

/* Malt einen Fortschrittsbalken (z.B. für CPU Last) */
/* percent: 0.0 bis 1.0 */
void clgui_draw_progressbar(int x, int y, int w, int h, float percent, bool is_critical);

#endif

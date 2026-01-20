/* **************************************************************************
 * DATEI: src/clgui/clgui.c
 * ZWECK: Raylib-Implementierung mit HD-Font und Vektor-Maus.
 * **************************************************************************/
#include <stdio.h>

#include "clgui.h"
#include "raylib.h"
/* Interne Konstanten */
#define COL_BG      (Color){10, 10, 30, 255}
#define COL_WIN_BG  (Color){20, 20, 40, 245}
#define COL_ACCENT  (Color){0, 121, 241, 255}

/* Globale Variable für den Font (nur innerhalb dieses Moduls sichtbar) */
static Font app_font;

void clgui_init(void) {
    /* 1. Flags für HD Performance */
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_VSYNC_HINT);
    
    /* 2. HD Auflösung erzwingen (1920x1080) */
    InitWindow(1920, 1080, "CommanderLink");
    
    /* 3. System-Maus weg, wir machen gleich eine eigene */
    HideCursor();
    SetTargetFPS(60);

    /* 4. SCHRIFTART LADEN (Vector Physics) */
    /* Wir laden sie groß (32px), damit sie auch herunterskaliert scharf bleibt. */
    /* Pfad muss relativ zum Binary sein, oder absolut. */
    app_font = LoadFontEx("assets/fonts/OpenSans-Bold.ttf", 64, 0, 0);
    
    /* Filter setzen für glatte Kanten (Bilinear Filtering) */
    SetTextureFilter(app_font.texture, TEXTURE_FILTER_BILINEAR);
}

void clgui_shutdown(void) {
    UnloadFont(app_font); /* Wichtig: Speicher freigeben */
    CloseWindow();
}

bool clgui_should_close(void) {
    return WindowShouldClose();
}

/* HELFER: Zeichnet Text mit unserer Custom Font */
/* size: Schriftgröße in Pixeln */
static void draw_text_hd(const char *text, int x, int y, int size, Color color) {
    Vector2 pos = { (float)x, (float)y };
    /* spacing = 2.0f für bessere Lesbarkeit */
    DrawTextEx(app_font, text, pos, (float)size, 2.0f, color);
}

void clgui_begin_frame(void) {
    BeginDrawing();
    ClearBackground(COL_BG);
}

void clgui_end_frame(void) {
    /* MAUS RENDERING (Targeting System Style) */
    Vector2 m = GetMousePosition();
    
    /* Wir nutzen DrawLineEx für Dicke (3.0f = 3 Pixel breit) */
    /* Grün, damit es knallt */
    float thick = 3.0f;
    float len = 15.0f;
    Color cursor_col = GREEN;

    /* Fadenkreuz */
    DrawLineEx((Vector2){m.x - len, m.y}, (Vector2){m.x + len, m.y}, thick, cursor_col);
    DrawLineEx((Vector2){m.x, m.y - len}, (Vector2){m.x, m.y + len}, thick, cursor_col);
    
    /* Leerer Kreis in der Mitte für Präzision */
    DrawRing(m, 4.0f, 6.0f, 0, 360, 0, cursor_col);

    EndDrawing();
}

bool clgui_is_clicked(int x, int y, int w, int h) {
    Vector2 m = GetMousePosition();
    Rectangle r = { (float)x, (float)y, (float)w, (float)h };
    return CheckCollisionPointRec(m, r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void clgui_draw_background(void) {
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    /* Gitter etwas feiner zeichnen */
    for(int i=0; i<h; i+=100) DrawLine(0, i, w, i, Fade(WHITE, 0.05f));
    for(int i=0; i<w; i+=100) DrawLine(i, 0, i, h, Fade(WHITE, 0.05f));
}

void clgui_draw_taskbar(bool start_active) {
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    int bar_h = 60;
    
    /* Leiste */
    DrawRectangle(0, h-bar_h, w, bar_h, (Color){15,15,15,255});
    DrawRectangleLines(0, h-bar_h, w, bar_h, GRAY); // Rahmen oben

    /* Start Button */
    Rectangle btn = { 10, h-bar_h+10, 140, 40 }; // Etwas breiter für OpenSans
    Color c = start_active ? MAROON : COL_ACCENT;
    
    DrawRectangleRec(btn, c);
    DrawRectangleLinesEx(btn, 2, WHITE); // 2px Rahmen
    
    /* Text mit HD Font */
    draw_text_hd("SYSTEM", btn.x + 20, btn.y + 8, 24, WHITE);
    
    /* Uhrzeit */
    draw_text_hd("20:42", w - 100, h - 45, 24, LIGHTGRAY);
}

void clgui_draw_window(int x, int y, int w, int h, const char *title) {
    /* Schatten für Tiefe */
    DrawRectangle(x+4, y+4, w, h, Fade(BLACK, 0.5f));

    /* Körper */
    DrawRectangle(x, y, w, h, COL_WIN_BG);
    DrawRectangleLinesEx((Rectangle){x,y,w,h}, 2, COL_ACCENT); // Dickerer Rahmen
    
    /* Header Balken */
    DrawRectangle(x, y, w, 40, COL_ACCENT);
    
    /* Titel HD */
    draw_text_hd(title, x + 10, y + 8, 24, WHITE);
}

void clgui_draw_label_value(int x, int y, const char *label, const char *value) {
    /* Label Grau, kleiner (20px) */
    draw_text_hd(label, x, y, 20, LIGHTGRAY);
    
    /* Wert Weiß, Fett (da Font Bold ist), selbe Größe */
    /* Offset X erhöhen, da OpenSans breiter läuft als Default Font */
    draw_text_hd(value, x + 200, y, 20, WHITE);
}

void clgui_draw_progressbar(int x, int y, int w, int h, float percent, bool is_critical) {
    /* Hintergrund Track */
    DrawRectangle(x, y, w, h, (Color){50,50,50,255});
    
    /* Füllung */
    int fill = (int)(w * percent);
    Color c = is_critical ? RED : GREEN;
    
    /* Glow Effekt (Doppelt zeichnen) */
    DrawRectangle(x, y, fill, h, c);
    DrawRectangle(x, y, fill, h/2, Fade(WHITE, 0.3f)); // Glanz oben drauf

    /* Rahmen */
    DrawRectangleLines(x, y, w, h, LIGHTGRAY);
    
    /* Prozent Text in den Balken rein */
    char buf[16];
    snprintf(buf, 16, "%d%%", (int)(percent*100));
    draw_text_hd(buf, x + w + 10, y-2, 20, WHITE);
}

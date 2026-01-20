# LAW_00_ZERO.md
STATUS: CANONICAL · FROZEN · SSOT  
VERSION: 1.0  
APPLIES TO: CommanderLink (Genesis, clinit, Runtime, Monitor, Tooling)

---

## 0. Definition

ZERO bezeichnet den **vollständig neutralen Anfangszustand** eines CommanderLink-Systems.

ZERO ist kein Fehlerzustand.  
ZERO ist **die einzig erlaubte Ausgangswahrheit**.

Alles, was vor ZERO liegt, ist außerhalb des Systems.  
Alles, was nach ZERO kommt, muss aus ZERO ableitbar sein.

---

## 1. ZERO ist beschreibend, niemals entscheidend

Im ZERO-Zustand gilt:

- keine Optimierung
- keine Heuristik
- keine Ableitung
- keine Interpretation
- keine Policy
- kein Fallback

ZERO beschreibt **nur**, was existiert – nicht, was daraus zu tun ist.

---

## 2. Implizite Defaults sind verboten

Im ZERO-Zustand sind verboten:

- angenommene Werte
- implizite Aktivierungen
- automatisch gesetzte Flags
- „sinnvolle“ Startparameter

Wenn ein Wert nicht messbar oder nicht bekannt ist, muss er explizit als:

- UNINITIALIZED
- ABSENT
- RECLAIMED
- UNSUPPORTED

markiert werden.

---

## 3. Genesis (clinit) unterliegt ZERO

Die Genesis-Phase (z. B. `clinit`) darf ausschließlich:

- Speicher anlegen
- Layout definieren
- Strukturen auf **0** setzen
- TOC und Root beschreiben

Genesis darf **nicht**:
- Validität setzen
- Gates öffnen
- Epochs > 0 veröffentlichen
- Identitäten bewerten
- Redirects erlauben

---

## 4. Epoch-Semantik im ZERO-Zustand

ZERO definiert die Epoch-Regel:

- `epoch == 0` → UNINITIALIZED
- `epoch > 0`  → VALID (nach Cut-Point)

ZERO selbst darf **niemals** eine Epoch erhöhen.

Epochs werden ausschließlich von autoritativen Modulen
(z. B. CORE) nach erfolgreicher Validierung gesetzt.

---

## 5. Presence ist Teil von ZERO

Abwesenheit ist ein **expliziter Zustand**.

Mögliche Presence-Zustände:

- PRESENT
- RECLAIMED
- PARTIAL

ZERO verlangt:
- Kein Zugriff auf RECLAIMED
- Kein Interpretieren von PARTIAL
- Keine automatische Rekonstruktion

---

## 6. ZERO kennt keine Performance

Performance existiert **nicht** im ZERO-Zustand.

Alle Aussagen über:
- Geschwindigkeit
- Durchsatz
- Latenz
- Skalierung

sind außerhalb von ZERO.

ZERO liefert nur die **Voraussetzungen**, niemals die Wirkung.

---

## 7. ZERO ist global und unumkehrbar

ZERO gilt:

- für alle Module
- für alle Plattformen
- für alle Builds
- für alle Stages

ZERO kann nicht „teilweise“ gelten  
und nicht durch spätere Laws überschrieben werden.

Alle weiteren Laws müssen **mit ZERO konsistent sein**.

---

## 8. ZERO und Fehler

ZERO ist kein Fehler.

Ein Fehler liegt erst dann vor, wenn:
- ZERO verlassen werden sollte
- aber die Voraussetzungen fehlen

ZERO selbst ist immer ein gültiger Zustand.

---

## 9. Ableitungsregel

Jedes weitere LAW oder Contract muss:

1. ZERO respektieren
2. ZERO nicht implizit verlassen
3. ZERO explizit dokumentieren, wenn es verlassen wird

Verletzt ein Dokument diese Regeln,
ist es **nicht SSOT-konform**.

---

## 10. Schlussformel

ZERO ist Stille.  
ZERO ist Wahrheit ohne Annahme.  
ZERO ist der einzige erlaubte Anfang.

Alles andere ist Ableitung.


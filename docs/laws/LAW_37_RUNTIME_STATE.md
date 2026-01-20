# LAW_37_RUNTIME_STATE.md
## CommanderLink – Runtime-Zustands-, Übergangs- & Epoch-Gesetz

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Zweck

Dieses Gesetz definiert **konkrete Laufzeitzustände**,
deren Übergänge und zeitliche Gültigkeit
für CommanderLink.

---

## 1. Runtime-Zustand ist konkret

Runtime-Zustände sind numerisch, messbar und zeitgebunden.

Folgerungen:
- Zustände sind nicht symbolisch.
- Zustände altern.
- Zustände können ungültig werden.

---

## 2. Genau ein aktiver Zustand

Zu jedem Zeitpunkt gilt:
- genau ein aktiver Zustand
- optionale historische Zustände

Mehrfachaktive Zustände sind verboten.

---

## 3. Übergänge sind atomar

Zustandswechsel erfolgen atomar oder klar zweistufig.

Folgerungen:
- Halbe Zustände sind verboten.
- Übergangsdauer ist messbar.
- Rollback ist definiert.

---

## 4. Epoch ist Pflicht

Jeder Zustand ist einer Epoch zugeordnet.

Folgerungen:
- Epoch monoton steigend
- Epoch-Wechsel sichtbar
- Epoch-Kollision ist Fehler

---

## 5. Zustand + Epoch = Wahrheit

Ein Zustand ohne Epoch ist bedeutungslos.

---

## 6. Schlussformel

Runtime-Zustände sind die lebende Wahrheit.
Ohne Epoch gibt es keine Zeit, ohne Zeit keine Wahrheit.


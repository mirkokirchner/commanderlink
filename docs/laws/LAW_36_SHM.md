# LAW_36_SHM.md
## CommanderLink – Shared-Memory-, Segment- & Layout-Gesetz (Runtime/shm)

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Zweck

Dieses Gesetz definiert die **physikalische Shared-Memory-Wahrheit**
von CommanderLink.

SHM ist kein Datencontainer,
sondern ein **direkt abbildbarer Zustand der Maschine**.

---

## 1. SHM ist physikalische Wahrheit

SHM repräsentiert reale Zustände zur Laufzeit.

Folgerungen:
- SHM ist messbasiert.
- SHM ist zeitgebunden.
- SHM ist beobachtbar.

SHM ist **keine Policy** und **kein Manifest**.

---

## 2. SHM ist cacheline-exakt

Für alle SHM-Strukturen gilt:

- Basiseinheit: **64 Byte Cacheline**
- Jede Hot-Einheit = exakt 64B
- Records/Strides = Vielfache von 64B
- Keine Teil-Cachelines

Abweichung ist verboten.

---

## 3. SHM kennt keine `packed`-Layouts

Bestätigung und Anwendung von LAW_26:

- `packed` ist verboten
- Bitfields sind verboten
- Misaligned Access ist verboten

SHM folgt natürlichem Alignment der Hardware.

---

## 4. Layout ist ABI-gebunden

Jedes SHM-Layout:

- ist Teil des ABI
- besitzt harte Größen-, Offset- und Alignment-Beweise
- ist versionsgebunden

Änderungen sind ABI-Brüche.

---

## 5. Wire ≠ SHM

SHM darf **niemals** Wire-Formate abbilden.

Folgerungen:
- Keine serialisierten Header im SHM
- Keine LE/BE-Felder im SHM
- Keine verdichteten Bitformate

Wire wird explizit encodiert/decodiert.

---

## 6. SHM ist segmentiert

SHM besteht aus klar getrennten Segmenten.

Normativ:
- Segmente haben feste Rollen
- Segmente sind unabhängig versionierbar
- Segment-Grenzen sind explizit

Monolithisches SHM ist verboten.

---

## 7. Hot / Warm / Cold Trennung

SHM-Daten sind nach Zugriffshäufigkeit getrennt:

- **Hot**: ständig gelesen (64B)
- **Warm**: regelmäßig gelesen (Vielfache von 64B)
- **Cold**: selten gelesen (Vielfache von 64B)

Vermischung in einer Cacheline ist verboten.

---

## 8. Zugriff ist primär lesend

SHM ist primär ein **Read-Model**.

Folgerungen:
- Leser dominieren
- Schreiber sind begrenzt
- Schreibzugriffe sind kontrolliert

Unkontrolliertes Schreiben ist verboten.

---

## 9. Synchronisation ist explizit

SHM kennt keine implizite Synchronisation.

Folgerungen:
- Atomics sind explizit
- Memory-Order ist explizit
- Konsistenzmodelle sind dokumentiert

„Es funktioniert zufällig“ ist unzulässig.

---

## 10. Zeitbezug ist Pflicht

Jede relevante SHM-Einheit enthält:

- Zeitstempel oder Epoch-Bezug
- Gültigkeitskontext

Zeitlose SHM-Wahrheit ist verboten.

---

## 11. Reclaim ist Teil des Designs

SHM kann teilweise freigegeben werden.

Folgerungen:
- Reclaim-Status ist sichtbar
- Abwesenheit von Segmenten ist ein Zustand
- Zugriff auf reclaimte Segmente ist definiert

Leckender SHM ist verboten.

---

## 12. SHM ist passiv beobachtbar

Externe Leser (Monitor, Tools):

- lesen passiv
- verändern nichts
- verursachen keinen Lock

Beobachtung darf das System nicht stören.

---

## 13. Fehler sind SHM-Zustände

Fehler werden **nicht versteckt**.

Folgerungen:
- Fehlerflags sind explizit
- Inkonsistenzen sind sichtbar
- „Heilung durch Überschreiben“ ist verboten

---

## 14. Validitätskriterium

Ein SHM-Layout ist gültig, wenn:

- jede Einheit cacheline-exakt ist,
- jedes Feld ABI-belegt ist,
- keine Wire-Semantik enthalten ist,
- jede Änderung versioniert ist,
- und alle Beweise existieren.

---

## 15. Architekturbindung

Dieses SHM-Gesetz gilt ausschließlich für:

- x86_64
- Little-Endian
- definierte Cacheline-Größe

Andere Architekturen benötigen eigene SHM-Gesetze.

---

## 16. Schlussformel

SHM ist der Spiegel der Maschine.

Wer SHM wie ein normales Datenmodell behandelt,
verliert Physik, Wahrheit und Determinismus.

Mit diesem Gesetz ist `runtime/shm/`
vollständig normiert.

**Nächster erlaubter Schritt:**
`LAW_37_STATE.md` – konkrete Runtime-Zustände & Übergänge.


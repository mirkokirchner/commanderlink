# LAW_31_TERMS.md
## CommanderLink – Terminologie-, Begriffs- & Bedeutungs-Gesetz (Manifest/terms)

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Zweck

Dieses Gesetz fixiert die **verbindliche Terminologie**
für CommanderLink und definiert,
wie Begriffe im Manifest (`manifest/terms/`) zu entstehen haben.

Ziel: **Single Meaning**.
Kein Begriff darf mehr als eine Bedeutung tragen.
Keine Bedeutung darf mehr als einen Begriff haben.

---

## 1. Begriffe sind Verträge

Ein Begriff ist ein normativer Vertrag.

Folgerungen:
- Begriffe sind präzise.
- Begriffe sind stabil.
- Begriffe sind nicht erklärend, sondern definierend.

Ein unklarer Begriff ist ungültig.

---

## 2. Ein Begriff – eine Bedeutung

Für jeden Begriff gilt:

- exakt **eine** Bedeutung
- keine Synonyme
- keine Mehrfachverwendung in unterschiedlichen Kontexten

Wenn zwei Bedeutungen existieren, müssen zwei Begriffe existieren.

---

## 3. Keine implizite Bedeutung

Begriffe dürfen keine impliziten Annahmen tragen.

Folgerungen:
- Keine Kontextabhängigkeit.
- Keine Abkürzungen mit Deutungsspielraum.
- Keine „offensichtlichen“ Bedeutungen.

Alles ist explizit.

---

## 4. Ebenentrennung der Begriffe

Begriffe sind ihrer Ebene zugeordnet:

- **LAW-Begriffe**: normativ, abstrakt, zeitlos
- **Manifest-Begriffe**: symbolisch, compile-time
- **Runtime-Begriffe**: konkret, messbar, zeitgebunden

Vermischung ist verboten.

---

## 5. Begriffe kennen keine Größen

Im `terms/`-Manifest gilt:

- Keine numerischen Werte mit physikalischer Bedeutung
- Keine Bytes, Hertz, Sekunden
- Keine Cacheline-Größen

Begriffe sind **qualitativ**, nicht quantitativ.

---

## 6. Rollen sind statisch

Rollen beschreiben **was etwas ist**, nicht **was es tut**.

Folgerungen:
- Rollen sind unveränderlich während der Existenz.
- Wechsel der Rolle ist Zustandswechsel.
- Implizite Rollen sind verboten.

---

## 7. Zustände sind exklusiv

Zustände sind **mutual exclusive**.

Folgerungen:
- Ein Objekt befindet sich zu jedem Zeitpunkt in genau einem Zustand.
- Übergänge sind explizit.
- Mehrfachzustände sind verboten.

---

## 8. Domänen sind trennscharf

Domänen trennen Verantwortungsbereiche.

Folgerungen:
- Jede Domäne hat klare Grenze.
- Überschneidung ist verboten.
- Interaktion erfolgt nur über definierte Schnittstellen.

---

## 9. Fehler sind Begriffe, keine Codes

Fehlerbegriffe beschreiben Zustände, keine Reaktionen.

Folgerungen:
- Fehlercodes sind Runtime-Ableitung.
- Begriffe beschreiben Kategorie, nicht Handling.
- Vermischung ist verboten.

---

## 10. Kommentierung ist normativ

Kommentare in `terms/` sind **normativ**.

Folgerungen:
- Kommentare erklären Bedeutung, nicht Nutzung.
- Beispiele sind verboten.
- Implementierungsbezug ist verboten.

---

## 11. Naming-Regeln für `terms/`

Dateien in `manifest/terms/`:

- Präfix `clm_terms_`
- Inhalt: Enums, `typedef enum`, symbolische Konstanten
- Keine Structs
- Keine Layouts
- Keine Includes außer zwingend `<stdint.h>`

---

## 12. Erweiterungsregel

Neue Begriffe dürfen nur hinzugefügt werden, wenn:

- sie keine bestehende Bedeutung duplizieren,
- sie eine Lücke schließen,
- und ihre Ebene eindeutig ist.

Begriffsentfernung ist Revision, kein Patch.

---

## 13. Validitätskriterium

Ein `terms`-Artefakt ist gültig, wenn:

- jeder Begriff eindeutig definiert ist,
- keine numerische Physik enthalten ist,
- keine Runtime-Annahme existiert,
- und keine Überschneidung mit anderen Begriffen besteht.

---

## 14. Schlussformel

Begriffe sind die Atome der Architektur.

Wer Begriffe verwässert,
zerstört Determinismus, Ableitbarkeit und Wahrheit.

Mit diesem Gesetz ist der **Inhalt** von
`manifest/terms/` vollständig normiert.
Der nächste Schritt ist die **konkrete Erstellung**
der `terms`-Dateien.


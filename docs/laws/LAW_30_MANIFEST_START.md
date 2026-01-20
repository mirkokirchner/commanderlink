# LAW_30_MANIFEST_START.md
## CommanderLink – Manifest-Start-, Umfangs- & Reinheits-Gesetz

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Zweck

Dieses Gesetz definiert **exakt**, wie der Einstieg in die
Manifest-Erstellung erfolgt, **was erlaubt ist**,
**was verboten ist** und **wann ein Manifest als korrekt gilt**.

Es ist das Scharnier zwischen Gesetz und Technik.

---

## 1. Manifest ist Gesetzesableitung

Ein Manifest ist keine Spezifikation,
sondern eine **mechanische Ableitung** der Gesetze.

Folgerungen:
- Manifest verhandelt nichts.
- Manifest erklärt nichts.
- Manifest konkretisiert ausschließlich.

---

## 2. Manifest ist compile-time Wahrheit

Manifest-Inhalte gelten zur Compile-Zeit.

Folgerungen:
- Keine Messwerte.
- Keine Laufzeit-Zustände.
- Keine dynamischen Größen.
- Keine Heuristiken.

Wenn ein Wert erst zur Laufzeit feststeht,
ist er im Manifest verboten.

---

## 3. Erlaubter Inhalt (positiv definiert)

Manifest darf **ausschließlich** enthalten:

- Begriffe (Terms)
- Rollen
- Zustände (symbolisch, nicht numerisch)
- Enums
- Klassen von Größen (z. B. SMALL/MEDIUM/LARGE)
- Symbolische Konstanten ohne physikalische Bedeutung
- Compile-time Flags mit normativer Semantik

Alles andere ist verboten.

---

## 4. Verbotener Inhalt (explizit)

Manifest darf **niemals** enthalten:

- Struct-Layouts
- Größen in Bytes
- Cacheline-Zahlen
- Alignment-Werte
- SHM-Segmente
- Offsets
- Runtime-Counter
- Performance-Werte
- „Optimale“ Parameter

Verstoß ist ein Gesetzesbruch.

---

## 5. Manifest kennt keine Physik-Zahlen

Physik wird **qualitativ**, nicht quantitativ abgebildet.

Beispiel:
- erlaubt: `CACHELINE`
- verboten: `CACHELINE_64B`

Die Quantisierung erfolgt **erst in Runtime/ABI**.

---

## 6. Manifest-Paket 0 (verbindlich)

Der erste Manifest-Schritt ist **festgelegt**:

```

manifest/
└── terms/
├── clm_terms_roles.h
├── clm_terms_states.h
├── clm_terms_domains.h
└── clm_terms_errors.h

```

Diese Dateien enthalten **nur**:
- Enums
- symbolische Namen
- Kommentare mit normativer Bedeutung

Keine Includes außer `<stdint.h>` wenn zwingend nötig.

---

## 7. Reihenfolge ist bindend

Manifest wird in genau dieser Reihenfolge aufgebaut:

1. terms/
2. base/
3. policy/

Ein späterer Schritt darf frühere nicht erweitern oder korrigieren.

---

## 8. Manifest-Freeze

Jedes Manifest-Paket wird eingefroren.

Folgerungen:
- Änderungen erfordern neue Version.
- Kein „Nachziehen“ für Runtime.
- Runtime passt sich Manifest an, nie umgekehrt.

---

## 9. Validitätskriterium

Ein Manifest ist gültig, wenn:

- es vollständig aus Gesetzen ableitbar ist,
- keine Laufzeitannahmen enthält,
- keine impliziten physikalischen Zahlen nutzt,
- und vollständig deterministisch ist.

---

## 10. Schlussformel

Manifest ist der erste Ort,
an dem Gesetze technisch greifbar werden.

Wer im Manifest Physik „vorwegnimmt“,
zerstört die Trennung von Recht und Realität.

Mit diesem Gesetz ist der Startpunkt definiert.

**Nächster erlaubter Schritt:**
`manifest/terms/clm_terms_roles.h`


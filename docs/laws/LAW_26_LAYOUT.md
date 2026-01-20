# LAW_26_LAYOUT.md
## CommanderLink – Layout-, Padding- & „packed“-Verbot-Gesetz (SHM/ABI)

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Zweck

Dieses Gesetz regelt verbindlich:

- Struktur-Layout (SHM/ABI)
- Padding/Alignment-Determinismus
- Umgang mit Compiler-Freiheiten
- Verbot von `packed` und verwandten Mechanismen

Ziel: physikalisch korrektes, deterministisches, cacheline-exaktes Layout
ohne Undefined Behavior und ohne implizite Drift.

---

## 1. `packed` ist verboten

Für alle CommanderLink Datenstrukturen gilt:

- `__attribute__((packed))` ist verboten.
- `#pragma pack(...)` ist verboten.
- „packed member access“ in SHM ist verboten.
- Jegliche Form der erzwungenen Misalignment-Serialisierung im Speicherlayout ist verboten.

Begründungen sind irrelevant.
Verstoß ist ein Systemfehler.

---

## 2. Keine impliziten Padding-Annahmen

Es ist verboten anzunehmen, dass:

- der Compiler „kein Padding“ einfügt,
- Padding „schon passt“,
- oder Feldreihenfolgen „egal“ sind.

Feldreihenfolge ist physikalisch wirksam und ist deterministisch zu wählen.

---

## 3. Layout-Determinismus ist Pflicht

Jede SHM-/ABI-relevante Struktur muss:

- Größe deterministisch festlegen,
- Offsets deterministisch festlegen,
- und Alignment-Anforderungen explizit erfüllen.

Für jede Struktur gilt:
- `_Static_assert(sizeof(T) == N)`
- `_Static_assert(_Alignof(T) == A)` sofern relevant
- `_Static_assert(offsetof(T, field) == K)` für kritische Felder

Ohne diese Beweise ist die Struktur ungültig.

---

## 4. Cacheline-Block-Primat (64B)

Für SHM/Runtime gilt:

- 64B ist die atomare Layout-Einheit.
- Hot/Warm/Cold-Einheiten sind 64B-Blöcke.
- Records/Strides sind Vielfache von 64B.

Teil-Cachelines als „Record“ sind verboten.

---

## 5. Zugriffssicherheit schlägt Bitdichte

Bitdichte ist nachrangig gegenüber:

- Alignment-Korrektheit
- Load/Store-Sicherheit
- deterministischem Verhalten

Daher gilt:
- Keine bitfield-basierten SHM-Layouts.
- Keine misaligned Multi-Byte-Felder.
- Keine „Wire-Layout im SHM“.

---

## 6. Wire und SHM sind strikt getrennt

Es ist verboten, Wire-Formate (LE/packed/bitted)
direkt in SHM-Strukturen abzubilden.

Folgerungen:
- Wire ist serialisiert, SHM ist nativ.
- Wire-Parsing/Encoding ist ein expliziter Schritt.
- Endianness-Konversion ist explizit und lokal.

---

## 7. Erlaubte Mittel zur Layout-Fixierung

Erlaubt sind ausschließlich:

- natürliche Feldreihenfolge nach Alignment (u64 → u32 → u16 → u8)
- explizite End-Padding-Felder (z. B. `uint8_t pad[...]`)
- 64B-Normflächen (z. B. `uint8_t raw[64]`) als Blockrahmen
- `_Static_assert`-Beweise

Nicht erlaubt:
- `aligned(64)` auf Substrukturen als Layout-Trick,
  wenn es implizite Padding-Drift erzeugt.
- 0-Byte Pads.
- compilerabhängige Extensions als Ersatz für Beweise.

---

## 8. Alias- und UB-Regeln

Für SHM-Daten gilt:

- Strict-Aliasing-Tricks sind verboten.
- Reinterpret-Casts zur Feldsicht sind verboten, wenn UB möglich ist.
- Notwendige Typumdeutung erfolgt über sichere Mechanik (z. B. Kopiersemantik).

Ein UB-Risiko macht den Datensatz ungültig.

---

## 9. Prüfpflicht

Jede Build-Konfiguration muss Layoutfehler als Hard-Fail behandeln:

- Warnings über „packed member access“ sind Hard-Fail.
- Size/Align/Offset-Drift ist Hard-Fail.
- Unterschiedliche Compiler-Ergebnisse sind Hard-Fail.

Ein Build, der solche Fehler toleriert, ist nicht CommanderLink.

---

## 10. Schlussformel

`packed` ist in CommanderLink nicht nur unerwünscht,
sondern physikalisch unzulässig.

Layout wird nicht „gemacht“,
Layout wird **bewiesen**.

Dieses Gesetz ist Grundlage aller
SHM-, Runtime- und ABI-Verträge.


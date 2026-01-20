```markdown
# LAW_25_CONFORMANCE.md
## CommanderLink – Konformitäts- & Cacheline-Gesetz (Physics-First Proof)

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Zweck

Dieses Gesetz definiert, was „konform“ bedeutet, und setzt die
nicht verhandelbaren Prüfkriterien für:

- Physics-first
- x86_64-first
- Cacheline-Exaktheit
- ABI-/Layout-Determinismus

Es beantwortet die Frage „Sind wir fertig?“ ausschließlich über
prüfbare Kriterien.

---

## 1. Law-Bedarf

Es besteht **kein weiterer Pflicht-LAW-Bedarf**, wenn und nur wenn
die in diesem Gesetz festgelegten Konformitätskriterien erfüllt sind.

Wenn ein Kriterium nicht erfüllt ist, besteht automatisch Law-Bedarf:
entweder Revision bestehender Gesetze oder neues Gesetz für die Lücke.

---

## 2. Definition „Physics-first konform“

Ein Artefakt ist Physics-first konform, wenn:

- es keine Annahme über ideale Ressourcen trifft,
- es Worst-Case dominiert,
- es messbasiert steuert,
- es Interlocks respektiert,
- und es reversible Degradation/Rückzug besitzt.

Fehlt eine dieser Eigenschaften, ist es nicht konform.

---

## 3. Definition „Cacheline-exakt“

Cacheline-exakt bedeutet:

- Jede **Hot-/kritische** Einheit ist exakt **64 Byte** groß.
- Jede zusammengesetzte Einheit ist ein **ganzzahliges Vielfaches** von 64 Byte.
- Keine impliziten Pads beeinflussen die Größe.
- Kein Feldlayout ist compiler-abhängig.

„Ungefähr 64B“ ist verboten.

---

## 4. Alignment-Anforderungen (x86_64)

Für CommanderLink gilt normativ:

- **64B** ist die Basiseinheit für Hot-State.
- Alignment auf **64B** ist für Hot-Blöcke Pflicht.
- Strides/Records sind Vielfache von 64B.
- Segment-/Page-Boundaries sind explizit.

Abweichung ist verboten.

---

## 5. SHM-/ABI-Layout-Anforderungen

Für alle SHM-/Runtime-Strukturen gilt:

- Keine `packed`-Layouts.
- Keine Annahmen über fehlendes Compiler-Padding.
- Feldreihenfolge folgt Alignment-Realität.
- End-Padding ist explizit und berechnet, nicht geraten.
- Jede Größe wird durch `_Static_assert` erzwungen.

Wenn ein Struct ohne harte Größenasserts existiert, ist es nicht konform.

---

## 6. „x64 ausgerichtet“ bedeutet 64B, nicht „x64“

Der Begriff „x64 ausgerichtet“ ist in CommanderLink verbindlich als
**64-Byte cacheline aligned** zu verstehen.

Jede andere Interpretation ist verboten.

---

## 7. Konformitätsnachweis ist Pflicht

Konformität ist nur gültig, wenn sie bewiesen ist durch:

- Compile-time Layout-Beweise (Größe/Offset/Alignment)
- Laufzeit-Sanity Checks (optional, aber erlaubt)
- ABI-Report als single source of truth

Ohne Nachweis gilt „nicht konform“.

---

## 8. Aktueller Stand (normative Antwort)

Aus den Gesetzen allein folgt:

- Die Forderung nach Physics-first ist vollständig abgedeckt.
- Die Forderung nach Cacheline-Exaktheit ist vollständig abgedeckt.
- Ob „alles exakt an 64B Cachelines ausgerichtet“ ist,
  kann **nicht** durch Gesetze behauptet werden, sondern nur durch Nachweis.

Daher gilt aktuell normativ:

- **Gesetzlich konformierbar:** JA.
- **Faktisch bewiesen konform:** NUR, wenn ABI-/Layout-Beweise vorliegen.

---

## 9. Startkriterium für Manifest-Header unter Cacheline-Anspruch

Manifest-Header dürfen nur begonnen werden, wenn:

- die Cacheline-Regeln dieses Gesetzes als oberste ABI-Norm akzeptiert sind,
- und jede geplante Manifest-/Runtime-Struktur
  eine verpflichtende Static-Assert-Strategie besitzt.

---

## 10. Schlussformel

LAW-Bedarf: NEIN, sofern diese Konformitätsdefinition akzeptiert ist.

Physics-first: JA, normativ vollständig.

Cacheline-Exaktheit: als Pflicht definiert,
aber als Tatsache erst bewiesen, wenn der Layout-Nachweis existiert.

Damit ist der nächste zwingende Schritt:
Konformitätsnachweis als Ableitungsartefakt (ABI-Report/Static-Asserts),
und dann Manifest-Header.
```


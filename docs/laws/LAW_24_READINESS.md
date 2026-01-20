# LAW_24_READINESS.md
## CommanderLink – Startreife-, Freeze- & Ableitungs-Gesetz (Manifest/Runtime)

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Zweck

Dieses Gesetz definiert:

1. Welche zusätzlichen Pflichten vor Beginn der Header-Ableitung gelten.
2. Wann die Manifest-Header begonnen werden dürfen.
3. Den verbindlichen Arbeitsablauf von Gesetz → Vertrag → Header → Code.

---

## 1. Nichts Relevantes fehlt, wenn es geregelt ist

CommanderLink gilt als „bereit zur Ableitung“ nur, wenn:

- alle systemkritischen Domänen gesetzlich geregelt sind,
- keine impliziten Annahmen verbleiben,
- und keine Kernbegriffe unbestimmt sind.

Nicht geregeltes Verhalten ist verboten.

---

## 2. Pflichtliste „vor Header“

Vor Beginn der **Manifest-Header** müssen genau diese Punkte als **normative Texte** vorliegen:

1. **Terminologie-Fixierung** (Begriffe, Zustände, Rollen; Single Meaning)
2. **Freeze-Regel** (was ist „freigegeben“, was ist „in Arbeit“)
3. **Policy-vs-Runtime Trennung** (Manifest definiert compile-time Gesetze; Runtime beschreibt Laufzeit-Wahrheit)
4. **Trust/Join Grundsatz** (Default-deny; Übergänge; Entzug)
5. **Budget & Rückzug** (Symbiont-Regelkreis; Prioritäten)
6. **Endianness/Order** (Host-Order intern; Wire-Order explizit)
7. **ABI-Determinismus** (Layout, Größen, Alignment, keine impliziten Pads)

Wenn einer dieser Punkte nicht eindeutig geregelt ist, ist Header-Start verboten.

---

## 3. Header-Startkriterium

Manifest-Header dürfen begonnen werden, wenn:

- LAW_00 bis LAW_24 freigegeben sind,
- keine Widersprüche zwischen Gesetzen existieren,
- und die Pflichtliste „vor Header“ vollständig erfüllt ist.

Ab dann gilt:
- Manifest-Header sind **Ableitung**, nicht Verhandlung.
- Änderungen an Manifest-Headern sind nur über Gesetzesrevision zulässig.

---

## 4. Reihenfolge der Ableitung

Die Ableitung erfolgt strikt in dieser Reihenfolge:

1. Manifest (compile-time Gesetze)
2. Runtime-Verträge (SHM/Wahrheit/Segmente)
3. Common-Mechanik (kleinster gemeinsamer Unterbau)
4. Tools (ABI-Report, Dumper, Validity Gates)
5. Erst danach Data Plane / Link / Flow / Monitor

Abweichung ist verboten.

---

## 5. Arbeitsmodus „Symbiont“

Der Symbiont-Charakter wird vor allem durch diese Ableitungsziele erreicht:

- Budgetierung als primäre Leitplanke
- Rückzug/Degradation als Standardpfad
- Messung als einzige Wahrheit
- Interlocks mit Vorrang
- Reversibilität aller Eingriffe

Jede Header-Entscheidung muss auf diese Ziele rückführbar sein.

---

## 6. Projektzustand (normative Einschätzung)

CommanderLink ist im Kern **richtig** aufgestellt, wenn diese Eigenschaften erfüllt sind:

- Physik-first: Gesetze dominieren jede Implementation.
- Symbiont: Schutz des Wirts ist übergeordnet.
- Determinismus/Worst-Case: keine „best effort“-Lücken.
- Messung/Wahrheit: klare Trennung von Rohdaten und Interpretation.
- Änderungsdisziplin: Freeze/Versionierung/Single Source of Truth.

Wenn eine dieser Eigenschaften praktisch verletzt wird, ist das Projekt nicht „falsch gebaut“, sondern **noch nicht fertig abgeleitet**.

---

## 7. Wann beginnen wir konkret?

Beginn der Manifest-Header ist **jetzt** erlaubt, wenn:

- Du erklärst, dass LAW_00–LAW_24 als Arbeitsstand gelten,
- und wir die Pflichtliste (Abschnitt 2) als bereits erfüllt betrachten.

Ab dem Moment ist der nächste Schritt verbindlich:
**Manifest Paket 0: cl_manifest_base.h + cl_manifest_terms.h (nur Begriffe/Enums/States, keine Runtime-Layouts).**

---

## 8. Schlussformel

Es gibt keine „magische Restliste“ außerhalb der Gesetze.
Was wir beachten müssen, ist Disziplin:

- nichts implizit lassen,
- nichts doppelt definieren,
- nichts ohne Freigabe bauen.

Header beginnen, sobald du den LAW-Stand freigibst.


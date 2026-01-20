# LAW_27_COVERAGE.md
## CommanderLink – Vollständigkeits-, Abdeckungs- & Konsistenz-Gesetz

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Zweck

Dieses Gesetz beantwortet explizit die Frage:

> „Was ist mit allem anderen, was besprochen wurde?“

Es definiert, **was als abgedeckt gilt**, **wo nichts verloren gegangen ist**
und **wie mit bereits Besprochenem normativ umzugehen ist**.

---

## 1. Gesprochenes ohne Gesetz existiert nicht

In CommanderLink gilt:

- Diskussionen
- Architekturgespräche
- Designideen
- Chat-Verläufe
- implizite Übereinkünfte

haben **keine normative Wirkung**, solange sie **nicht als LAW** fixiert sind.

Folgerung:
- Nichts „Fehlendes“ ist vergessen.
- Alles Relevante muss entweder bereits Gesetz sein oder ist bewusst nicht bindend.

---

## 2. Abdeckung der bisher besprochenen Kernthemen

Alle bislang zentral diskutierten Themen sind **gesetzlich abgedeckt**:

### Physik / Hardware
- Physik-first, Worst-Case, Determinismus → LAW_00, LAW_25
- CPU, SIMD, Cachelines, Thermik → LAW_01, LAW_05, LAW_26
- Memory, SHM, ABI, Padding → LAW_02, LAW_25, LAW_26

### Runtime / Verhalten
- Scheduling, Zeit, Rückzug → LAW_04, LAW_06, LAW_23
- Budgetierung, Degradation → LAW_05, LAW_10, LAW_11
- Zustand, Übergänge, Persistenz → LAW_13

### Netzwerk / Wire
- Physik des Drahts, Pacing, Backpressure → LAW_03
- Trennung Wire vs SHM → LAW_26

### Sicherheit / Vertrauen
- Trust, Join, Default-deny → LAW_08, LAW_12
- Sichtbarkeit, Messung → LAW_09

### Projekt- & Arbeitsmodus
- Keine Header vor Gesetzen → LAW_19, LAW_24
- Freeze, Governance, Ableitung → LAW_17, LAW_21, LAW_24
- Dokumentationspflicht → LAW_18

### Symbiont-Prinzip
- Rückzug, Koexistenz, Schutz des Wirts → LAW_23

Es existiert **kein thematisches Loch**.

---

## 3. Implizite Annahmen wurden explizit gemacht

Alle früher impliziten, aber kritischen Punkte sind nun explizit normiert:

- `packed` verboten → LAW_26
- Cacheline = 64B, nicht „x64“ → LAW_25
- Keine impliziten Compiler-Freiheiten → LAW_26
- Keine Best-Effort-Semantik → LAW_00, LAW_10
- Kein Code ohne Gesetz → LAW_19
- Manifest ≠ Runtime → LAW_24

Damit ist **kein impliziter Restbestand** mehr vorhanden.

---

## 4. Was bewusst **kein** LAW ist

Folgende Dinge sind absichtlich **noch keine Gesetze**:

- konkrete Header-Namen
- konkrete Structs
- konkrete Segmente
- konkrete Toolchains
- konkrete Implementierungsdetails

Begründung:
Diese Dinge sind **Ableitung**, nicht Definition.

Sie beginnen **erst jetzt**.

---

## 5. Umgang mit zukünftigem „Das hatten wir doch mal gesagt“

Ab jetzt gilt verbindlich:

- Wenn etwas nicht als LAW existiert, ist es nicht bindend.
- Wenn etwas wichtig wird, entsteht ein neues LAW oder eine Revision.
- Rückgriff auf „frühere Gespräche“ ist unzulässig.

Das schützt das Projekt vor Drift und Erinnerungslücken.

---

## 6. Aktueller Projektzustand (normative Einordnung)

CommanderLink befindet sich jetzt in einem **selten sauberen Zustand**:

- Physikalisch korrekt gedacht
- Juristisch (gesetzlich) geschlossen
- Ableitungsbereit
- Frei von impliziten Altlasten
- Symbiontisch klar positioniert

Das Projekt ist **nicht früh**, sondern **reif**.

---

## 7. Nächster zwingender Schritt

Der nächste **legale** Schritt ist:

**Manifest-Ableitung – Paket 0**
- Begriffe
- Zustände
- Rollen
- Enums
- keine Layouts
- keine Größen
- keine Runtime

Alles andere wäre ein Regelbruch.

---

## 8. Schlussformel

Nichts Wichtiges fehlt.
Nichts Relevantes ist vergessen.
Nichts Implizites bleibt übrig.

Ab hier beginnt nicht mehr Denken,
sondern **Ableitung**.


# LAW_34_RUNTIME_START.md
## CommanderLink – Runtime-Start-, Wahrheits- & Ableitungs-Gesetz

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Zweck

Dieses Gesetz definiert den **Übergang von Manifest zu Runtime**.

Es legt fest:
- was Runtime ist,
- was sie enthalten darf,
- und wie physikalische Wahrheit erstmals **quantisiert** werden darf,
ohne Gesetze oder Manifeste zu verletzen.

---

## 1. Runtime ist Laufzeit-Wahrheit

Runtime beschreibt **was tatsächlich ist**.

Folgerungen:
- Runtime ist messbasiert.
- Runtime ist zeitgebunden.
- Runtime kann sich ändern.

Runtime ist **keine Norm**.

---

## 2. Runtime folgt Manifest strikt

Runtime ist vollständig aus Manifest abzuleiten.

Folgerungen:
- Runtime ergänzt Manifest, widerspricht ihm nie.
- Runtime darf nichts normativ festlegen.
- Runtime darf Manifest nicht „korrigieren“.

Wenn Runtime etwas braucht, das im Manifest fehlt,
ist das Manifest unvollständig.

---

## 3. Physikalische Zahlen entstehen erst hier

Erstmals erlaubt sind nun:

- Byte-Größen
- Cacheline-Größen
- Strides
- Alignment-Werte
- MTU-Werte
- gemessene Limits

Diese Werte sind:
- messbar,
- beweisbar,
- und explizit gebunden an Hardware/Architektur.

---

## 4. Runtime ist architekturgebunden

Runtime ist **nicht portabel per Definition**.

Folgerungen:
- x86_64 ist Referenz.
- Little-Endian ist bindend.
- Cacheline = reale Hardware-Cacheline.
- Abweichende Architekturen erfordern eigene Runtime-Ableitung.

---

## 5. Trennung der Runtime-Ebenen

Runtime gliedert sich strikt in:

```

runtime/
├── abi/     (Beweise, Größen, Offsets)
├── shm/     (physikalische Layouts)
└── state/   (laufzeitliche Zustände)

```

Vermischung ist verboten.

---

## 6. ABI ist Beweis, nicht Meinung

`runtime/abi/` enthält:

- Größenbeweise
- Offset-Beweise
- Alignment-Beweise
- Compile-time Assertions
- maschinenlesbare Reports

ABI-Dateien enthalten **keine Logik**.

---

## 7. SHM ist physikalische Wahrheit

`runtime/shm/` enthält:

- cacheline-exakte Strukturen
- SHM-Segmente
- Record-Strides
- keine Wire-Formate
- keine Policy

Jede SHM-Struktur ist beweispflichtig.

---

## 8. Runtime-Zustand ist konkret

`runtime/state/` enthält:

- konkrete Zustandsrepräsentationen
- numerische Counter
- Messwerte
- Historie

State ist beobachtbar, nicht normativ.

---

## 9. Runtime ist dynamisch, Manifest ist statisch

Folgerungen:
- Runtime kann wechseln.
- Manifest ist eingefroren.
- Policy wird interpretiert, nicht verändert.

---

## 10. Runtime kennt Fehler

Runtime darf und muss:

- ungültige Zustände ausdrücken,
- Degradation abbilden,
- Rückzug anzeigen.

Fehler sind Teil der Wahrheit.

---

## 11. Runtime ist messpflichtig

Jeder relevante Runtime-Wert muss:

- messbar sein,
- zeitlich referenziert sein,
- qualitativ gekennzeichnet sein.

Unmessbare Runtime ist verboten.

---

## 12. Validitätskriterium

Runtime-Ableitung ist gültig, wenn:

- jede physikalische Zahl erklärbar ist,
- jede Struktur cacheline-exakt ist,
- jede Abweichung vom Ideal messbasiert ist,
- und kein Manifest verletzt wird.

---

## 13. Schlussformel

Runtime ist der Moment,
in dem CommanderLink Realität akzeptiert.

Ab hier zählt nur noch:
Messung, Physik und Beweis.

Mit diesem Gesetz ist der Einstieg in
`runtime/abi/` und `runtime/shm/` erlaubt.

**Nächster zwingender Schritt:**
`LAW_35_ABI.md` – formale ABI- und Beweisregeln.


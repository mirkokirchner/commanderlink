# LAW_35_ABI.md
## CommanderLink – ABI-, Größen- & Beweis-Gesetz (Runtime/abi)

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Zweck

Dieses Gesetz definiert das **ABI** als formalen Beweisraum
für alle physikalischen und strukturellen Eigenschaften
der CommanderLink-Runtime.

ABI ist kein Header-Design,
sondern **Nachweis der Wahrheit**.

---

## 1. ABI ist bindend

Das ABI ist der verbindliche Vertrag zwischen:

- Manifest
- Runtime
- Implementierung
- Tools
- Monitor

Folgerungen:
- ABI-Widerspruch ist Systemfehler.
- Implementierung ohne ABI-Nachweis ist ungültig.
- ABI hat Vorrang vor Code.

---

## 2. ABI ist vollständig

Für jedes runtime-relevante Artefakt gilt:

- Größe ist festgelegt
- Alignment ist festgelegt
- Offsets sind festgelegt
- Endianness ist festgelegt

Unvollständiges ABI ist ungültig.

---

## 3. Cacheline ist die Basiseinheit

Für x86_64 gilt normativ:

- Cacheline = reale Hardware-Cacheline
- Hot-/kritische Blöcke = exakt 1 Cacheline
- Records/Strides = Vielfache der Cacheline

Teil-Cachelines sind verboten.

---

## 4. Beweis ist verpflichtend

Jede ABI-Eigenschaft muss bewiesen werden durch:

- `_Static_assert(sizeof(T) == N)`
- `_Static_assert(_Alignof(T) == A)`
- `_Static_assert(offsetof(T, field) == K)` für kritische Felder

Ohne Beweis gilt die Eigenschaft als nicht existent.

---

## 5. Keine impliziten Compiler-Freiheiten

Es ist verboten, sich auf:

- implizites Padding
- implizite Alignment-Anpassung
- „übliche“ Compiler-Layouts

zu verlassen.

ABI muss unabhängig vom Compiler stabil sein.

---

## 6. Feldreihenfolge ist Teil des ABI

Die Reihenfolge der Felder ist normativ.

Folgerungen:
- Reordering ist ABI-Bruch.
- Einfügen von Feldern ist ABI-Bruch.
- Entfernen von Feldern ist ABI-Bruch.

ABI-Brüche erfordern neue Version.

---

## 7. Endianness ist explizit

Für Runtime/ABI gilt:

- Host-Order intern
- Keine implizite Konversion
- Wire-Order ist kein ABI-Thema

Vermischung ist verboten.

---

## 8. Keine `packed`-Layouts

Bestätigung von LAW_26:

- `packed` ist verboten
- Misaligned Access ist verboten
- ABI erzwingt natürliche Alignment-Regeln

Abweichung ist ungültig.

---

## 9. ABI kennt Versionen

Jeder ABI-Stand ist eindeutig:

- ABI-Version ist explizit
- ABI-Version ist monoton
- Mehrere ABI-Stände parallel sind verboten

---

## 10. ABI ist maschinenlesbar

ABI-Artefakte müssen maschinenlesbar sein:

- für Tools
- für Monitor
- für Verifikation

Freitext ersetzt keinen Beweis.

---

## 11. ABI-Report ist Single Source of Truth

Der ABI-Report enthält:

- alle Größen
- alle Alignments
- alle Offsets
- alle Versionen

Code darf diese Informationen **nicht duplizieren**.

---

## 12. ABI ist testbar

ABI-Eigenschaften müssen:

- beim Build geprüft werden
- bei Abweichung den Build abbrechen
- reproduzierbar sein

ABI-Drift ist verboten.

---

## 13. Architekturbindung

Dieses ABI gilt ausschließlich für:

- x86_64
- Little-Endian
- definierte Cacheline-Größe

Andere Architekturen benötigen neues ABI-Gesetz.

---

## 14. Schlussformel

ABI ist Beweis, nicht Behauptung.

Ein System ohne bewiesenes ABI
existiert nicht deterministisch.

Mit diesem Gesetz ist der formale Rahmen
für `runtime/abi/` vollständig.

**Nächster erlaubter Schritt:**
`LAW_36_SHM.md` – physikalische Shared-Memory-Strukturen.


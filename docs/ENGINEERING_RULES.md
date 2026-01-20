# CommanderLink – ENGINEERING RULES

## Zweck
Dieses Dokument definiert die unverrückbaren technischen Grundregeln
für CommanderLink (CL).

CommanderLink ist kein „Framework“, kein „Netzwerktool“ und kein
Standard-Stack.  
CommanderLink ist eine **physiknahe Datenmaschine**.

Diese Regeln gelten **immer** – unabhängig von Plattform, Version oder
Implementierungsdetails.

---

## 1. Primat der Physik

- Hardware ist kein Abstraktionsdetail.
- Cachelines, Register, Busbreiten und Speicherhierarchien sind reale
  physikalische Objekte.
- Software muss sich **der Hardware anpassen**, nicht umgekehrt.

**Konsequenz:**  
„Best Practice“, wenn sie der Physik widerspricht, wird ignoriert.

---

## 2. Zieldefinition

CommanderLink maximiert **gleichzeitig**:

1. Durchsatz
2. Energieeffizienz pro Bit
3. Determinismus
4. Stabilität
5. Hardware-Schutz

Keiner dieser Punkte darf isoliert optimiert werden.

---

## 3. Stabilität vor Geschwindigkeit

- Ein System, das abstürzt, ist wertlos.
- Jeder aggressive Pfad benötigt:
  - Validity Gates
  - Hysterese
  - Sofortige Fallbacks

**Fail-Open Regel:**  
Wenn CL stirbt, bleibt das Betriebssystem funktionsfähig.

---

## 4. Keine impliziten Annahmen

- Keine impliziten Padding-Regeln
- Keine impliziten Alignments
- Keine impliziten Typgrößen
- Keine impliziten Endianness-Annahmen

Alles ist explizit oder verboten.

---

## 5. Messbarkeit

Jede Entscheidung in CL muss messbar sein:
- im Shared Memory
- im Monitor
- in Forensik-Logs

Was nicht sichtbar ist, existiert nicht.

---

## 6. Menschenlesbarkeit

- Code ist auf Deutsch kommentiert.
- Jede Struktur erklärt **warum**, nicht nur **was**.
- Kommentare sind Teil des Vertrags.

---

## 7. Erweiterbarkeit ohne ABI-Bruch

- Strukturen wachsen nur über reservierte Felder.
- Alte Monitore dürfen neue Felder ignorieren können.
- Schema-Versionierung ist Pflicht.

---

## 8. Energie ist erlaubt

- 100 % CPU-Last ist erlaubt.
- AVX-512 Volllast ist erlaubt.
- Hoher Stromverbrauch ist erlaubt.

**Solange:**
- Thermik-Gates offen sind
- VRM-Safety nicht verletzt wird
- OS-Interaktivität nicht leidet

---

## 9. Schlussformel

CommanderLink ist kein Kompromiss.

CommanderLink ist:
> **Investment Protection durch Maximierung**


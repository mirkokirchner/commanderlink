# LAW_00_PHYSICS.md
STATUS: CANONICAL · FROZEN · SSOT  
VERSION: 1.1  
APPLIES TO: CommanderLink (Runtime, HAL, CORE, LINK, FLOW, ORACLE, MONITOR, Tooling)

---

## 0. Präambel

CommanderLink ist ein physikalisch deterministisches System.

Es existiert, um **Wahrheit** aus realer Hardware, Zeit und Topologie zu gewinnen,
zu stabilisieren und kontrolliert verfügbar zu machen.

CommanderLink optimiert **nicht** für Komfort, Kompatibilität oder Konvention,
sondern für:

- Determinismus
- Reproduzierbarkeit
- Worst-Case-Stabilität
- Maschinen-Wahrheit

Alles andere ist nachrangig.

---

## 1. Rangordnung der Wahrheit (absolut bindend)

Bei jedem Konflikt gilt ausschließlich folgende Reihenfolge:

1. Physik  
   (Elektrik, Thermik, Zeit, Bandbreite, Latenz, Energie)
2. Mikroarchitektur  
   (Caches, TLBs, SIMD, DMA, PCIe)
3. ABI / Layout  
   (Alignment, Cachelines, Epochs, Presence)
4. Betriebssystem  
   (Syscalls, Schedulers, Permissions)
5. Industrie-Standards  
   (TCP, POSIX, APIs, RFCs)

**Ein Standard kann physikalisch falsch sein.**  
**Physik ist niemals verhandelbar.**

---

## 2. Determinismus als Grundzustand

CommanderLink kennt keinen impliziten „best effort“-Modus.

Jeder Zustand ist entweder:

- **deterministisch gültig**, oder
- **explizit degradiert**, mit Kennzeichnung:
src = DIRECT | ESTIMATED | UNSUPPORTED
qual = OK | DEGRADED | INVALID
reason = P0..P5


Stille Defaults, implizite Nullen oder UI-Heuristiken sind verboten.

---

## 3. Zeit ist eine physikalische Ressource

Zeit ist kein Hilfswert, sondern eine **erste-Klasse-Wahrheit**.

Pflichten:

- Monotone Zeitbasis (z. B. CLOCK_MONOTONIC)
- Reproduzierbare Messfenster
- Begrenzte (bounded) Schleifen
- Kein Debug-Output, der Zeit zerstört

Wenn Zeit nicht stabil ist → **P0 oder P1**, je nach Modul.

---

## 4. Energie und Thermik sind Systemzustände

Thermik, Power-Limits und Throttling sind **keine Umgebung**, sondern Teil der Wahrheit.

Wenn:

- CPU throttled
- NIC Queues überlaufen
- VRM / Power Caps greifen

→ dann **muss** CL das sehen und berücksichtigen.

Ignorieren gilt als physikalischer Fehler.

---

## 5. Speicher ist Physik

Speicher ist kein abstrakter Pool.

Er ist ein physikalischer Raum mit:

- Cachelines (64 B als atomare Einheit)
- NUMA-Topologie
- TLB-Grenzen
- DMA-Kohärenz

Konsequenzen:

- Alle Runtime-Strukturen sind cacheline-exakt
- Layout ist ABI-fest, nicht „compilerfreundlich“
- Shared Memory ist Wahrheit, kein Transport

---

## 6. Cacheline-Souveränität

Die Cacheline ist die kleinste atomare Wahrheitseinheit.

Regeln:

- Hot / Warm / Cold sind cacheline-basiert
- Kein implizites Padding
- Jeder Writer hat einen klaren Commit-Punkt
- Leser interpretieren nicht, sie **lesen**

---

## 7. Zero-Transformation-Prinzip

CommanderLink transformiert keine Daten heimlich.

Verboten sind:

- implizite Normalisierungen
- UI-Abkürzungen
- stilles Runden
- „das sieht ungefähr so aus“

Wenn etwas geschätzt wird, **steht es dabei**.

---

## 8. Maximierung mit Gates

CommanderLink strebt maximale Performance an,
aber **nur innerhalb physikalischer Schranken**.

Gates können schließen wegen:

- Thermik
- Power
- Budget
- Link-Health
- Identity / Trust
- Safety / Watchdog

Ein geschlossenes Gate erzwingt **deterministischen Rückfall**.
Kein Blackhole, kein Stillstand, kein Chaos.

---

## 9. SIMD ist Baseline

SIMD ist kein Feature, sondern Normalzustand.

- Plattformabhängige Mindestannahmen sind erlaubt
- Höhere SIMD-Stufen sind gated
- Fallback ist sofort und deterministisch

---

## 10. Koexistenz statt Zerstörung

CommanderLink ist ein **Symbiont**, kein Eroberer.

- Management bleibt OS-nativ
- Dataplane darf ersetzt/beschleunigt werden
- Fallback **muss immer funktionieren**
- Redirect darf niemals das OS lahmlegen

---

## 11. Auditierbarkeit

Jede Aussage des Systems muss belegbar sein.

Pflichten:

- Root / TOC / ABI sind prüfbar
- Epoch == 0 → UNINITIALIZED
- Epoch > 0 → VALID
- Presence == RECLAIMED → Abwesenheit
- Fehler sind priorisiert (P0–P5)

Es gibt keine „magischen“ Zustände.

---

## 12. Ableitungsregel für alle weiteren Laws

Jedes weitere LAW oder Contract muss:

1. mit LAW_00_PHYSICS konsistent sein
2. physikalische Rangordnung respektieren
3. Commit-/Epoch-Semantik definieren
4. Doppelwahrheit verhindern

Verletzt ein Dokument dieses Gesetz,
ist es **ungültig**, unabhängig von Version oder Autor.

---

## 13. Schlussformel

Physikalisch falsch bleibt falsch.  
Auch wenn es standardkonform ist.

CommanderLink folgt nicht Standards.  
CommanderLink folgt Wahrheit.



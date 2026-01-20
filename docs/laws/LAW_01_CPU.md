# LAW_01_CPU.md
## CommanderLink – CPU-Gesetz (x86_64)

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Geltungsbereich

Dieses Gesetz regelt die Nutzung, Wahrnehmung und Belastung der CPU
für CommanderLink auf **x86_64**-Systemen.

Es beschreibt keine Implementierungen.
Es definiert verbindliche physikalische und architektonische Regeln.

---

## 1. Die CPU ist eine physikalische Maschine

Die CPU ist kein abstrakter Rechenkern.

* Kerne sind physisch.
* Ausführungseinheiten sind endlich.
* Frontend, Backend und Speicherpfade sind getrennt.

Jede Annahme einer „idealen CPU“ ist verboten.

---

## 2. Kerne sind exklusive Ressourcen

Ein CPU-Kern kann nur einen physikalischen Ausführungskontext
ohne Interferenz garantieren.

Folgerungen:
- Exklusive Nutzung ist der Referenzzustand.
- Geteilte Nutzung ist ein degradierter Zustand.
- Überschneidung erzeugt nichtlineare Effekte.

---

## 3. SMT ist kein zusätzlicher Kern

SMT/Hyper-Threading erzeugt keine neuen Ausführungseinheiten.

Folgerungen:
- SMT-Threads konkurrieren um dieselbe Physik.
- SMT darf niemals als Kapazitätserweiterung gerechnet werden.
- SMT ist optional und jederzeit entziehbar.

---

## 4. Pipelines sind endlich und zustandsbehaftet

Instruktionspipelines haben Tiefe, Latenz und Abhängigkeiten.

Folgerungen:
- Pipeline-Flushes sind reale Kosten.
- Fehlvorhersagen sind physikalische Verluste.
- Kontrollfluss ist teurer als Datenfluss.

---

## 5. SIMD ist ein physikalischer Verstärker

SIMD nutzt vorhandene Breite der Recheneinheiten.

Folgerungen:
- SIMD ist der Normalfall, nicht die Optimierung.
- Skalierung erfolgt über Vektorisierung, nicht über Threads.
- Nicht-vektorisierte Pfade gelten als degradiert.

---

## 6. Frequenz ist variabel und instabil

CPU-Frequenz ist kein konstanter Wert.

* Turbo ist opportunistisch.
* Drosselung ist zwingend.
* Übergänge sind nicht linear.

Folgerungen:
- Zeitabschätzungen dürfen keine fixe Frequenz annehmen.
- Thermal- und Power-Limits haben Vorrang.
- Performance ohne Frequenzkontrolle ist ungültig.

---

## 7. Zeitbasis ist lokal

Zeit entsteht auf der CPU.

Folgerungen:
- Lokale Zeitquellen haben Vorrang vor externen.
- Zeitstempel sind kernabhängig.
- Synchronisation kostet Zeit.

---

## 8. Preemption ist ein physikalischer Eingriff

Unterbrechungen reißen Ausführungskontexte auf.

Folgerungen:
- Preemption ist ein Störereignis.
- Latenzpfade müssen preemption-resistent sein.
- Kritische Pfade sind gegen Verdrängung zu schützen.

---

## 9. Pinning ist ein Vertrag mit der Physik

CPU-Pinning ist keine Optimierung, sondern eine Festlegung.

Folgerungen:
- Gepinnte Ausführung übernimmt Verantwortung für den Kern.
- Pinning entzieht dem System Ressourcen.
- Falsches Pinning ist ein Systemfehler.

---

## 10. Kontextwechsel sind Verluste

Jeder Kontextwechsel verursacht:

* Cache-Verlust
* Pipeline-Verlust
* Zeitverlust

Folgerungen:
- Kontextwechsel sind zu minimieren.
- Hohe Wechselraten sind ein Designfehler.
- „Leichtgewichtige“ Threads existieren physikalisch nicht.

---

## 11. Überbuchung ist verboten

Mehr Arbeit als physikalisch ausführbar zu planen ist unzulässig.

Folgerungen:
- Warteschlangen müssen begrenzt sein.
- Rückstau ist vorzuziehen gegenüber Überlast.
- Dropped Work ist besser als instabiles Systemverhalten.

---

## 12. Messung erfolgt am Kern

CPU-Messungen müssen kernnah sein.

Folgerungen:
- Aggregierte Werte sind sekundär.
- Mittelwerte verschleiern Realität.
- Worst-Case-Beobachtung ist verpflichtend.

---

## 13. Architekturbindung

CommanderLink bindet sich explizit an:

- x86_64
- Little-Endian
- bekannte Cacheline-Größen
- bekannte SIMD-Breiten

Abweichungen sind neue Architekturen und erfordern neue Gesetze.

---

## 14. Schlussformel

Die CPU bestimmt das Verhalten des Systems.
Software hat sich anzupassen, nicht umgekehrt.

Dieses Gesetz ist Grundlage aller CPU-nahen Verträge,
Layouts und Laufzeitentscheidungen.


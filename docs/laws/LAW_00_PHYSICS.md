# LAW_00_PHYSICS.md
## CommanderLink – Physikalisches Grundgesetz

### Status
VERBINDLICH · NORMATIV · NICHT DISKUTABEL

---

## 0. Präambel

CommanderLink ist ein physikalisches System.  
Alle weiteren Gesetze, Verträge, Header und Implementierungen sind diesem Gesetz untergeordnet.

Physik ist die oberste Autorität.  
Spezifikationen, Konventionen, Standards und „Best Practices“ haben keinerlei Vorrang.

---

## 1. Rangordnung der Wahrheit

Die Gültigkeit von Aussagen, Entscheidungen und Messungen folgt strikt dieser Ordnung:

1. Physikalische Realität (Zeit, Energie, Temperatur, elektrische Grenzen)
2. Mikroarchitektur (Caches, Pipelines, TLBs, SIMD, NUMA)
3. Konkrete Hardware-Implementierung (x86_64, reale CPUs, reale NICs)
4. ABI-Verträge (Binärlayout, Alignment, Atomics)
5. Betriebssystem-Verträge
6. Protokolle und formale Spezifikationen

Bei Konflikt gilt immer die höhere Ebene.  
Ein Verstoß gegen diese Ordnung ist ein Designfehler.

---

## 2. Zeit ist endlich

Zeit ist eine physikalische Ressource.

* Jede Operation verbraucht Zeit.
* Zeit kann nicht parallelisiert werden.
* Zeit kann nicht abstrahiert werden.

Folgerungen:
- Worst-Case-Zeit ist maßgeblich, nicht Durchschnittswerte.
- Jitter ist ein Fehler, kein Statistikproblem.
- Ungebundene Wartezeiten sind verboten.

---

## 3. Energie ist endlich

Energie ist begrenzt und messbar.

* CPU-Zyklen sind Energieverbrauch.
* Speicherzugriffe sind Energieverbrauch.
* Netzwerktransfers sind Energieverbrauch.

Folgerungen:
- Jede Berechnung muss eine energetische Rechtfertigung haben.
- Aggressive Nutzung ist erlaubt, ineffiziente Nutzung nicht.
- Thermische Grenzen sind harte Grenzen, keine Hinweise.

---

## 4. Temperatur ist ein Zwang

Temperatur ist ein nicht verhandelbarer Zustand.

* Drosselung ist physikalisch, nicht logisch.
* Thermische Effekte treten verzögert auf.
* Überschreitung führt zu nichtlinearem Verhalten.

Folgerungen:
- Thermische Gates sind sofort wirksam.
- Kein Mechanismus darf Temperatur ignorieren.
- Performance ohne thermische Stabilität ist ungültig.

---

## 5. Speicher ist physikalisch

Speicher ist kein abstrakter Raum.

* Cache-Lines haben feste Größe.
* Alignment ist real.
* Falsches Layout erzeugt reale Kosten.

Folgerungen:
- Implizites Padding ist verboten.
- Layouts müssen cacheline-exakt sein.
- Speicherzugriffe werden nach physikalischer Nähe bewertet.

---

## 6. Parallelität ist begrenzt

Parallelität existiert nur dort, wo Hardware sie zulässt.

* Kerne sind endlich.
* Ausführungseinheiten sind endlich.
* Bandbreite ist endlich.

Folgerungen:
- Oversubscription ist ein Fehler.
- Software darf keine fiktive Parallelität annehmen.
- Scheduling darf keine physikalischen Konflikte erzeugen.

---

## 7. Latenz ist eine Eigenschaft, kein Parameter

Latenz entsteht aus Physik.

* Entfernung
* Zwischenspeicher
* Synchronisation

Folgerungen:
- Latenz kann gemessen, nicht „konfiguriert“ werden.
- Latenzreduktion hat Vorrang vor Durchsatzsteigerung, sofern stabil.
- Ungebundene Latenzpfade sind verboten.

---

## 8. Bandbreite ist ein Fluss

Bandbreite ist kontinuierlich, nicht paketweise.

* Burst-Verhalten erzeugt Störungen.
* Gleichmäßigkeit ist stabiler als Spitzen.

Folgerungen:
- Pacing ist Pflicht.
- Backpressure ist Pflicht.
- Drops sind ein physikalisches Versagen.

---

## 9. Messung ist Teil des Systems

Ein System ohne Messung ist blind.

* Messung darf das System nicht destabilisieren.
* Messung muss zeitlich korrekt sein.
* Messung ist Wahrheit, nicht Interpretation.

Folgerungen:
- Messpfade sind physikalisch zu budgetieren.
- Heuristiken ersetzen keine Messung.
- Logs sind sekundär gegenüber direkter Telemetrie.

---

## 10. Determinismus ist ein physikalisches Ziel

Determinismus bedeutet:
Gleiche Bedingungen → gleiches Verhalten.

Folgerungen:
- Zufälligkeit ist verboten.
- „Best Effort“ ist verboten.
- Nichtdeterministische APIs sind zu kapseln oder auszuschließen.

---

## 11. Fehler sind Zustände, keine Ausnahmen

Fehler entstehen aus Physik.

* Überlast
* Hitze
* Zeitüberschreitung
* Ressourcenmangel

Folgerungen:
- Fehlerpfade sind gleichwertige Pfade.
- Recovery ist Teil des Normalbetriebs.
- Undefined Behavior ist verboten.

---

## 12. Architekturbindung

CommanderLink ist primär x86_64-gebunden.

* Little-Endian ist gegeben.
* Cache-Line-Größe ist bekannt.
* SIMD-Eigenschaften sind explizit.

Portabilität ist nachrangig gegenüber physikalischer Korrektheit.

---

## 13. Schlussformel

Wenn eine Entscheidung physikalisch falsch ist,
ist sie falsch – unabhängig von Eleganz, Konformität oder Konvention.

Dieses Gesetz ist unveränderlich.  
Alle weiteren Gesetze müssen ihm widerspruchsfrei folgen.


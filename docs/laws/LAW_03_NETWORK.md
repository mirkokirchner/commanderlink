# LAW_03_NETWORK.md
## CommanderLink – Netzwerk-Gesetz (Physik des Drahts)

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Geltungsbereich

Dieses Gesetz regelt alle Formen von Datenübertragung
über physische oder virtuelle Netzwerke
für CommanderLink.

Es beschreibt die physikalische Realität des Transports.
Es ignoriert abstrahierende Protokollmodelle.

---

## 1. Das Netzwerk ist ein physikalisches Medium

Ein Netzwerk ist kein logischer Kanal.

* Signale benötigen Zeit.
* Medien haben Bandbreite.
* Übertragung erzeugt Verluste.

Jede Annahme eines „idealen Links“ ist verboten.

---

## 2. Latenz ist durch Physik bestimmt

Netzwerklatenz entsteht aus:

* Ausbreitung
* Serialisierung
* Puffern
* Verarbeitung

Folgerungen:
- Latenz ist nicht konfigurierbar.
- Latenz ist richtungsabhängig.
- Jitter ist ein Fehlerzustand.

---

## 3. Bandbreite ist endlich

Bandbreite ist ein kontinuierlicher Fluss.

Folgerungen:
- Line-Rate ist ein Grenzzustand.
- Überbuchung erzeugt Instabilität.
- Bursts sind physikalisch schädlich.

---

## 4. Puffer sind Zeitverschiebung

Puffer speichern Zeit, nicht Daten.

Folgerungen:
- Große Puffer erhöhen Latenz.
- Unkontrollierte Puffer sind verboten.
- Bufferbloat ist ein Designfehler.

---

## 5. Drops sind physikalische Ereignisse

Paketverluste entstehen durch Überlast oder Störung.

Folgerungen:
- Drops sind Signale, keine Ausnahme.
- Vermeidung hat Vorrang vor Reparatur.
- Wiederholung ersetzt keine Kontrolle.

---

## 6. Backpressure ist Pflicht

Ein Empfänger bestimmt die Geschwindigkeit.

Folgerungen:
- Sender warten.
- Empfänger geben explizit frei.
- Blindes Senden ist verboten.

---

## 7. Pacing ist zwingend

Daten müssen gleichmäßig eingespeist werden.

Folgerungen:
- Zeitbasierte Steuerung ist Pflicht.
- Burst-getriebene Übertragung ist verboten.
- Gleichmäßigkeit schlägt Spitzenleistung.

---

## 8. Reihenfolge ist teuer

Geordnete Übertragung kostet Zeit und Puffer.

Folgerungen:
- Ordnung ist explizit zu fordern.
- Unordnung ist der Default.
- Erzwungene Ordnung ohne Notwendigkeit ist verboten.

---

## 9. Zuverlässigkeit kostet Bandbreite

Integrität und Wiederherstellung sind nicht kostenlos.

Folgerungen:
- Fehlerkorrektur ist adaptiv.
- Statische Sicherheitsmargen sind verboten.
- Physikalische Qualität bestimmt das Maß.

---

## 10. MTU ist eine physikalische Grenze

MTU bestimmt Effizienz und Stabilität.

Folgerungen:
- MTU ist zu messen, nicht anzunehmen.
- Fragmentierung ist verboten.
- Größer ist nur besser bei Stabilität.

---

## 11. Netzwerk-Hardware ist Teil des Systems

NICs, Switches und Treiber sind aktive Akteure.

Folgerungen:
- Offloads sind physikalische Zustände.
- Interrupts sind Störungen.
- DMA ist kein „kostenloser“ Pfad.

---

## 12. Virtuelle Netzwerke sind reale Last

Virtuelle Links verbrauchen reale Ressourcen.

Folgerungen:
- Virtualisierung ändert Physik nicht.
- Overcommit ist sichtbar.
- Abstraktion reduziert nicht die Kosten.

---

## 13. Messung erfolgt am Draht

Netzwerk-Messung muss drahtnah sein.

Folgerungen:
- End-to-End allein ist unzureichend.
- Sender- und Empfängersicht sind getrennt.
- Worst-Case bestimmt das Verhalten.

---

## 14. Architekturbindung

CommanderLink betrachtet Netzwerke primär als:

- Punkt-zu-Punkt-Flüsse
- Zeitkontinuierliche Medien
- Physikalisch begrenzte Systeme

Protokolle sind untergeordnet.

---

## 15. Schlussformel

Das Netzwerk verzeiht keine Annahmen.

Wer schneller sendet als der Empfänger aufnehmen kann,
zerstört Stabilität.

Dieses Gesetz ist Grundlage aller
Wire-, Transport- und Flow-Entscheidungen.


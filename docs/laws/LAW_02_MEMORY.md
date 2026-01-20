# LAW_02_MEMORY.md
## CommanderLink – Speicher-Gesetz (x86_64)

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Geltungsbereich

Dieses Gesetz regelt Struktur, Nutzung und Bewertung von Speicher
für CommanderLink auf **x86_64**-Systemen.

Es definiert physikalische Wahrheiten.
Es enthält keine Implementierungsanweisungen.

---

## 1. Speicher ist physikalisch lokal

Speicher existiert in Stufen realer Entfernung.

* Register
* L1 / L2 / L3 Cache
* DRAM
* Remote-NUMA

Folgerungen:
- Nähe bestimmt Kosten.
- Entfernung ist nicht abstrahierbar.
- Falsche Lokalität ist ein Fehler.

---

## 2. Cache-Lines sind die atomare Einheit

Die Cache-Line ist die kleinste relevante Transporteinheit.

Folgerungen:
- Alle Hot-Daten müssen cacheline-exakt sein.
- Teilweise Nutzung ist Verschwendung.
- False Sharing ist ein Designfehler.

---

## 3. Alignment ist zwingend

Alignment ist keine Optimierung, sondern Voraussetzung.

Folgerungen:
- Implizites Alignment ist verboten.
- Strukturen müssen explizit ausgerichtet sein.
- Fehlalignment erzeugt reale Zusatzlast.

---

## 4. Layout ist ein Vertrag

Speicherlayout ist Teil der ABI-Wahrheit.

Folgerungen:
- Layouts dürfen sich nicht implizit ändern.
- Compiler-Freiheiten sind einzuschränken.
- Padding muss deterministisch sein.

---

## 5. Speicherzugriff ist teurer als Rechenlogik

Ein Speicherzugriff kostet mehr als eine Rechenoperation.

Folgerungen:
- Rechenintensität ist günstiger als Speicherintensität.
- Wiederverwendung hat Vorrang vor Neuladen.
- Datenbewegung ist zu minimieren.

---

## 6. NUMA ist Realität

NUMA ist kein Sonderfall.

Folgerungen:
- Cross-NUMA-Zugriffe sind degradierte Pfade.
- Speicher gehört zu Rechenknoten.
- Migration erzeugt reale Instabilität.

---

## 7. Atomics sind physikalische Barrieren

Atomare Operationen greifen tief in die Hardware ein.

Folgerungen:
- Atomics sind teuer.
- Globale Atomics skalieren nicht.
- Ordnung ist explizit festzulegen.

---

## 8. Konsistenz kostet Zeit

Speicherkonsistenz ist nicht kostenlos.

Folgerungen:
- Strenge Ordnung ist der teuerste Zustand.
- Lockerung ist erlaubt, Unklarheit nicht.
- Memory-Order muss bewusst gewählt werden.

---

## 9. Speicher ist endlich

Speicher ist eine begrenzte Ressource.

Folgerungen:
- Wachstum ohne Obergrenze ist verboten.
- Reclaim ist Pflicht.
- OOM ist ein Designversagen.

---

## 10. Allokation ist ein Eingriff

Allokation verändert den Systemzustand.

Folgerungen:
- Dynamische Allokation im Hot-Pfad ist verboten.
- Vorab-Reservierung ist der Referenzzustand.
- Fragmentierung ist ein Fehler.

---

## 11. Freigabe ist nicht sofort

Freigabe bedeutet nicht sofortige Wiederverwendbarkeit.

Folgerungen:
- Lebenszeiten müssen klar sein.
- Wiederverwendung ohne Stabilität ist verboten.
- Dangling States sind unzulässig.

---

## 12. Sichtbarkeit ist kontrolliert

Nicht jeder Speicher ist für jeden sichtbar.

Folgerungen:
- Shared Memory ist explizit.
- Private Daten bleiben privat.
- Sichtbarkeitsgrenzen sind Verträge.

---

## 13. Architekturbindung

CommanderLink bindet sich an:

- x86_64 Speicherordnung
- Cache-Line-basierte Kohärenz
- Little-Endian Darstellung

Andere Architekturen erfordern neue Gesetze.

---

## 14. Schlussformel

Speicher bestimmt Geschwindigkeit, Stabilität und Wahrheit.

Ein fehlerhaftes Speicherdesign
kann durch keine Logik kompensiert werden.

Dieses Gesetz ist Grundlage aller Layout-,
ABI- und Runtime-Entscheidungen.

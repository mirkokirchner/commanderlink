# LAW_14_INTERFACE.md
## CommanderLink – Schnittstellen- & Grenz-Gesetz

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Geltungsbereich

Dieses Gesetz regelt alle Schnittstellen, Übergänge
und Grenzflächen zwischen Komponenten,
Subsystemen und externen Umgebungen
in CommanderLink.

Eine Schnittstelle ist eine physikalische und logische Grenze,
kein Komfortmechanismus.

---

## 1. Jede Schnittstelle ist eine Grenze

Schnittstellen trennen Verantwortlichkeiten und Zustände.

Folgerungen:
- Grenzen sind explizit zu definieren.
- Implizite Übergänge sind verboten.
- Grenzverletzungen sind Fehler.

---

## 2. Übergänge kosten Zeit und Ressourcen

Jeder Schnittstellenübertritt erzeugt Overhead.

Folgerungen:
- Übergänge sind zu minimieren.
- Ketten von Schnittstellen sind instabil.
- Versteckte Übergänge sind verboten.

---

## 3. Schnittstellen sind Verträge

Eine Schnittstelle definiert erlaubtes Verhalten.

Folgerungen:
- Vertragsverletzungen sind Fehler.
- Kulanzverhalten ist verboten.
- Fehlertoleranz ist explizit zu regeln.

---

## 4. Eingaben sind feindlich

Alle Eingaben gelten als potenziell fehlerhaft.

Folgerungen:
- Validierung ist Pflicht.
- Vertrauen ohne Prüfung ist verboten.
- Grenzprüfung ist nicht optional.

---

## 5. Ausgaben sind verbindlich

Ausgaben haben reale Wirkung.

Folgerungen:
- Falsche Ausgaben sind Fehler.
- Unvollständige Ausgaben sind unzulässig.
- Mehrdeutige Ausgaben sind verboten.

---

## 6. Richtung ist relevant

Schnittstellen sind nicht symmetrisch.

Folgerungen:
- Sender- und Empfängerrollen sind getrennt.
- Verantwortlichkeiten sind eindeutig.
- Umkehr ohne Vertrag ist verboten.

---

## 7. Versionierung ist Zustandswechsel

Änderungen an Schnittstellen sind Zustandsübergänge.

Folgerungen:
- Versionen sind explizit.
- Implizite Änderungen sind verboten.
- Abwärtskompatibilität ist kein Automatismus.

---

## 8. Stabilität schlägt Flexibilität

Stabile Schnittstellen sind wichtiger als flexible.

Folgerungen:
- Erweiterung schlägt Änderung.
- Änderung ist Ausnahme.
- Brechende Änderungen sind sichtbar zu machen.

---

## 9. Fehler gehören zur Schnittstelle

Fehlerfälle sind Teil des Vertrags.

Folgerungen:
- Fehlerausgaben sind definiert.
- Schweigen ist verboten.
- Teilantworten sind kenntlich zu machen.

---

## 10. Schnittstellen sind messbar

Grenzübergänge müssen beobachtbar sein.

Folgerungen:
- Latenz ist zu messen.
- Fehlerquoten sind sichtbar.
- Dunkle Schnittstellen sind verboten.

---

## 11. Isolation ist Pflicht

Schnittstellen begrenzen Schadensausbreitung.

Folgerungen:
- Fehler dürfen Grenzen nicht durchbrechen.
- Seiteneffekte sind zu minimieren.
- Kopplung ist zu reduzieren.

---

## 12. Minimalismus ist Sicherheit

Jede zusätzliche Funktion erhöht Risiko.

Folgerungen:
- Minimale Schnittstellen sind vorzuziehen.
- Überladung ist verboten.
- Unbenutzte Pfade sind zu entfernen.

---

## 13. Architekturbindung

CommanderLink Schnittstellen binden sich an:

- explizite Verträge
- messbare Übergänge
- deterministische Grenzdefinitionen

Andere Schnittstellenmodelle erfordern neue Gesetze.

---

## 14. Schlussformel

Schnittstellen entscheiden über Ordnung oder Chaos.

Ein System mit unscharfen Grenzen
kann weder sicher noch stabil sein.

Dieses Gesetz ist Grundlage aller
ABI-, API- und Wire-Verträge.


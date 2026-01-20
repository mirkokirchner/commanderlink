# LAW_10_FAILURE.md
## CommanderLink – Fehler-, Ausfall- & Degradations-Gesetz

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Geltungsbereich

Dieses Gesetz regelt den Umgang mit Fehlern, Ausfällen,
Degradation und Wiederherstellung
in CommanderLink.

Fehler sind unvermeidliche physikalische Zustände,
keine Sonderfälle.

---

## 1. Fehler sind normal

Fehler entstehen aus Physik, Last und Zeit.

Folgerungen:
- Fehlerfreiheit ist keine Annahme.
- Fehlerzustände sind einplanbar.
- Überraschungsfehler sind Designfehler.

---

## 2. Fehler sind Zustände, keine Ereignisse

Ein Fehler ist ein anhaltender Zustand.

Folgerungen:
- Fehler haben Dauer.
- Übergänge sind explizit.
- Kurzzeitige Maskierung ersetzt keine Behandlung.

---

## 3. Degradation ist korrektes Verhalten

Leistungsreduktion erhält Stabilität.

Folgerungen:
- Degradation ist erlaubt.
- Instabilität ist verboten.
- Volle Leistung ist kein Anspruch.

---

## 4. Fail-Stop schlägt Fail-Chaos

Stillstand ist besser als unkontrolliertes Verhalten.

Folgerungen:
- Kontrollierter Abbruch ist zulässig.
- Ungebremste Weiterverarbeitung ist verboten.
- Schutz geht vor Fortschritt.

---

## 5. Recovery kostet Zeit

Wiederherstellung ist kein Nullkostenpfad.

Folgerungen:
- Recovery ist zu budgetieren.
- Häufiges Recovery ist ein Fehlerindikator.
- Endloses Recovery ist verboten.

---

## 6. Zustand vor Neustart ist relevant

Neustart löscht keine Realität.

Folgerungen:
- Vorzustände sind zu berücksichtigen.
- Blindes Reset ist verboten.
- Persistente Fehler müssen erkannt werden.

---

## 7. Isolation begrenzt Schaden

Fehler dürfen sich nicht unkontrolliert ausbreiten.

Folgerungen:
- Fehlerdomänen sind zu definieren.
- Kaskadierung ist zu verhindern.
- Isolation ist Pflicht.

---

## 8. Zeit bestimmt Fehlerwirkung

Je länger ein Fehler andauert,
desto größer der Schaden.

Folgerungen:
- Früherkennung ist entscheidend.
- Verzögerte Reaktion verschärft Schäden.
- Ignorieren ist verboten.

---

## 9. Teilfunktion schlägt Totalausfall

Reduzierte Funktion ist besser als keine.

Folgerungen:
- Graceful Degradation ist Pflicht.
- Kernfunktionen haben Vorrang.
- Komfortfunktionen sind verzichtbar.

---

## 10. Fehler müssen sichtbar sein

Unsichtbare Fehler sind gefährlich.

Folgerungen:
- Fehlerzustände sind zu messen.
- Maskierung ohne Kennzeichnung ist verboten.
- Beobachtbarkeit ist Pflicht.

---

## 11. Automatik schlägt manuelle Intervention

Automatische Reaktion ist schneller als Mensch.

Folgerungen:
- Automatische Schutzmechanismen sind Pflicht.
- Manuelle Eingriffe sind sekundär.
- Mensch ist nicht im Hot-Pfad.

---

## 12. Fehlerhistorie ist relevant

Vergangene Fehler beeinflussen Zukunft.

Folgerungen:
- Fehlerverläufe sind zu speichern.
- Wiederholungen sind zu erkennen.
- Lernen ist verpflichtend.

---

## 13. Architekturbindung

CommanderLink Fehlerbehandlung bindet sich an:

- reale Ausfallmodi der Hardware
- messbare Zustandsübergänge
- zeitabhängige Wirkung

Andere Modelle erfordern neue Gesetze.

---

## 14. Schlussformel

Ein System ohne Fehlerstrategie
ist bereits gescheitert.

Stabilität entsteht nicht durch Vermeidung von Fehlern,
sondern durch kontrollierten Umgang mit ihnen.

Dieses Gesetz ist Grundlage aller
Fallback-, Gate- und Recovery-Verträge.


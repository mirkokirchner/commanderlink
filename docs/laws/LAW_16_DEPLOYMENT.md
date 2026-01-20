# LAW_16_DEPLOYMENT.md
## CommanderLink – Ausrollungs- & Betriebs-Gesetz

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Geltungsbereich

Dieses Gesetz regelt Auslieferung, Inbetriebnahme,
Betrieb und Stilllegung
von CommanderLink.

Deployment ist ein physikalischer und organisatorischer Eingriff,
kein logistischer Nebenschritt.

---

## 1. Deployment verändert Realität

Jede Ausrollung verändert den Systemzustand.

Folgerungen:
- Deployment ist ein Zustandswechsel.
- Teildeployment ist ein Fehlerzustand.
- Ungesteuertes Deployment ist verboten.

---

## 2. Betrieb ist der Normalzustand

Der Betrieb ist kein Sonderfall.

Folgerungen:
- Betrieb bestimmt Designentscheidungen.
- Testzustände sind degradiert.
- Produktionsrealität hat Vorrang.

---

## 3. Inbetriebnahme ist kritisch

Der Start ist der fragilste Zustand.

Folgerungen:
- Initiale Last ist zu begrenzen.
- Schutzmechanismen sind aktiv.
- Optimismus ist verboten.

---

## 4. Stilllegung ist ein Zustand

Abschaltung ist Teil des Lebenszyklus.

Folgerungen:
- Geordnete Stilllegung ist Pflicht.
- Ressourcenfreigabe ist verbindlich.
- Abbruch ohne Cleanup ist verboten.

---

## 5. Rollout ist gestuft

Großflächige Änderungen erhöhen Risiko.

Folgerungen:
- Stufenweises Ausrollen ist Pflicht.
- Beobachtung zwischen Stufen ist notwendig.
- Sofortige Vollausrollung ist verboten.

---

## 6. Rollback ist Pflicht

Jede Änderung muss rückgängig gemacht werden können.

Folgerungen:
- Rückkehrpfade sind vorzuhalten.
- Einweg-Deployments sind verboten.
- Rollback-Zeit ist zu budgetieren.

---

## 7. Umgebung ist Teil des Systems

Hardware, OS und Topologie beeinflussen Verhalten.

Folgerungen:
- Umgebung ist zu erfassen.
- Annahmen über Umgebung sind verboten.
- Drift ist zu erkennen.

---

## 8. Betrieb erzeugt Verschleiß

Langzeitbetrieb verändert Systeme.

Folgerungen:
- Alterung ist einzukalkulieren.
- Ressourcenverhalten driftet.
- Periodische Neubewertung ist Pflicht.

---

## 9. Automatik schlägt manuelle Pflege

Dauerbetrieb erfordert Automatisierung.

Folgerungen:
- Manuelle Routinen sind fehleranfällig.
- Automatische Überwachung ist Pflicht.
- Mensch ist nicht der Regelkreis.

---

## 10. Updates sind Zustandswechsel

Aktualisierung ist kein Detail.

Folgerungen:
- Updates sind zu planen.
- Nebenwirkungen sind zu erwarten.
- Live-Updates ohne Schutz sind verboten.

---

## 11. Konfigurationsdrift ist Fehler

Abweichungen vom Sollzustand sind kritisch.

Folgerungen:
- Drift ist zu erkennen.
- Selbstheilung ist vorzusehen.
- Stiller Drift ist verboten.

---

## 12. Betrieb ist messpflichtig

Ein System ohne Betriebsdaten ist blind.

Folgerungen:
- Laufzeitmessung ist Pflicht.
- Langzeittrends sind relevant.
- Kurzsichtige Beobachtung ist unzureichend.

---

## 13. Architekturbindung

CommanderLink Betrieb bindet sich an:

- reale Betriebsumgebungen
- messbare Langzeitzustände
- kontrollierte Zustandswechsel

Andere Betriebsmodelle erfordern neue Gesetze.

---

## 14. Schlussformel

Ein System, das nur im Labor funktioniert,
existiert nicht.

Stabiler Betrieb ist der einzige gültige Beweis
für Korrektheit.

Dieses Gesetz ist Grundlage aller
Release-, Update- und Betriebsverträge.


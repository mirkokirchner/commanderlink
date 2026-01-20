# LAW_06_SCHEDULING.md
## CommanderLink – Ausführungs- & Scheduling-Gesetz

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Geltungsbereich

Dieses Gesetz regelt Planung, Reihenfolge und Ausführung von Arbeit
in CommanderLink.

Scheduling ist eine physikalische Disziplin,
keine abstrakte Verwaltungsaufgabe.

---

## 1. Arbeit ist endlich

Arbeit existiert nur, wenn sie ausführbar ist.

Folgerungen:
- Unausführbare Arbeit ist Last.
- Geplante Arbeit ohne Ressourcen ist verboten.
- Warteschlangen ersetzen keine Kapazität.

---

## 2. Scheduling ist Ressourcenbindung

Scheduling bindet reale Ressourcen.

Folgerungen:
- Jede geplante Einheit reserviert Zeit, Energie und Speicher.
- Planung ohne Ressourcenbindung ist ungültig.
- Überplanung ist ein Designfehler.

---

## 3. Priorität ist physikalisch

Priorität entsteht aus Notwendigkeit, nicht aus Wunsch.

Folgerungen:
- Zeitkritische Arbeit dominiert.
- Interaktive Pfade haben Vorrang.
- Hintergrundarbeit ist verdrängbar.

---

## 4. Fairness ist sekundär

Stabilität schlägt Fairness.

Folgerungen:
- Gleichbehandlung ist kein Ziel.
- Kritische Pfade dürfen bevorzugt werden.
- Starvation ist erlaubt, Instabilität nicht.

---

## 5. Preemption ist Verlust

Verdrängung zerstört Fortschritt.

Folgerungen:
- Preemption ist zu minimieren.
- Kurze, abgeschlossene Einheiten sind vorzuziehen.
- Lange, nicht unterbrechbare Arbeit ist verboten.

---

## 6. Arbeitseinheiten sind endlich

Jede Arbeitseinheit benötigt klare Grenzen.

Folgerungen:
- Unbegrenzte Tasks sind verboten.
- Fortschritt muss messbar sein.
- Abbruchpfade sind Pflicht.

---

## 7. Rückstau ist korrektes Verhalten

Backlog signalisiert physikalische Grenzen.

Folgerungen:
- Rückstau ist sichtbar zu machen.
- Ablehnung ist besser als Überlast.
- Drop vor Chaos.

---

## 8. Zeitbudget ist verbindlich

Jede Arbeit erhält ein Zeitbudget.

Folgerungen:
- Budgetüberschreitung ist ein Fehler.
- Arbeit darf gestoppt werden.
- Elastizität ist explizit zu definieren.

---

## 9. Koordination kostet Zeit

Abstimmung zwischen Einheiten erzeugt Overhead.

Folgerungen:
- Zentrale Koordination ist teuer.
- Lokale Entscheidungen sind stabiler.
- Globale Locks sind verboten.

---

## 10. Reihenfolge ist ein Vertrag

Ausführungsreihenfolge beeinflusst Ergebnis.

Folgerungen:
- Ordnung ist explizit festzulegen.
- Implizite Reihenfolgen sind verboten.
- Nebenläufigkeit ohne Ordnung ist instabil.

---

## 11. Stillstand ist ein Zustand

Idle ist kein Fehler.

Folgerungen:
- Leerlauf ist energetisch korrekt.
- Künstliche Auslastung ist verboten.
- Ruhe stabilisiert Systeme.

---

## 12. Messung steuert Scheduling

Ohne Messung keine Planung.

Folgerungen:
- Laufzeiten sind zu erfassen.
- Abweichungen sind zu reagieren.
- Planung ohne Rückkopplung ist blind.

---

## 13. Architekturbindung

CommanderLink Scheduling bindet sich an:

- reale CPU-Kerne
- bekannte Preemption-Mechanismen
- messbare Laufzeiten

Andere Modelle erfordern eigene Gesetze.

---

## 14. Schlussformel

Scheduling entscheidet über Stabilität.

Wer mehr plant als ausführen kann,
verliert Kontrolle über das System.

Dieses Gesetz ist Grundlage aller
Executor-, Loop- und Ablaufmodelle.


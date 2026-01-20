# LAW_13_STATE.md
## CommanderLink – Zustands- & Kontinuitäts-Gesetz

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Geltungsbereich

Dieses Gesetz regelt Zustände, Zustandsübergänge
und Kontinuität aller Komponenten in CommanderLink.

Zustand ist eine physikalisch reale Tatsache,
keine logische Abstraktion.

---

## 1. Zustand ist vollständig

Ein Zustand beschreibt die gesamte relevante Realität
einer Einheit zu einem Zeitpunkt.

Folgerungen:
- Teilzustände sind unzulässig.
- Implizite Annahmen sind verboten.
- Unvollständige Zustände sind Fehler.

---

## 2. Zustand ist eindeutig

Zu jedem Zeitpunkt existiert genau ein gültiger Zustand.

Folgerungen:
- Überlagerte Zustände sind verboten.
- Ambivalenz ist ein Fehlerzustand.
- Gleichzeitige Mehrzustände sind unzulässig.

---

## 3. Zustandsübergänge sind diskret

Ein Übergang trennt zwei Zustände eindeutig.

Folgerungen:
- Übergänge sind explizit.
- Implizite Übergänge sind verboten.
- Übergangsdauer ist ein messbarer Zeitraum.

---

## 4. Übergänge kosten Zeit

Jeder Zustandswechsel verbraucht Zeit und Ressourcen.

Folgerungen:
- Übergänge sind zu budgetieren.
- Häufige Wechsel erzeugen Instabilität.
- Flattern ist ein Fehler.

---

## 5. Vorheriger Zustand ist relevant

Der vorherige Zustand beeinflusst den nächsten.

Folgerungen:
- Zustandslosigkeit ist verboten.
- Reset ohne Kontext ist unzulässig.
- Historie beeinflusst Verhalten.

---

## 6. Initialzustand ist definiert

Jede Einheit besitzt einen klaren Anfangszustand.

Folgerungen:
- Implizite Initialisierung ist verboten.
- Unbekannter Startzustand ist ein Fehler.
- Initialzustand ist überprüfbar.

---

## 7. Endzustand ist definiert

Auch Beendigung ist ein Zustand.

Folgerungen:
- Unsauberes Ende ist unzulässig.
- Ressourcenfreigabe ist Teil des Zustands.
- Zombie-Zustände sind verboten.

---

## 8. Zustand ist beobachtbar

Aktuelle Zustände müssen sichtbar sein.

Folgerungen:
- Verborgene Zustände sind verboten.
- Beobachtbarkeit ist Pflicht.
- Verzögerte Sichtbarkeit ist zu kennzeichnen.

---

## 9. Zustand ist konsistent

Zustand darf sich nicht selbst widersprechen.

Folgerungen:
- Inkonsistenz ist ein Fehlerzustand.
- Widersprüche erfordern sofortige Klärung.
- Teilreparaturen sind unzulässig.

---

## 10. Persistenz ist explizit

Nicht jeder Zustand ist persistent.

Folgerungen:
- Persistenz ist zu deklarieren.
- Implizite Persistenz ist verboten.
- Flüchtige Zustände sind kenntlich zu machen.

---

## 11. Zustandskopien sind Momentaufnahmen

Kopierte Zustände altern sofort.

Folgerungen:
- Kopien sind zeitlich gebunden.
- Nutzung ohne Zeitkontext ist verboten.
- Snapshots ersetzen keine Live-Sicht.

---

## 12. Zustand steuert Verhalten

Logik folgt Zustand, nicht umgekehrt.

Folgerungen:
- Verhalten ohne Zustandsbasis ist verboten.
- Zustand bestimmt erlaubte Aktionen.
- Missachtung führt zu Instabilität.

---

## 13. Architekturbindung

CommanderLink Zustandsmodell bindet sich an:

- messbare Systemrealität
- zeitlich diskrete Übergänge
- deterministische Zustandsdefinitionen

Andere Zustandsmodelle erfordern neue Gesetze.

---

## 14. Schlussformel

Zustand ist die Wahrheit eines Systems.

Ein System ohne klares Zustandsmodell
verliert Kontinuität, Kontrolle und Determinismus.

Dieses Gesetz ist Grundlage aller
Lifecycle-, Recovery- und Kontrollmechanismen.


# LAW_11_CONTROL.md
## CommanderLink – Steuerungs- & Interlock-Gesetz

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Geltungsbereich

Dieses Gesetz regelt Steuerung, Rückkopplung,
Interlocks und Eingriffsmechanismen
in CommanderLink.

Steuerung ist ein physikalischer Regelkreis,
kein abstraktes Kommandomodell.

---

## 1. Steuerung setzt Messung voraus

Ohne Messung keine Kontrolle.

Folgerungen:
- Unbeobachtete Steuerung ist verboten.
- Steuerentscheidungen ohne aktuelle Daten sind ungültig.
- Verzögerte Messung reduziert Wirksamkeit.

---

## 2. Regelkreise sind zeitabhängig

Jeder Regelkreis besitzt eine Reaktionszeit.

Folgerungen:
- Zu schnelle Regelung erzeugt Instabilität.
- Zu langsame Regelung ist wirkungslos.
- Regelperioden sind explizit festzulegen.

---

## 3. Rückkopplung ist zwingend

Steuerung ohne Rückmeldung ist Blindflug.

Folgerungen:
- Jede Aktion benötigt Feedback.
- Fehlendes Feedback erzwingt Degradation.
- Positive Rückkopplung ist zu begrenzen.

---

## 4. Interlocks haben Vorrang

Interlocks schützen vor physikalischem Schaden.

Folgerungen:
- Interlocks übersteuern Logik.
- Interlocks sind nicht abschaltbar.
- Umgehung ist verboten.

---

## 5. Hysterese ist Pflicht

Sofortige Umschaltung erzeugt Flattern.

Folgerungen:
- Zustandswechsel benötigen Hysterese.
- Grenzwerte sind nicht punktuell.
- Flatterzustände sind Fehler.

---

## 6. Steuerung ist minimal-invasiv

Eingriffe verändern das System.

Folgerungen:
- Kleinster wirksamer Eingriff ist vorzuziehen.
- Übersteuerung ist Ausnahme.
- Dauerhafte Eingriffe sind zu vermeiden.

---

## 7. Lokale Kontrolle schlägt globale

Lokale Regelung ist stabiler als zentrale.

Folgerungen:
- Dezentralität reduziert Latenz.
- Zentrale Steuerung ist teuer.
- Globale Entscheidungen sind begrenzt einzusetzen.

---

## 8. Priorität schützt Kernfunktionen

Nicht alle Funktionen sind gleichwertig.

Folgerungen:
- Schutz kritischer Pfade hat Vorrang.
- Komfortfunktionen sind abschaltbar.
- Steuerung folgt Funktionshierarchie.

---

## 9. Automatik ist der Default

Maschinelle Reaktion ist schneller als Mensch.

Folgerungen:
- Automatische Steuerung ist Pflicht.
- Manuelle Eingriffe sind nachrangig.
- Menschliche Reaktion ist außerhalb des Regelkreises.

---

## 10. Eskalation ist gestuft

Nicht jeder Zustand erfordert Maximalreaktion.

Folgerungen:
- Mehrstufige Reaktion ist Pflicht.
- Sofortiger Hard-Stop nur bei Gefahr.
- Eskalationspfade sind definiert.

---

## 11. Kontrolle ist reversibel

Steuermaßnahmen dürfen rückgängig gemacht werden.

Folgerungen:
- Dauerhafte Sperren sind zu vermeiden.
- Rückkehr in stabilen Zustand ist Ziel.
- Irreversible Eingriffe sind Ausnahmefälle.

---

## 12. Steuerung ist sichtbar

Kontrollzustände müssen beobachtbar sein.

Folgerungen:
- Aktive Interlocks sind sichtbar zu machen.
- Eingriffe sind zu kennzeichnen.
- Unsichtbare Kontrolle ist verboten.

---

## 13. Architekturbindung

CommanderLink Steuerung bindet sich an:

- messbare Zustände
- zeitgebundene Regelkreise
- physikalisch begründete Interlocks

Andere Modelle erfordern neue Gesetze.

---

## 14. Schlussformel

Steuerung entscheidet über Überleben.

Ein System ohne Interlocks
verwechselt Freiheit mit Instabilität.

Dieses Gesetz ist Grundlage aller
Gate-, Budget- und Schutzmechanismen.


# LAW_08_SECURITY.md
## CommanderLink – Sicherheits- & Vertrauens-Gesetz

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Geltungsbereich

Dieses Gesetz regelt Sicherheit, Vertrauen und Integrität
aller Komponenten, Pfade und Interaktionen
in CommanderLink.

Sicherheit ist ein physikalischer und systemischer Zustand,
kein Zusatzmerkmal.

---

## 1. Vertrauen ist nicht implizit

Kein Bestandteil ist per Existenz vertrauenswürdig.

Folgerungen:
- Initialer Zustand ist **nicht vertrauenswürdig**.
- Vertrauen muss explizit entstehen.
- Implizites Vertrauen ist verboten.

---

## 2. Integrität ist physikalisch

Integrität bedeutet Unverändertheit unter realen Bedingungen.

Folgerungen:
- Daten können beschädigt werden.
- Übertragung ist fehleranfällig.
- Speicher ist nicht perfekt.

---

## 3. Sicherheit kostet Ressourcen

Sicherheitsmechanismen verbrauchen Zeit, Energie und Speicher.

Folgerungen:
- Sicherheit ist zu budgetieren.
- Unbudgetierte Sicherheit ist verboten.
- Unsichtbare Kosten sind ein Fehler.

---

## 4. Sichtbarkeit ist Voraussetzung für Sicherheit

Was nicht sichtbar ist, kann nicht geschützt werden.

Folgerungen:
- Sicherheitszustände müssen messbar sein.
- Verdeckte Entscheidungen sind verboten.
- Blindes Vertrauen ist unzulässig.

---

## 5. Authentizität ist eindeutig

Identitäten müssen eindeutig bestimmbar sein.

Folgerungen:
- Mehrdeutige Identitäten sind verboten.
- Identitätswechsel ist ein Zustand.
- Identitätsverlust ist ein Fehler.

---

## 6. Autorisierung ist zustandsabhängig

Berechtigung hängt vom aktuellen Zustand ab.

Folgerungen:
- Statische Berechtigungen sind unzureichend.
- Kontext bestimmt Zugriff.
- Überprivilegierung ist verboten.

---

## 7. Geheimnisse sind Last

Geheime Daten erzeugen dauerhafte Verantwortung.

Folgerungen:
- Geheimnisse sind zu minimieren.
- Lebensdauer ist zu begrenzen.
- Persistenz ohne Notwendigkeit ist verboten.

---

## 8. Kryptographie ist Physik

Kryptographie unterliegt Zeit, Energie und Implementierung.

Folgerungen:
- Kryptographie ist kein Allheilmittel.
- Falsche Nutzung erzeugt Scheinsicherheit.
- Implementierungsfehler sind Sicherheitsfehler.

---

## 9. Fehler sind Angriffsflächen

Fehlerzustände erhöhen Angriffsrisiken.

Folgerungen:
- Fehlerpfade sind abzusichern.
- Undefined Behavior ist verboten.
- Recovery darf keine Lücken öffnen.

---

## 10. Isolation ist relativ

Isolation ist niemals absolut.

Folgerungen:
- Grenzen sind zu definieren.
- Seiteneffekte sind einzukalkulieren.
- Vertrauen endet an der Grenze.

---

## 11. Reduktion schlägt Komplexität

Komplexität ist ein Sicherheitsrisiko.

Folgerungen:
- Minimalismus erhöht Sicherheit.
- Überflüssige Mechanismen sind zu entfernen.
- Vielfalt ohne Kontrolle ist verboten.

---

## 12. Sicherheit ist dynamisch

Sicherheitszustände ändern sich über Zeit.

Folgerungen:
- Vertrauen kann entzogen werden.
- Re-Evaluierung ist Pflicht.
- Dauerhafte Annahmen sind verboten.

---

## 13. Architekturbindung

CommanderLink Sicherheit bindet sich an:

- reale Hardware-Eigenschaften
- messbare Zustände
- explizite Vertrauensübergänge

Abweichungen erfordern neue Gesetze.

---

## 14. Schlussformel

Sicherheit ist kein Zustand, den man erreicht,
sondern ein Zustand, den man aufrechterhält.

Wer Sicherheit voraussetzt,
hat sie bereits verloren.

Dieses Gesetz ist Grundlage aller
Trust-, Join- und Integritätsverträge.


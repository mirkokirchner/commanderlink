# LAW_20_VERIFICATION.md
## CommanderLink – Verifikations-, Test- & Beweis-Gesetz

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Geltungsbereich

Dieses Gesetz regelt Verifikation, Test, Nachweis
und Beweisführung
für CommanderLink.

Verifikation ist der Beweis der Gesetzestreue,
nicht eine Qualitätsvermutung.

---

## 1. Verifikation ist Pflicht

Unverifiziertes Verhalten ist ungültig.

Folgerungen:
- Jede relevante Eigenschaft ist zu verifizieren.
- Vertrauen ohne Nachweis ist verboten.
- Nicht prüfbares Verhalten ist unzulässig.

---

## 2. Gesetze definieren Tests

Tests leiten sich aus Gesetzen ab, nicht umgekehrt.

Folgerungen:
- Jeder Test referenziert mindestens ein Gesetz.
- Tests ohne Gesetzesbezug sind zu entfernen.
- Änderung eines Gesetzes erzwingt Testanpassung.

---

## 3. Worst-Case ist der Referenzfall

Verifikation erfolgt am schlimmsten Fall.

Folgerungen:
- Durchschnittstests sind unzureichend.
- Grenzfälle sind Pflicht.
- Erfolg im Best-Case ist irrelevant.

---

## 4. Determinismus ist beweispflichtig

Deterministisches Verhalten muss nachgewiesen werden.

Folgerungen:
- Wiederholbarkeit ist Pflicht.
- Abweichungen sind Fehler.
- Flaky Tests sind unzulässig.

---

## 5. Messung schlägt Simulation

Reale Messung hat Vorrang vor Modellannahmen.

Folgerungen:
- Hardware-nahe Tests sind Referenz.
- Simulation ist degradiert.
- Annahmen ohne Messung sind verboten.

---

## 6. Störung ist Teil des Tests

Tests müssen Störungen einschließen.

Folgerungen:
- Überlast, Hitze, Jitter sind zu provozieren.
- Ideale Bedingungen sind unzureichend.
- Robustheit ist zu beweisen.

---

## 7. Fehlerpfade sind primär

Fehlerpfade sind gleichwertig zu testen.

Folgerungen:
- Fehlerfalltests sind Pflicht.
- Recovery ist zu verifizieren.
- Untestbare Fehlerpfade sind verboten.

---

## 8. Langzeitverhalten ist relevant

Kurztests reichen nicht aus.

Folgerungen:
- Dauerläufe sind Pflicht.
- Drift ist zu erkennen.
- Stabilität über Zeit ist nachzuweisen.

---

## 9. Isolation ist überprüfbar

Grenzen müssen standhalten.

Folgerungen:
- Fehler dürfen Grenzen nicht überschreiten.
- Kaskaden sind zu testen.
- Isolation ohne Nachweis ist ungültig.

---

## 10. Regression ist sichtbar

Einmal korrektes Verhalten bleibt korrekt.

Folgerungen:
- Regressionstests sind Pflicht.
- Stillen Verschlechterungen sind verboten.
- Jede Abweichung ist zu begründen.

---

## 11. Beweise sind reproduzierbar

Nachweise müssen wiederholbar sein.

Folgerungen:
- Testumgebung ist definiert.
- Abhängigkeiten sind dokumentiert.
- Zufälligkeit ist ausgeschlossen.

---

## 12. Verifikation endet nicht

Änderung erzwingt neue Verifikation.

Folgerungen:
- Freigabe ist zeitgebunden.
- Alternde Nachweise sind ungültig.
- Pflege ist dauerhaft.

---

## 13. Architekturbindung

CommanderLink Verifikation bindet sich an:

- reale x86_64 Hardware
- messbare Zustände
- gesetzesgeleitete Testableitung

Andere Verifikationsmodelle erfordern neue Gesetze.

---

## 14. Schlussformel

Was nicht bewiesen ist,
gilt als falsch.

Verifikation ist kein Schritt im Prozess,
sondern die Existenzbedingung des Systems.

Dieses Gesetz ist Grundlage aller
Test-, Audit- und Freigabeentscheidungen.


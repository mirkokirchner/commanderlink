# LAW_19_IMPLEMENTATION.md
## CommanderLink – Implementierungs- & Ableitungs-Gesetz

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Geltungsbereich

Dieses Gesetz regelt die Ableitung von Implementierungen
aus den CommanderLink-Gesetzen.

Implementierung ist die physikalische Realisierung von Recht,
nicht dessen Interpretation.

---

## 1. Gesetze sind primär

Implementierungen existieren ausschließlich zur Umsetzung der Gesetze.

Folgerungen:
- Implementierungen haben keine Eigenautorität.
- „Funktioniert in der Praxis“ rechtfertigt keinen Gesetzesbruch.
- Optimierung ohne Gesetzesgrundlage ist verboten.

---

## 2. Ableitung ist deterministisch

Aus Gesetzen folgt genau ein erlaubter Lösungsraum.

Folgerungen:
- Mehrdeutige Ableitungen sind unzulässig.
- Implizite Annahmen sind verboten.
- Jeder Designschritt muss rückführbar sein.

---

## 3. Kein Code ohne Gesetz

Code ohne gesetzliche Grundlage ist nicht existent.

Folgerungen:
- Jedes Modul referenziert mindestens ein Gesetz.
- Unreferenzierter Code ist zu entfernen.
- „Vorbereitender Code“ ist verboten.

---

## 4. Implementierung respektiert Physik

Kein Code darf physikalische Realität abstrahieren oder ignorieren.

Folgerungen:
- Worst-Case-Pfad ist maßgeblich.
- Undefined Behavior ist verboten.
- Heuristik ohne Messung ist unzulässig.

---

## 5. Einfachheit ist bindend

Komplexität ist ein Implementierungsrisiko.

Folgerungen:
- Der einfachste gesetzeskonforme Ansatz ist zu wählen.
- Cleverness ohne Notwendigkeit ist verboten.
- Komplexität erfordert explizite Rechtfertigung.

---

## 6. Sichtbarkeit ist Pflicht

Implementiertes Verhalten muss beobachtbar sein.

Folgerungen:
- Stille Effekte sind verboten.
- Interne Zustände sind messbar zu machen.
- Debug-Pfad ist kein Sonderpfad.

---

## 7. Fehlerpfade sind gleichwertig

Fehlerpfade sind Teil der Implementierung.

Folgerungen:
- Fehlerpfade sind vollständig implementiert.
- Abkürzungen im Fehlerfall sind verboten.
- Recovery ist kein Nachgedanke.

---

## 8. Performance ist kein Selbstzweck

Performance dient Stabilität und Determinismus.

Folgerungen:
- Schnell, aber instabil ist ungültig.
- Vorhersagbarkeit schlägt Spitzenleistung.
- Optimierung ohne Messgrundlage ist verboten.

---

## 9. Portabilität ist nachrangig

Physikalische Korrektheit hat Vorrang vor Portabilität.

Folgerungen:
- x86_64 ist Referenzarchitektur.
- Abstraktion darf keine Kosten verschleiern.
- Andere Architekturen benötigen neue Gesetze.

---

## 10. Implementierung ist überprüfbar

Code muss überprüfbar sein.

Folgerungen:
- Klarheit schlägt Kürze.
- Deterministisches Verhalten ist beweisbar.
- Verifikation ist vorgesehen.

---

## 11. Implementierung ist austauschbar

Kein Implementierungsdetail ist sakrosankt.

Folgerungen:
- Austausch ohne Gesetzesänderung ist möglich.
- Lock-in durch Implementation ist verboten.
- Verhalten bleibt konstant.

---

## 12. Freeze ist bindend

Freigegebene Implementierungen sind eingefroren.

Folgerungen:
- Änderungen erfordern neue Freigabe.
- Stillen Drift gibt es nicht.
- Hotfix ohne Gesetz ist verboten.

---

## 13. Architekturbindung

CommanderLink Implementierungen binden sich an:

- x86_64 Physik
- explizite Gesetze
- deterministische Ableitung

Andere Implementierungsmodelle erfordern neue Gesetze.

---

## 14. Schlussformel

Implementierung ist Ausführung von Recht.

Ein System, das seine Gesetze nicht einhalten kann,
ist nicht falsch implementiert –
es ist falsch entworfen.

Dieses Gesetz ist Grundlage aller
Header-, Code- und Build-Artefakte.


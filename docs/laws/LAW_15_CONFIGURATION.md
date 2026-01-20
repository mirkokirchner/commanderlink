# LAW_15_CONFIGURATION.md
## CommanderLink – Konfigurations- & Parameter-Gesetz

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Geltungsbereich

Dieses Gesetz regelt Konfiguration, Parameterisierung
und alle einstellbaren Werte
in CommanderLink.

Konfiguration verändert physikalisches Verhalten
und ist kein Komfortmerkmal.

---

## 1. Konfiguration ist Eingriff

Jede Konfiguration verändert den Systemzustand.

Folgerungen:
- Konfigurationsänderungen sind Zustandswechsel.
- Unbeabsichtigte Änderungen sind Fehler.
- Laufzeitänderungen sind besonders kritisch.

---

## 2. Default ist ein Vertrag

Ein Default-Wert ist eine bewusste Entscheidung.

Folgerungen:
- Defaults müssen stabil sein.
- Unsichere Defaults sind verboten.
- „Unkonfiguriert“ ist kein Zustand.

---

## 3. Konfiguration ist endlich

Nicht alles darf konfigurierbar sein.

Folgerungen:
- Physikalische Grenzen sind nicht konfigurierbar.
- Kernverhalten ist fest.
- Überkonfiguration ist ein Designfehler.

---

## 4. Konfiguration ersetzt keine Logik

Parameter dürfen keine fehlende Steuerung kompensieren.

Folgerungen:
- Parameter ohne Regelkreis sind verboten.
- Manuelle Feinabstimmung ersetzt keine Kontrolle.
- Magische Werte sind unzulässig.

---

## 5. Konfiguration ist deterministisch

Gleiche Konfiguration erzeugt gleiches Verhalten.

Folgerungen:
- Kontextabhängige Interpretation ist verboten.
- Zufällige Initialisierung ist unzulässig.
- Reihenfolgeeffekte sind zu vermeiden.

---

## 6. Konfiguration ist validierbar

Jede Konfiguration muss prüfbar sein.

Folgerungen:
- Ungültige Konfigurationen sind abzulehnen.
- Grenzwerte sind explizit.
- Teilgültigkeit ist verboten.

---

## 7. Konfiguration ist sichtbar

Aktive Konfigurationen müssen beobachtbar sein.

Folgerungen:
- Versteckte Parameter sind verboten.
- Laufende Werte sind einsehbar.
- Abweichungen sind erkennbar.

---

## 8. Konfiguration ist versioniert

Änderungen an Konfigurationen sind nachvollziehbar.

Folgerungen:
- Versionierung ist Pflicht.
- Implizite Migration ist verboten.
- Rollback muss möglich sein.

---

## 9. Konfiguration ist minimal

Weniger Parameter erhöhen Stabilität.

Folgerungen:
- Reduktion schlägt Flexibilität.
- Selten genutzte Parameter sind zu entfernen.
- Sonderfälle sind zu eliminieren.

---

## 10. Konfiguration folgt Physik

Parameter dürfen physikalische Realität nicht verletzen.

Folgerungen:
- Wunschwerte sind irrelevant.
- Physikalische Messung schlägt Konfiguration.
- Konflikte erzwingen Korrektur.

---

## 11. Konfiguration ist kein Geheimnis

Konfiguration ist kein Sicherheitsmechanismus.

Folgerungen:
- Versteckte Werte bieten keine Sicherheit.
- Sicherheit erfolgt an anderer Stelle.
- Obfuskation ist verboten.

---

## 12. Konfiguration endet an der Grenze

Externe Konfiguration darf interne Gesetze nicht brechen.

Folgerungen:
- Schnittstellen begrenzen Konfiguration.
- Überschreibung ohne Vertrag ist verboten.
- Externe Kontrolle ist eingeschränkt.

---

## 13. Architekturbindung

CommanderLink Konfiguration bindet sich an:

- deterministische Parameter
- explizite Validierung
- messbares Verhalten

Andere Konfigurationsmodelle erfordern neue Gesetze.

---

## 14. Schlussformel

Konfiguration ist Macht über das System.

Wer Konfiguration ungezügelt zulässt,
tauscht Kontrolle gegen Chaos.

Dieses Gesetz ist Grundlage aller
Policy-, Tuning- und Betriebsverträge.


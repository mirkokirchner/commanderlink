# LAW_33_POLICY.md
## CommanderLink – Policy-, Entscheidungs- & Ableitungs-Gesetz (Manifest/policy)

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Zweck

Dieses Gesetz definiert den **Inhalt, die Grenzen und die Disziplin**
der Manifest-Ebene `manifest/policy/`.

`policy/` beschreibt **wie entschieden wird**, nicht **was gemessen wird**
und nicht **wie es implementiert ist**.

---

## 1. Policy ist Ableitung, keine Physik

Policy darf **keine** physikalischen Messwerte enthalten.

Verboten sind:
- konkrete Zahlen (Bytes, Hz, ms, Watt)
- hardwareabhängige Limits
- feste Schwellenwerte

Policy operiert ausschließlich auf:
- Begriffen aus `terms/`
- Klassen aus `base/`
- Zuständen aus `terms/`

---

## 2. Policy ist deterministisch

Gleiche Eingaben → gleiche Entscheidung.

Folgerungen:
- Keine Zufälligkeit
- Keine Zeitabhängigkeit
- Keine impliziten Nebenbedingungen

Nichtdeterministische Policy ist verboten.

---

## 3. Policy ist vollständig

Für jeden erlaubten Eingaberaum existiert eine Entscheidung.

Folgerungen:
- Keine „undefinierten Fälle“
- Kein „best effort“
- Kein Schweigen

Fehlende Regel ist ein Policy-Fehler.

---

## 4. Policy ist zustandsbasiert

Entscheidungen hängen vom **Zustand** ab, nicht von Wunschwerten.

Folgerungen:
- Zustand bestimmt Handlungsspielraum
- Policy kennt Übergänge, keine Aktionen
- Aktionen sind Runtime-Ableitung

---

## 5. Policy ist monotone Ordnung

Policy darf keine widersprüchlichen Entscheidungen erzeugen.

Folgerungen:
- Verschärfung führt nie zu Lockerung
- Degradation ist einseitig
- Eskalation ist gerichtet

Zyklen in der Entscheidungslogik sind verboten.

---

## 6. Policy kennt Prioritäten

Nicht alle Ziele sind gleichwertig.

Bindende Reihenfolge:
1. Physikalische Sicherheit
2. Stabilität des Wirts (Symbiont)
3. Determinismus / Worst-Case
4. Fortschritt / Durchsatz
5. Komfort / Opportunismus

Abweichung ist verboten.

---

## 7. Policy ist reversibel

Policy-Entscheidungen müssen rücknehmbar sein.

Folgerungen:
- Kein irreversibler Zustand
- Rückkehrpfade sind definiert
- Lock-in ist verboten

---

## 8. Policy ist transparent

Policy-Zustände sind sichtbar und erklärbar.

Folgerungen:
- Aktive Policy ist identifizierbar
- Entscheidungsgrundlage ist benennbar
- Verdeckte Policy ist verboten

---

## 9. Policy ist schmal

Policy enthält **nur** Entscheidungsregeln.

Verboten sind:
- Implementierungshinweise
- Optimierungstipps
- Workarounds
- Sonderfälle ohne Gesetzesbezug

Ein schlanker Policy-Raum ist stabiler.

---

## 10. Trennung zu Runtime

Policy entscheidet **was gelten soll**,
Runtime entscheidet **was tatsächlich passiert**.

Folgerungen:
- Policy kennt keine Messung
- Runtime kennt keine Normsetzung
- Rückkopplung erfolgt über Zustandsabbildung

---

## 11. Naming-Regeln für `policy/`

Dateien in `manifest/policy/`:

- Präfix `clm_policy_`
- Inhalt: Entscheidungsregeln als Enums / Tabellen / Zustandsmatrizen
- Keine Structs mit Layout
- Keine Zahlen mit physikalischer Bedeutung

Beispiele:
- `clm_policy_budget.h`
- `clm_policy_degradation.h`
- `clm_policy_trust.h`

---

## 12. Validitätskriterium

Ein Policy-Artefakt ist gültig, wenn:

- jede Entscheidung deterministisch ist,
- alle Eingabekombinationen abgedeckt sind,
- keine Physik-Zahlen enthalten sind,
- und jede Regel auf Gesetze rückführbar ist.

---

## 13. Schlussformel

Policy ist das letzte Wort **vor** der Realität.

Wer Policy mit Physik vermischt,
verliert Kontrolle über beide.

Mit diesem Gesetz ist die **Manifest-Ebene vollständig**.

**Nächster erlaubter Schritt:**
Freeze von `manifest/terms`, `manifest/base`, `manifest/policy`
und Beginn der **Runtime-Ableitung (ABI/SHM)**.


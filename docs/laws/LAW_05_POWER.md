# LAW_05_POWER.md
## CommanderLink – Energie- & Leistungs-Gesetz

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Geltungsbereich

Dieses Gesetz regelt Energieaufnahme, Leistungszustände
und deren Auswirkungen auf das Systemverhalten
für CommanderLink.

Energie ist eine physikalische Zwangsgröße.

---

## 1. Leistung ist Energie pro Zeit

Leistung entsteht ausschließlich durch Energieverbrauch.

Folgerungen:
- Rechenleistung ist kein abstrakter Wert.
- Jede Operation kostet messbare Energie.
- Unbudgetierter Verbrauch ist verboten.

---

## 2. Energiequellen sind begrenzt

Energiezufuhr ist endlich.

Folgerungen:
- Power-Limits sind harte Grenzen.
- Überschreitung erzwingt Korrektur.
- Ignorieren von Limits ist ein Systemfehler.

---

## 3. Leistungszustände sind diskret

Power-States sind endlich und hardwaredefiniert.

Folgerungen:
- Übergänge kosten Zeit.
- Häufige Wechsel erzeugen Instabilität.
- Unkontrollierte Zustandswechsel sind verboten.

---

## 4. Thermik folgt Energie

Temperatur ist eine Folge von Energie.

Folgerungen:
- Thermische Effekte sind verzögert.
- Prävention ist Pflicht.
- Reaktion allein ist zu spät.

---

## 5. Turbo ist opportunistisch

Leistungsreserven sind nicht garantiert.

Folgerungen:
- Turbo darf nicht eingeplant werden.
- Planung erfolgt auf Basis garantierter Zustände.
- Turbo ist ein Bonus, kein Vertrag.

---

## 6. Drosselung ist korrektes Verhalten

Drosselung schützt das System.

Folgerungen:
- Drosselung ist kein Fehler.
- Verdrängung von Arbeit ist erlaubt.
- Instabilität ist schlimmer als Langsamkeit.

---

## 7. Energieeffizienz ist Pflicht

Maximaler Verbrauch ohne Nutzen ist verboten.

Folgerungen:
- Energie pro Arbeitseinheit ist maßgeblich.
- Ineffiziente Pfade sind zu eliminieren.
- Idle-Verbrauch ist zu minimieren.

---

## 8. Leistungsspitzen sind schädlich

Spitzenbelastung destabilisiert.

Folgerungen:
- Glättung ist Pflicht.
- Burst-Leistung ohne Budget ist verboten.
- Gleichmäßigkeit erhöht Stabilität.

---

## 9. Energie ist messpflichtig

Ohne Messung keine Kontrolle.

Folgerungen:
- Energie- und Power-Messungen sind verpflichtend.
- Schätzungen ersetzen keine Messung.
- Messqualität ist offenzulegen.

---

## 10. Energie bestimmt Scheduling

Arbeit wird nach Energiezustand geplant.

Folgerungen:
- Hohe Last erfordert niedrige Energieintensität.
- Scheduling ohne Power-Kontext ist ungültig.
- Energetische Rückkopplung ist Pflicht.

---

## 11. Energie dominiert Performance

Mehr Energie ist nicht automatisch mehr Leistung.

Folgerungen:
- Effizienz schlägt rohe Leistung.
- Überhitzung zerstört Durchsatz.
- Stabilität hat Vorrang.

---

## 12. Architekturbindung

CommanderLink bindet sich an:

- reale Power-Limits der Hardware
- bekannte Power-States
- messbare Energiepfade

Abweichungen erfordern neue Gesetze.

---

## 13. Schlussformel

Energie ist der stille Regulator aller Systeme.

Wer Energie ignoriert,
verliert Kontrolle über Zeit und Stabilität.

Dieses Gesetz ist Grundlage aller
Budget-, Gate- und Fallback-Mechanismen.


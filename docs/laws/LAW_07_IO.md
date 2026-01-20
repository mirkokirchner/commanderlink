# LAW_07_IO.md
## CommanderLink – Ein-/Ausgabe-Gesetz (I/O & Gerätepfade)

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Geltungsbereich

Dieses Gesetz regelt alle Formen von Ein- und Ausgabe
zwischen CPU, Speicher und externen Geräten
für CommanderLink.

I/O ist physikalische Interaktion, keine Abstraktion.

---

## 1. I/O ist langsamer als Rechenlogik

Jeder I/O-Vorgang ist teurer als lokale Berechnung.

Folgerungen:
- I/O ist zu minimieren.
- Berechnung vor Transfer.
- Vermeidung schlägt Beschleunigung.

---

## 2. Geräte sind eigenständige Akteure

Geräte besitzen eigene Zustände und Latenzen.

Folgerungen:
- Geräteverhalten ist nicht deterministisch steuerbar.
- Wartezeiten sind real.
- Gerätefehler sind physikalische Zustände.

---

## 3. Pfade bestimmen Kosten

Der I/O-Pfad definiert Latenz und Stabilität.

Folgerungen:
- Direkte Pfade sind vorzuziehen.
- Umwege erhöhen Jitter.
- Verborgene Pfade sind verboten.

---

## 4. DMA ist kein kostenloser Transport

DMA verlagert Arbeit, eliminiert sie nicht.

Folgerungen:
- Cache-Kohärenz kostet Zeit.
- Synchronisation ist verpflichtend.
- Blindes DMA ist verboten.

---

## 5. Blockierende I/O ist Zeitvernichtung

Blockierung hält Ressourcen fest.

Folgerungen:
- Blockierende I/O im Hot-Pfad ist verboten.
- Zeitlich gebundene Operationen sind Pflicht.
- Fortschritt muss beobachtbar sein.

---

## 6. Asynchrone I/O erfordert Kontrolle

Asynchronität erzeugt Nebenläufigkeit.

Folgerungen:
- Rückmeldung ist Pflicht.
- Backpressure ist zwingend.
- Unkontrollierte Parallel-I/O ist verboten.

---

## 7. Puffer sind Zustandsverschiebung

I/O-Puffer verschieben Zeit und Verantwortung.

Folgerungen:
- Puffergrößen sind zu begrenzen.
- Implizite Puffer sind verboten.
- Große Puffer erhöhen Latenz.

---

## 8. Reihenfolge ist geräteabhängig

Geräte können Reihenfolgen ändern.

Folgerungen:
- Ordnung darf nicht angenommen werden.
- Explizite Ordnung kostet Zeit.
- Ordnung nur bei Notwendigkeit.

---

## 9. Fehler sind normal

I/O-Fehler entstehen aus Physik.

Folgerungen:
- Fehlerpfade sind gleichwertig.
- Wiederholung ist begrenzt.
- Endlose Retries sind verboten.

---

## 10. Bandbreite ist geteilt

Geräte teilen sich Busse und Controller.

Folgerungen:
- Konkurrenz ist einzukalkulieren.
- Exklusive Annahmen sind verboten.
- Überlast ist sichtbar zu machen.

---

## 11. Initialisierung ist I/O

Geräteinitialisierung kostet Zeit und Energie.

Folgerungen:
- Initialisierung ist zu budgetieren.
- Späte Initialisierung ist riskant.
- On-Demand ohne Grenze ist verboten.

---

## 12. Messung erfolgt am Gerät

I/O-Messung muss gerätenah sein.

Folgerungen:
- Host-Sicht allein ist unzureichend.
- Wartezeiten sind zu messen.
- Worst-Case dominiert.

---

## 13. Architekturbindung

CommanderLink bindet sich an:

- reale Busse und Controller
- bekannte DMA- und Interrupt-Modelle
- messbare Gerätepfade

Abweichungen erfordern neue Gesetze.

---

## 14. Schlussformel

I/O ist die langsamste Wahrheit im System.

Wer I/O unterschätzt,
zerstört Zeit, Energie und Stabilität.

Dieses Gesetz ist Grundlage aller
Geräte-, DMA- und Transferverträge.


# LAW_04_TIME.md
## CommanderLink – Zeit-Gesetz (Determinismus & Ablauf)

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Geltungsbereich

Dieses Gesetz regelt den Umgang mit Zeit,
Abläufen und zeitlichen Abhängigkeiten
für CommanderLink auf **x86_64**.

Zeit ist eine physikalische Größe,
kein Abstraktionsmittel.

---

## 1. Zeit ist irreversibel

Zeit verläuft nur vorwärts.

Folgerungen:
- Rückwärtskorrekturen sind verboten.
- Nachträgliche „Reparaturen“ sind physikalisch unmöglich.
- Entscheidungen müssen zum Entscheidungszeitpunkt korrekt sein.

---

## 2. Zeit ist lokal

Zeit entsteht am Ausführungsort.

Folgerungen:
- Lokale Zeitquellen haben Vorrang.
- Globale Zeit ist ein abgeleiteter Zustand.
- Synchronisation erzeugt Verzögerung.

---

## 3. Zeitmessung ist störend

Jede Zeitmessung greift in das System ein.

Folgerungen:
- Zeitmessung ist zu budgetieren.
- Übermäßige Messung verfälscht Ergebnisse.
- Messpfade dürfen Hot-Pfade nicht destabilisieren.

---

## 4. Worst-Case dominiert

Zeitliche Planung richtet sich nach dem Worst-Case.

Folgerungen:
- Durchschnittswerte sind informativ, nicht bindend.
- P99/P999 sind Mindestanforderungen.
- Ungebundene Latenzen sind verboten.

---

## 5. Jitter ist ein Fehler

Zeitliche Schwankung ist Instabilität.

Folgerungen:
- Jitter ist zu minimieren, nicht zu akzeptieren.
- Glättung ist Pflicht.
- Burst-Verhalten erzeugt Jitter.

---

## 6. Deadlines sind physikalische Grenzen

Eine Deadline ist eine reale Grenze.

Folgerungen:
- Verpasste Deadlines sind Fehlerzustände.
- Spätes Ergebnis ist kein Ergebnis.
- Deadline-Überschreitungen sind sichtbar zu machen.

---

## 7. Blockierung ist Zeitvernichtung

Blockierende Operationen vernichten Zeit.

Folgerungen:
- Ungebundene Blockierung ist verboten.
- Warten ohne obere Grenze ist verboten.
- Fortschritt muss messbar sein.

---

## 8. Planung ersetzt Hoffnung

Zeitplanung ist verpflichtend.

Folgerungen:
- Jeder Ablauf benötigt ein Zeitbudget.
- Überziehung ist ein Vertragsbruch.
- Elastizität ist explizit zu definieren.

---

## 9. Asynchronität ist nicht kostenlos

Asynchronität verlagert Zeit, sie eliminiert sie nicht.

Folgerungen:
- Asynchrone Pfade benötigen Kontrolle.
- Entkopplung ohne Rückmeldung ist verboten.
- Jeder asynchrone Pfad braucht Backpressure.

---

## 10. Zeitdomänen sind getrennt

Unterschiedliche Zeitdomänen dürfen nicht vermischt werden.

Folgerungen:
- CPU-Zeit ≠ Netzwerkzeit ≠ IO-Zeit.
- Übersetzung kostet Zeit.
- Domänenwechsel sind explizit zu markieren.

---

## 11. Zeitbasierte Steuerung schlägt Ereignisreaktion

Zeitsteuerung ist stabiler als reine Ereignissteuerung.

Folgerungen:
- Periodische Kontrolle ist Pflicht.
- Ereignisstürme sind instabil.
- Reaktion ohne Zeitkontext ist verboten.

---

## 12. Messfehler sind Zeitfehler

Unpräzise Zeitmessung erzeugt falsche Entscheidungen.

Folgerungen:
- Zeitauflösung muss bekannt sein.
- Drift ist zu erkennen.
- Ungenaue Zeit ist ein Risiko.

---

## 13. Architekturbindung

CommanderLink bindet sich an:

- x86_64 Zeitquellen
- deterministische Zeitpfade
- bekannte Takt- und Drift-Eigenschaften

Andere Architekturen benötigen eigene Zeitgesetze.

---

## 14. Schlussformel

Zeit ist der härteste Zwang im System.

Ein System, das Zeit nicht kontrolliert,
wird von ihr kontrolliert.

Dieses Gesetz ist Grundlage aller
Scheduler-, Pacing- und Ablaufverträge.

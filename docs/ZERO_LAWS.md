# CommanderLink – ZERO LAWS
## Endlos gültig · bitgenau · physics-first · ZERO-Abweichung

---

## ZERO_LAW_00_PHYSICS_ABSOLUTE

Physik ist die einzige Autorität.

Elektrische, thermische, zeitliche und speicherphysikalische Realität
haben immer Vorrang vor Spezifikationen, Konventionen,
Implementationen oder Meinungen.

---

## ZERO_LAW_01_ZERO_DEVIATION

Abweichungen sind unzulässig.

Es existiert kein Sonderfall, kein Debug-Modus,
keine Übergangsregel und kein späterer Fix.

Wenn Implementierung und Gesetz widersprechen,
ist die Implementierung falsch.

---

## ZERO_LAW_02_SHM_SINGLE_SOURCE_OF_TRUTH

Shared Memory ist die einzige Wahrheit.

Alle Zustände existieren ausschließlich in SHM.
Logs, Events, UI-Zustände oder Heuristiken haben keinen Wahrheitsanspruch.

---

## ZERO_LAW_03_SHADOW_TO_ACTIVE_CORE_ONLY

Der Übergang von Shadow nach Active darf ausschließlich im CORE erfolgen.

Der Übergang ist atomar.
Kein anderes Modul darf Active schreiben
oder Shadow interpretieren oder reparieren.

---

## ZERO_LAW_04_ROLE_PURITY

Rollen sind strikt getrennt und unverletzlich:

- HAL misst und publiziert Shadow.
- CORE entscheidet und setzt Wahrheit.
- FLOW leitet ab und erzeugt Intent.
- LINK führt aus und entscheidet nicht.
- ORACLE liest und empfiehlt.
- MONITOR liest passiv.

Rollenvermischung ist illegal.

---

## ZERO_LAW_05_ZERO_TRANSFORMATION

Im Kern von CommanderLink sind Transformationen verboten.

Verboten sind:
- implizite Kopien
- semantische Rewrites
- implizite Bedeutungsänderungen
- versteckte Normalisierungen

Zulässig sind ausschließlich explizit vertraglich definierte,
auditierbare Operationen unter Gate-Kontrolle.

---

## ZERO_LAW_06_MINIMUM_IS_MAXIMUM

Der Mindestbetrieb ist vollwertig.

Es existiert kein „Degraded Mode“.
Optimierungen sind Skalierungen,
keine funktionalen Rettungsmechanismen.

---

## ZERO_LAW_07_MINIMUM_TRANSFER_MODE

Der Mindestübertragungsmodus ist:

Ein CL-semantischer, credit- und pacing-gesteuerter Transport
über UDP ab Sekunde 0.

TCP ist kein Mindestmodus.
TCP ist ausschließlich Transport-Fallback.

CommanderLink ersetzt TCP ab dem ersten Paket.

---

## ZERO_LAW_08_SENDER_WAITS

Der Sender wartet.

Senden ohne Credits, ohne Drain-Rate
oder außerhalb deterministischen Pacings
ist verboten.

Push-basierte Übertragung ist unzulässig.

---

## ZERO_LAW_09_NO_IMPLICIT_HEURISTICS

Implizite Heuristiken sind verboten.

Alle Entscheidungen sind:
- tabellengetrieben
- zustandsbasiert
- worst-case-begrenzt
- SHM-sichtbar

---

## ZERO_LAW_10_WORST_CASE_BOUNDEDNESS

Jeder Pfad ist worst-case-begrenzt.

Unbounded Loops, implizite Backoffs,
opportunistische Entscheidungen
und adaptive Logik ohne harte Grenzen
sind verboten.

---

## ZERO_LAW_11_DEFAULT_DENY_TRUST

Join und Trust sind Default-Deny.

Ohne TRUSTED_ACTIVE existieren:
- keine Overlay-Routen
- keine Grants
- keine Data-Plane

Fail-Open betrifft Betrieb, nicht Berechtigung.

---

## ZERO_LAW_12_WATCHDOG_MANDATORY

Ein Watchdog ist verpflichtend.

Invariantverletzungen, Hänger
oder Vertragsbrüche müssen
deterministisch erkannt und behandelt werden.

---

## ZERO_LAW_13_ABI_AND_LAYOUT_IMMUTABLE

ABI- und SHM-Layouts sind unveränderlich.

- Cacheline-Union-Prinzip
- feste Strides
- kein `packed`
- explizites Padding
- statische Verifikation

---

## ZERO_LAW_14_ELASTIC_BULK_WITH_MINIMUM

Bulk-SHM ist elastisch und jederzeit reclaimable.

- Core-SHM ist immer resident und minimal funktionsfähig.
- Bulk-SHM darf bis zum maximal verfügbaren RAM wachsen.
- Bulk-SHM darf jederzeit bis auf 0 verschwinden.

Bulk-Abwesenheit ist semantisch RECLAIMED.

---

## ZERO_LAW_15_LIVE_REEVALUATION

CommanderLink skaliert permanent live.

Jeder Tick ist eine Neuberechnung
der physikalisch zulässigen Realität.

Es existiert kein stabiler Zustand,
nur ein aktuell erlaubter.

---

## ZERO_LAW_16_AUDITABILITY_TO_LAST_BIT

Jedes Bit ist entweder:
- semantisch definiert
- explizit reserviert

Undefinierte Bytes sind verboten.

Der Monitor zeigt Wahrheit, nicht Interpretation.

---

## ZERO_LAW_17_FINALITY

Diese Gesetze sind endlos gültig.

Sie stehen über Implementierungen,
Tools, Betriebssystemen und Meinungen.

CommanderLink ist ZERO oder nicht CommanderLink.


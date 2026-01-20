# CommanderLink – KANONISCHE GESETZE (aus diesem Gespräch)
## Stand: konsolidiert, ZERO-Abweichung, physics-first

---

## LAW_00_PHYSICS_FIRST

Physik ist die oberste Autorität.

Wenn elektrische, thermische, zeitliche oder speicherphysikalische Realität
einer Spezifikation, Konvention oder Implementationsannahme widerspricht,
gilt immer die Physik.

Spezifikationen sind Untergrenzen, niemals Designziele.

---

## LAW_01_WORST_CASE_BOUNDEDNESS

Jeder Pfad in CommanderLink muss worst-case-begrenzt sein.

Unbounded Loops, implizite Backoffs, adaptive Heuristiken ohne harte Grenzen
sind verboten.

Worst-Case-Stabilität hat Vorrang vor Durchschnittsperformance.

---

## LAW_02_SHM_IS_TRUTH

Shared Memory ist die einzige Wahrheit.

Alle Zustände existieren ausschließlich in SHM.
Logs, Events oder UI-Zustände haben keinen Wahrheitsanspruch.

---

## LAW_03_SHADOW_TO_ACTIVE_CORE_ONLY

Der Übergang von Shadow nach Active darf ausschließlich im CORE erfolgen.

Der Übergang ist atomar.
Kein anderes Modul darf Active schreiben oder Shadow „reparieren“.

---

## LAW_04_ROLE_PURITY

Rollen sind strikt getrennt:

- HAL misst und publiziert Shadow.
- CORE entscheidet und setzt Wahrheit.
- FLOW leitet ab und erzeugt Intent.
- LINK führt aus und entscheidet nicht.
- ORACLE liest und empfiehlt.
- MONITOR liest passiv.

Jede Rollenverletzung ist illegal.

---

## LAW_05_ZERO_TRANSFORMATION

Im Kern von CommanderLink sind Transformationen verboten.

Verboten sind:
- implizite Kopien
- semantische Rewrites
- implizite Format- oder Bedeutungsänderungen

Zulässig sind ausschließlich explizit vertraglich definierte Operationen
(CRC, FEC, Kompression), vollständig auditierbar und gate-gebunden.

---

## LAW_06_MINIMUM_IS_MAXIMUM

Der Mindestbetrieb von CommanderLink ist vollwertig.

„Degraded Mode“ existiert nicht.
Optimierungen sind Skalierungen, keine Rettungsmechanismen.

---

## LAW_07_MINIMUM_TRANSFER_MODE

Der Mindestübertragungsmodus von CommanderLink ist:

Ein CL-semantischer, credit- und pacing-gesteuerter Transport über UDP
ab Sekunde 0.

TCP ist kein Mindestmodus.
TCP ist ausschließlich ein Transport-Fallback, niemals Semantikträger.

CommanderLink ersetzt TCP ab dem ersten Paket.

---

## LAW_08_SENDER_WAITS

Der Sender wartet.

Senden ohne Credits, ohne Drain-Rate oder außerhalb von Pacing-Quanten
ist verboten.

Push-basierte Übertragung ist unzulässig.

---

## LAW_09_EXPLICIT_FLOW_CONTROL

Flow-Control ist explizit, sichtbar und deterministisch.

Credits, Inflight-Budgets, Burst-Caps und Rate-Ziele sind
tabellengetrieben und SHM-sichtbar.

Implizite Kernel- oder Protokollheuristiken sind unzulässig.

---

## LAW_10_AUTOMATION_WITHOUT_HEURISTICS

Automatik bedeutet deterministische Ableitung, nicht adaptive Heuristik.

Autopath, Autopacing, AutoMTU, AutoIntegrity und Autoscale
sind endliche Zustandsautomaten mit festen Tabellen und Hysterese.

LINK darf niemals autonom entscheiden.

---

## LAW_11_DEFAULT_DENY_TRUST

Join und Trust sind Default-Deny.

Ohne expliziten TRUSTED_ACTIVE-Zustand existieren:
- keine Overlay-Routen
- keine Grants
- keine Data-Plane

Fail-Open betrifft Betrieb, nicht Berechtigung.

---

## LAW_12_WATCHDOG_MANDATORY

Ein Watchdog ist verpflichtend.

Verletzungen von Invarianten, Hänger oder Vertragsbrüche
müssen deterministisch erkannt und behandelt werden.

---

## LAW_13_ABI_AND_LAYOUT_IMMUTABILITY

ABI- und SHM-Layouts sind deterministisch und unverhandelbar.

- Cacheline-Union-Prinzip
- feste Strides (256/512/1024/4096)
- kein `packed`
- explizites Padding
- Static Asserts überall

---

## LAW_14_ELASTIC_BULK_SHM

Bulk-SHM ist elastisch und jederzeit reclaimable.

- Core-SHM ist immer resident und minimal funktionsfähig.
- Bulk-SHM darf bis zum maximal verfügbaren RAM wachsen.
- Bulk-SHM darf jederzeit bis auf 0 verschwinden.

Bulk-Präsenz ist ausschließlich über `bulk_present` und `bulk_epoch`
definiert.

Bulk-Abwesenheit ist semantisch RECLAIMED, nicht „leer“.

---

## LAW_15_LIVE_REEVALUATION

CommanderLink skaliert permanent live.

Jeder Tick ist eine Neuberechnung der physikalisch zulässigen Realität.

Es existiert kein stabiler Betriebszustand,
nur ein aktuell erlaubter Zustand.

---

## LAW_16_AUDITABILITY_TO_LAST_BIT

Jedes Bit ist entweder:
- semantisch definiert
- explizit reserviert

Undefinierte Bytes sind verboten.

Der Monitor zeigt Wahrheit, nicht Interpretation.

---

## LAW_17_ZERO_DEVIATION

Abweichungen von diesen Gesetzen sind unzulässig.

Wenn Implementierung und Gesetz widersprechen,
ist die Implementierung falsch.

Diese Gesetze sind endlos gültig.


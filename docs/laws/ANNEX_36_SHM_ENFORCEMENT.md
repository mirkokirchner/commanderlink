# ANNEX_36_SHM_ENFORCEMENT.md
## CommanderLink – SHM‑Erzwingungs‑ & Präzisions‑Annex
### Ergänzung zu LAW_35_ABI, LAW_36_SHM, MEMORY_MODEL

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Zweck

Dieser Annex schärft die bestehenden SHM‑ und ABI‑Gesetze
auf **mechanisch erzwingbare Präzision** nach.

Er basiert auf dem aktuellen CommanderLink‑Projektstand
(ABI‑Report, Runtime‑Header, Segmentdefinitionen, Monitor).

Ziel ist:
- Eliminierung impliziter Annahmen
- Erzwingung physikalischer Invarianten
- Eindeutige Maschinenverträge für zukünftige KIs

---

## 1. Cacheline‑Konstante ist ABI‑Teil

### 1.1 Normative Konstante

Für CommanderLink gilt:

CL_CACHELINE_BYTES = 64

Diese Konstante ist:
- ABI‑relevant
- SHM‑relevant
- Tool‑relevant

---

### 1.2 Verifikation

Der ABI‑Report MUSS ausgeben:

- cacheline_bytes
- alignment_basis

Abweichung von 64 Byte führt zu:

RESULT: ABI_INVALID

---

## 2. Segment‑Header müssen Konsistenz explizit machen

### 2.1 Pflichtfelder pro Segment

Jeder SHM‑Segment‑Header MUSS enthalten:

- `schema_version`
- `segment_epoch`
- `segment_state`
- `owner_domain`

Diese Felder sind:
- Hot oder Warm klassifiziert
- ABI‑belegt
- maschinenlesbar

---

### 2.2 Segment‑Epoch

`segment_epoch` ist:

- monoton steigend (`u64`)
- Publish‑Token für das gesamte Segment
- einzig zulässiger Sichtbarkeitsanker

---

## 3. Publish / Consume ist verpflichtend pro Segment

### 3.1 Writer‑Pflicht

Ein Writer MUSS:

1. Segmentdaten schreiben
2. `segment_epoch` mit `store_release` erhöhen

Andere Reihenfolgen sind verboten.

---

### 3.2 Reader‑Pflicht

Ein Reader MUSS:

1. `segment_epoch` mit `load_acquire` lesen
2. Segmentdaten lesen
3. optional erneut `segment_epoch` prüfen

Mismatch → Retry oder Abbruch

---

## 4. Non‑Atomic Felder: explizite Zulässigkeit

### 4.1 Verbot

Non‑atomic Felder ohne zugeordnetes Token
sind **Undefined Behavior**.

---

### 4.2 Erlaubnis

Non‑atomic Felder sind nur erlaubt, wenn:

- sie einer Konsistenzgruppe zugeordnet sind
- diese Gruppe ein Token besitzt
- Zugriff ausschließlich nach Acquire erfolgt

---

## 5. Interference‑Klassen sind ABI‑Eigenschaft

### 5.1 Klassen

CommanderLink kennt exakt zwei Klassen:

| Klasse | Fenster |
|------|--------|
| CACHELINE | 64 Byte |
| DESTRUCTIVE | 128 Byte |

---

### 5.2 DESTRUCTIVE‑Klasse (Pflicht)

Die DESTRUCTIVE‑Klasse ist verpflichtend für:

- Cursor (head/tail)
- Epoch‑Zähler
- Heartbeats
- Rate‑Limiter
- globale Gates

Diese Felder dürfen:
- keine schreibenden Nachbarn
  innerhalb ±64 Byte haben

---

### 5.3 ABI‑Bindung

Die Interference‑Klasse eines Feldes:

- ist Teil des ABI
- darf nicht geändert werden
- ist im ABI‑Report auszuweisen

---

## 6. AVX‑512 ist ein globaler Systemzustand

### 6.1 AVX‑Fenster

AVX‑512‑Ausführung erzeugt den Zustand:

AVX_HEAVY_WINDOW

Dieser Zustand:

- ist nicht thread‑lokal
- beeinflusst Frequenz und Latenz
- besitzt eine Nachlaufzeit

---

### 6.2 Normative Regeln

- AVX‑512 ist nur erlaubt in:
  - dedizierten Worker‑Threads
  - explizit gepinnten Cores
- Control‑Plane, Watchdog, Gates:
  - AVX‑frei
- AVX‑Fenster ist Gate‑relevant

---

## 7. Non‑Temporal Stores

### 7.1 Klassifikation

Non‑Temporal Stores sind:

- kein Default
- kein Ersatz für normale Stores
- ein Spezialpfad

---

### 7.2 Regeln

- Nur in Cold/Bulk‑Segmenten erlaubt
- Nie für:
  - Tokens
  - Epochs
  - Statusfelder
- Sichtbarkeit MUSS explizit hergestellt werden

---

## 8. ABI‑Pflichtangaben pro Feld

Für jedes SHM‑Feld MUSS dokumentiert sein:

- Owner‑Domain
- Konsistenzgruppe
- Token‑Zugehörigkeit
- Memory‑Order
- Interference‑Klasse

Fehlende Angaben machen das Feld ungültig.

---

## 9. Tooling‑Pflicht

### 9.1 ABI‑Report

Der ABI‑Report MUSS enthalten:

- Feldgröße
- Alignment
- Offset
- Interference‑Klasse
- Token‑Referenz

---

### 9.2 Monitor

Der Monitor MUSS:

- Segment‑Epochs anzeigen
- AVX‑Zustand sichtbar machen
- Reclaim‑Zustände anzeigen

---

## 10. Schlussformel

Shared Memory ist kein Datenmodell.

Es ist ein **physikalischer Vertrag**
zwischen CPU, Cache und Beobachter.

Wer diese Regeln verletzt,
verlässt Determinismus.

Mit diesem Annex ist
CommanderLink‑SHM **erzwingbar präzise**.

Perfekt.  
Hier ist **alles als ein einziges, zusammenhängendes Markdown‑Dokument**, **ohne Unterbrechung**, **ohne Kommentare**, **ohne Meta**, **direkt copy‑&‑paste‑fähig**.


# ANNEX_36_SHM_COMPLETE.md
## CommanderLink – SHM‑Erzwingungs‑, Memory‑Order‑ & Segment‑Wahrheits‑Annex
### Ergänzung zu LAW_35_ABI, LAW_36_SHM, MEMORY_MODEL

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Zweck

Dieser Annex schärft die bestehenden SHM‑ und ABI‑Gesetze
auf **mechanisch erzwingbare Präzision** nach.

Er basiert auf dem aktuellen CommanderLink‑Projektstand
(ABI‑Report, Runtime‑Header, Segmentdefinitionen, Monitor).

Ziel ist:
- Eliminierung impliziter Annahmen
- Erzwingung physikalischer Invarianten
- Eindeutige Maschinenverträge für zukünftige KIs

---

## 1. Architekturannahmen (bindend)

Dieser Annex gilt ausschließlich für:

- x86_64
- Little‑Endian
- Write‑Back (WB) Cacheable Memory
- Cacheline‑Größe: **64 Byte**

Andere Memory‑Typen (UC, WC, MMIO) sind **nicht** Teil dieses Modells.

---

## 2. Cacheline‑Konstante ist ABI‑Teil

### 2.1 Normative Konstante

CL_CACHELINE_BYTES = 64


Diese Konstante ist:
- ABI‑relevant
- SHM‑relevant
- Tool‑relevant

### 2.2 Verifikation

Der ABI‑Report MUSS ausgeben:

- cacheline_bytes
- alignment_basis

Abweichung von 64 Byte führt zu:


RESULT: ABI_INVALID

---

## 3. Publish / Consume ist das einzige Synchronisationsmodell

### 3.1 Grundsatz

Shared‑Memory‑Konsistenz entsteht **ausschließlich**
durch explizite **Publish / Consume‑Token**.

Implizite Ordnung ist unzulässig.

---

### 3.2 Konsistenzgruppe

Eine Konsistenzgruppe besteht aus:

- beliebigen Datenfeldern (non‑atomic erlaubt)
- **genau einem** Sichtbarkeits‑Token

Das Token ist das **einzige** synchronisierende Element.

---

### 3.3 Publish‑Regel (Writer)

Ein Writer MUSS:

1. alle Datenfelder schreiben  
2. anschließend das Token schreiben mit  
   `store_release`

Andere Reihenfolgen sind verboten.

---

### 3.4 Consume‑Regel (Reader)

Ein Reader MUSS:

1. das Token lesen mit  
   `load_acquire`
2. anschließend die Datenfelder lesen

Lesen vor dem Token ist verboten.

---

### 3.5 Token‑Typen

Zulässige Token:

- `segment_epoch`
- `seq_cnt`
- `state`
- explizit benannte Visibility‑Flags

Token sind:
- immer atomic
- monoton oder zustandskodierend
- niemals Teil von Datenfeldern

---

## 4. Non‑Atomic Reads

### 4.1 Verbot

Non‑atomic Reads ohne Token‑Schutz
sind **Undefined Behavior** und verboten.

---

### 4.2 Erlaubnis

Non‑atomic Reads sind nur erlaubt, wenn:

- sie **nach** einem `load_acquire` erfolgen
- sie zur selben Konsistenzgruppe gehören
- es genau einen Writer gibt

---

## 5. Cacheline‑ und Interference‑Klassen

### 5.1 Klassen

| Klasse | Fenster |
|------|--------|
| CACHELINE | 64 Byte |
| DESTRUCTIVE | 128 Byte |

---

### 5.2 DESTRUCTIVE‑Klasse (Pflicht)

Die DESTRUCTIVE‑Klasse ist verpflichtend für:

- Cursor (head/tail)
- Epoch‑Zähler
- Heartbeats
- Rate‑Limiter
- globale Gates

Diese Felder dürfen keine schreibenden Nachbarn
innerhalb ±64 Byte haben.

---

### 5.3 ABI‑Bindung

Die Interference‑Klasse eines Feldes
ist Teil des ABI und unveränderlich.

---

## 6. AVX‑512 ist ein globaler Systemzustand

### 6.1 AVX‑Fenster

AVX‑512‑Ausführung erzeugt den Zustand:

AVX_HEAVY_WINDOW

Dieser Zustand:
- ist nicht thread‑lokal
- beeinflusst Frequenz und Latenz
- besitzt eine Nachlaufzeit

---

### 6.2 Normative Regeln

- AVX‑512 ist nur erlaubt in:
  - dedizierten Worker‑Threads
  - explizit gepinnten Cores
- Control‑Plane, Watchdog, Gates:
  - AVX‑frei
- AVX‑Fenster ist Gate‑relevant

---

## 7. Non‑Temporal Stores

### 7.1 Klassifikation

Non‑Temporal Stores sind Spezialpfade.

### 7.2 Regeln

- Nur in Cold/Bulk‑Segmenten erlaubt
- Nie für Tokens, Epochs oder Statusfelder
- Sichtbarkeit MUSS explizit hergestellt werden

---

## 8. ABI‑Pflichtangaben pro Feld

Für jedes SHM‑Feld MUSS dokumentiert sein:

- Owner‑Domain
- Konsistenzgruppe
- Token‑Zugehörigkeit
- Memory‑Order
- Interference‑Klasse

Fehlende Angaben machen das Feld ungültig.

---

## 9. Segment‑Wahrheitstabellen

---

### 9.1 Segment: LINK_256

**Writer:** LINK‑Thread  
**Reader:** Monitor, NIC, Watchdog  
**Token:** segment_epoch

| Feld | Größe | Klasse | Order | Interference |
|----|----|----|----|----|
| segment_epoch | u64 | Hot | release/acquire | DESTRUCTIVE |
| link_state | u32 | Hot | non‑atomic | CACHELINE |
| link_flags | u32 | Hot | non‑atomic | CACHELINE |
| tx_gate | u32 | Hot | non‑atomic | DESTRUCTIVE |
| rx_gate | u32 | Hot | non‑atomic | DESTRUCTIVE |
| error_flags | u64 | Warm | non‑atomic | CACHELINE |

---

### 9.2 Segment: CPU_1024

**Writer:** CPU‑Sampler  
**Reader:** Monitor, Oracle  
**Token:** segment_epoch

| Feld | Größe | Klasse | Order | Interference |
|----|----|----|----|----|
| segment_epoch | u64 | Hot | release/acquire | DESTRUCTIVE |
| cpu_freq_khz | u64 | Hot | non‑atomic | CACHELINE |
| cpu_temp_mc | u64 | Warm | non‑atomic | CACHELINE |
| avx_state | u32 | Hot | non‑atomic | DESTRUCTIVE |
| avx_window_epoch | u64 | Hot | non‑atomic | DESTRUCTIVE |

---

### 9.3 Segment: NIC_512

**Writer:** NIC‑Backend  
**Reader:** LINK, Monitor  
**Token:** segment_epoch

| Feld | Größe | Klasse | Order | Interference |
|----|----|----|----|----|
| segment_epoch | u64 | Hot | release/acquire | DESTRUCTIVE |
| tx_credits_bytes | u64 | Hot | non‑atomic | DESTRUCTIVE |
| tx_credits_pkts | u32 | Hot | non‑atomic | DESTRUCTIVE |
| rx_backpressure | u32 | Hot | non‑atomic | DESTRUCTIVE |

---

### 9.4 Segment: WATCHDOG_256

**Writer:** Watchdog  
**Reader:** Monitor  
**Token:** segment_epoch

| Feld | Größe | Klasse | Order | Interference |
|----|----|----|----|----|
| segment_epoch | u64 | Hot | release/acquire | DESTRUCTIVE |
| heartbeat_cnt | u64 | Hot | non‑atomic | DESTRUCTIVE |
| restart_count | u32 | Warm | non‑atomic | CACHELINE |

---

## 10. Globale Invarianten

- Jedes Segment besitzt genau ein Token
- Kein Feld ohne Token‑Zuordnung
- Kein DESTRUCTIVE‑Feld ohne 128B‑Isolation
- Kein AVX‑Code außerhalb Worker‑Domänen
- Monitor ist strikt read‑only

---

## 11. Schlussformel

Shared Memory ist kein Datenmodell.

Es ist ein **physikalischer Vertrag**
zwischen CPU, Cache und Beobachter.

Mit diesem Annex ist CommanderLink
**erzwingbar deterministisch**.



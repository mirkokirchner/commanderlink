# CommanderLink – Projekt‑Guardrails (Physik‑First, ABI‑First, Security‑First)

Dieses Dokument ist die **verbindliche Arbeitsgrundlage**. Es dient als „Definition of Done“ für Header, Runtime‑ABI und Monitoring.

## 0. Leitprinzip

**Maximum ist Minimum.**  
CL nutzt alle verfügbaren Ressourcen aggressiv **bis** ein Schutz‑Gate greift.  
Sobald ein Gate triggert: **innerhalb eines Ticks** sauber degradiert, nie experimentell.

## 1. Architektur‑Zerlegung (Schlüsseltechnologien)

- **HAL**: Hardware‑Erkennung, Sensor‑Sampling, Feature‑Probing (SSE4/AVX2/AVX‑512/AMX…), Thermik/VRM‑Signale (gefiltert).
- **CORE**: Zustandsautomaten (Cold‑Start, Join, Autopath), deterministische Entscheidungen.
- **LINK**: Transport‑Mechanik (BYPASS/UDP/TCP), MTU‑Negotiation, Grants/Credits.
- **FLOW**: Pacing, Queueing, Backpressure, Kompression/FEC‑Policy, „Shock‑Absorber Mode“ (RAM als Dämpfer).
- **ORACLE**: Bewertung, Empfehlungen, Prognosen (bottleneck inference, health scoring), aber **gedämpft** und explainable.
- **MONITOR**: Sichtbarkeit: *warum* sind wir wo wir sind (Reason/Constraints) + Hardware‑Inventar + Verlauf.
- **RUNTIME/SHM**: ABI‑stabile Wahrheitsschicht (Root/TOC/Segments).

## 2. ABI‑Vertrag für SHM (nicht verhandelbar)

### 2.1 Grundregeln
- **SHM ist Host‑Byteorder** (kein ntohl‑Salat im Monitor).
- **Kein `packed` im SHM.**  
  Stattdessen: explizites Padding + `_Static_assert(sizeof)==…`.
- **Fixe Segmentgrößen** (z. B. 256/512/1024/4096) und **Fix‑Alignment** (mind. 64B; Root 4096).
- **Union‑Normflächen** sind erlaubt/erwünscht:
  - `uint8_t raw[64]`, `raw[256]`, `raw[4096]` als ABI‑Anker
  - darüber strukturierte Sichten (Hot/Warm/Cold)
- **Keine 0‑Sized Arrays**, keine flex arrays in SHM‑Segmenten.
- **Append‑Only**: Felder werden nur hinten ergänzt; für Umbrüche: `schema_version` bump.

### 2.2 Atomics & Memory‑Order
- `_Atomic` nur dort, wo Reader/Writer parallel sind.
- Für jedes Atomic‑Feld: Kommentar, ob `relaxed` genügt oder `acquire/release` benötigt wird.
- `volatile` ist **kein** Synchronisationsersatz.

### 2.3 ABI‑Prüfpflicht
- Jede Änderung an `include/cl_runtime/*` muss durch:
  - `cl_abi_report` (Alignof/sizeof/offsets)
  - `cl_shm_dump` (TOC‑Kohärenz)
  - `make monitor` (Header‑Konsistenz)

## 3. Wire‑Vertrag (OOB / Data‑Plane)

### 3.1 OOB Wire
- Wire‑Structs sind **packed**.
- Zahlenfelder sind **explizit little‑endian** (`*_le`).
- Größen müssen exakt passen: `hdr32`, `beacon64`, `hello128`, `grant128` via `_Static_assert`.

### 3.2 Zero‑Trust: „Beacon ist untrusted“
- **Beacon** dient nur der Sichtbarkeit („seen“) und grobem Capability‑Hinweis.
- **Keine** Autorisierung anhand Beacon.
- Join/Trust entsteht erst via **Hello/Grant** mit Auth‑Tag + Anti‑Replay.

## 4. Join Policy (Security‑First, paranoid)

### 4.1 Ziel
- Kein Fremdgerät darf „aus Versehen“ Partner werden, nur weil es im selben L2/L3 auftaucht.
- Default: **deny**. Allow nur, wenn Join‑Policy es gestattet.

### 4.2 Mindestmechaniken
- `mesh_tag64` als Domain‑Trennung (mehrere Meshes im selben LAN).
- `node_tag64` als Identität.
- Anti‑Replay: monotones `epoch` pro Sender/Peer.
- Auth‑Tag (truncated) für Hello/Grant.
- Rate‑Limit/Backoff: gegen Flood/Join‑Storm.
- Fail‑Safe: bei Unsicherheit → `PASSIVE_MONITORING` oder `OOB_ONLY`.

### 4.3 Praxis‑Szenario: „10G + 1G Mischverkabelung“
- Peers können über unterschiedliche NICs sichtbar sein (10G MikroTik + 1G Fritz‑LAN).
- Join muss explizit entscheiden:
  - **welcher** physische Pfad (NIC/Route) ist „data eligible“
  - welche Pfade sind nur OOB/Discovery
- Ergebnis: kein „Bridging“, kein Default‑Route‑Hack.

## 5. Autopath & Fallback (Stabilität vor Turbo)

- Baseline immer möglich: **UDP**.
- Upgrade auf BYPASS nur nach Probe + Health‑Check + Hysterese.
- Sofort‑Downgrade bei Fehlern (Penalty Box).
- TCP ist Rettungsboot, nicht Standard.

## 6. Sensorik/VRM/Thermik (Reality‑Filter)

- VRM/Board/Power‑Sensoren sind oft noisy.  
  → Sie dürfen **nicht** das Micro‑Pacing im Fast‑Path steuern.
- Regel: VRM/Power ist **Safety Cutoff** mit Hysterese/Glättung (Trend‑Filter).
- Wenn kritisch: sofort dämpfen (Constraints setzen), aber kein Zickzack.

## 7. „Maximum‑Minimum“ Performance‑Policy

- Kompression/FEC/Vector‑Pfad dürfen hochskalieren, wenn:
  - Bottleneck == Netzwerk
  - CPU Budget verfügbar
  - Thermik nicht kritisch
  - User‑Interaktivität nicht leidet (Shadow/Reclaim)
- Wenn Bottleneck == Storage/Senke → Backpressure statt „mehr CPU“.

## 8. Watchdog (geplant / Erweiterung)

- Watchdog darf CL neu starten, weil Fail‑Open das OS stabil hält.
- Jeder Restart muss:
  - Grund/Reason/Constraints loggen (System‑Log‑Ordner)
  - im SHM „Forensics“ die letzten 5 Events führen
  - Monitor: Dashboard‑Status „Watchdog active“ + Historie

## 9. Code‑Stil (menschenlesbar, deutsch dokumentiert)

- Header‑Kommentare und Feld‑Kommentare **Deutsch**.
- Strukturen klar segmentieren (Hot/Warm/Cold).
- Keine „Workarounds“ die ABI verschleiern: jedes Padding ist bewusst und kommentiert.
- PRI‑Makros (`PRIu64` etc.) in Monitor‑Formatierung verwenden.

## 10. Definition of Done (für jede Änderung)

- Kompiliert auf Linux + FreeBSD/GhostBSD + OmniOS (mindestens Header‑/Tool‑Build).
- ABI‑Report: `RESULT: ABI_STABLE`.
- OOB‑Wire: `_Static_assert` für alle Message‑Sizes grün.
- Monitor liest ohne nt…‑Konvertierung und erklärt *warum* (Reason).


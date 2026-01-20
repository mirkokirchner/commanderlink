# CommanderLink ABI/Layout-Vertrag (FINAL)

Dieses Dokument ist die **normative ABI-Verfassung** für CommanderLink.  
Es definiert das Speicherlayout, die Größen, die Feldordnung und die Regeln, nach denen Änderungen zulässig sind.

CommanderLink ist **physikgetrieben**: Cacheline-Geometrie, Register-Breiten, deterministische Speicherbewegung.  
Portabilität ist Mittel zum Zweck, nicht Ziel. **Stabilität + Determinismus** sind Ziel.

---

## 1. Grundgesetze

### 1.1 Maximalmodell-Prinzip
- **Das Maximum ist das Minimum.**
- Abwesenheit ist **ein Zustand**, keine Lücke.
- Jedes Feature ist modelliert als:
  - `possible` (kann Hardware/OS prinzipiell?)
  - `effective` (wird aktuell genutzt?)
  - `state` (UNSUPPORTED/DISABLED/IDLE/ACTIVE/UNKNOWN/FAULT)
  - `reason` + `constraint_flags`
  - `source` + `quality`

### 1.2 Zero-Transformation (Standard)
- Im Hot-Path sind **Transformationen verboten**:
  - kein Shift/Rotate/Pack/Shuffle/Repack
  - kein hot memcpy
  - keine Endian-Konvertierung im Hot-Path
- Erlaubt ist nur:
  - `load → compare → store`
- Geometrie (Headroom/Alignment) wird **einmalig im Setup** festgelegt.

### 1.3 Endianness
- SHM ist **Host-Order** (keine ntohl/htonl-Kaskaden).
- Wire/OOB ist **explizit little-endian** (`*_le`) und wird an der Wire-Grenze konvertiert.
- Root enthält `endian_magic` zur Laufzeitprüfung.

### 1.4 Keine `packed`-Layouts im SHM
- `packed` ist in SHM-Segmenten verboten.
- SHM nutzt Cacheline-Unions (`raw[64]`) bzw. Page-Unions (`raw[4096]`).
- Jede Größe wird über `_Static_assert` hart abgesichert.

### 1.5 Atomics
- Atomics sind Sichtbarkeit, nicht Zähler-Müll:
  - **keine Atomics in inneren Paket-Loops**
  - Commit/Updates **pro Batch** (lokal aggregieren, dann 1 atomic add/store)
- Bei `_Atomic`-Felder ist Memory-Order im Code dokumentiert:
  - Hot: i.d.R. relaxed + Release beim Publish, Acquire beim Read
  - Monitor: Acquire

### 1.6 Alignment-Rounding (wichtiger Compiler-Fakt)
- **Wenn ein struct mit großem Alignment (z. B. 1024/4096) versehen wird, kann der Compiler die Gesamtgröße auf ein Vielfaches runden.**
- Darum: Records müssen exakt die Zielgröße treffen, sonst driftet das Layout (z. B. 1024→2048).
- Jede Zielgröße wird mit `_Static_assert(sizeof(...) == N)` erzwungen.

---

## 2. Runtime Kern: Root & TOC

### 2.1 Root (Core-SHM)
- Typ: `cl_root_4096_t` (alias `cl_root_t`)
- Größe: **4096 Bytes**
- Normfläche: `raw[4096]`
- Enthält:
  - Schema-/Endianness-Check (`schema_version`, `endian_magic`)
  - Identitäten (`mesh_tag64`, `node_tag64`)
  - Truth-States (`path_state`, `backend_active`, `mode_active`, `profile_active`)
  - Gates (`rx_gate`, `tx_gate`)
  - Constraints (`constraint_flags`, `reason_code`)
  - TOC pointer (`toc_offset`, `toc_size`)
  - Bulk presence (`bulk_present`, `bulk_epoch`)
- ABI: append-only innerhalb `raw[4096]`

**Runtime-Invariant:**
- Root ist die „Notfall-Wahrheit“. Kein Modul darf Root „interpretieren“, nur lesen/schreiben gemäß Owner-Regeln.

### 2.2 TOC (Table of Contents)
- Typ: `cl_toc_t`
- `cl_toc_entry_t` Größe: **64 Bytes** (Cacheline)
- `CL_TOC_MAX_SEGMENTS = 128`
- `sizeof(cl_toc_t) = 24 + 128*64 = 8216`

**TOC Entry Felder (host-order):**
- `type`, `flags`
- `offset_bytes`, `stride_bytes`, `count`
- `hot_off/hot_len`, `warm_off/warm_len`, `cold_off/cold_len`
- `upd_class`
- `src_tag`, `qual_tag`

**Invariants:**
- Entry ist exakt 64B.
- `offset_bytes` ist aligned zu `stride_bytes` (mindestens).
- Segmente werden niemals „implizit“ gefunden. TOC ist die Wahrheit.

---

## 3. Segment-Geometrie (Strides)

### 3.1 Fixe Strides (Gesetz)
- 256B, 512B, 1024B, 4096B
- jedes Segmentrecord ist Vielfaches von 64B Cachelines
- `raw[64]` Unions als Normfläche pro Line
- `raw[4096]` als Normfläche pro Page-Segment (Bulk)

### 3.2 Segmentliste (aktueller Stand)
Control/Policy:
- `cl_seg_link_256_t`
- `cl_seg_budget_256_t`
- `cl_seg_time_256_t`
- `cl_seg_overlay_256_t`
- `cl_seg_board_512_t`

Inventory:
- `cl_seg_cpu_1024_t`
- `cl_seg_nic_512_t`
- `cl_seg_mem_256_t`
- `cl_seg_pcie_256_t`

Mesh:
- `cl_seg_mesh_neighbor_256_t`
- `cl_seg_mesh_peer_256_t`

Bulk/Meta:
- `cl_seg_dma_4096_t`
- `cl_seg_history_4096_t`
- `cl_seg_forensics_4096_t`
- `cl_seg_zfs_4096_t`

---

## 4. Source/Quality Pflicht

Jeder Segment-Record enthält (mindestens einmal):
- `src` (cl_source_t) und `qual` (cl_quality_t)

**Regel:**
- Fehlende Daten werden nicht „0“ – sondern:
  - Feature state = UNSUPPORTED/UNKNOWN/DISABLED
  - `src/qual` entsprechend setzen
  - `reason_code/constraint_flags` erklären, warum

---

## 5. String- und Herstellerdaten

### 5.1 Grundsatz
- Hersteller/Name/Serial sind Pflicht zur Transparenz (Monitor-Anforderung).
- Strings gehören in COLD (statisch), niemals in HOT.

### 5.2 64B-Line Disziplin
- Keine 64B Cold-Line darf durch Strings >64B gesprengt werden.
- Längere Labels werden über mehrere Cold-Lines verteilt (z. B. CPU cold_b/cold_c).
- Das ist kein Workaround, sondern Normflächenprinzip.

---

## 6. Build-/ABI-Checks (Pflicht)

### 6.1 Compile-Time
- Jeder Segmenttyp hat `_Static_assert(sizeof(...) == STRIDE)`
- `cl_root_4096_t` hat `_Static_assert(sizeof(...) == 4096)`
- `cl_toc_entry_t` hat `_Static_assert(sizeof(...) == 64)`
- Bei großen Alignments (1024/4096) ist size-assert zwingend.

### 6.2 Laufzeit
- `cl_shm_dump` muss Root + TOC lesen können, ohne UB
- Root `endian_magic` muss passen
- `toc_offset + sizeof(cl_toc_t)` muss im Mapping liegen

---

## 7. Änderungsregeln (Append-only)

### 7.1 Was ist erlaubt
- Neue Felder werden **nur in reservierte Bereiche** gesetzt oder in neue Lines/COLD-Erweiterungen verschoben.
- Segment-Strides bleiben gleich.
- TOC kann neue Segmente hinzufügen (bis 128).
- Schema-Version wird erhöht, wenn Semantik/Interpretation sich ändern.

### 7.2 Was ist verboten
- Reordering von bestehenden Feldern in einer Line
- Entfernen/Umbenennen von Feldern ohne Kompatibilitätsbrücke
- `packed` in SHM
- implizite Padding-Annahmen
- „nur für einen Compiler“ Anpassungen

---

## 8. Performance- und Energie-Regel (aggressiv, aber gated)

- CommanderLink darf **aggressiv** Ressourcen nutzen (CPU bis 100%, SIMD bis Maximum), wenn:
  - Budget nicht SHADOW/RECLAIM
  - Thermal/VRM/Power Gates offen
  - Security/Join Trust ok
- Drosselung ist **sofort** und **lokal** (nur betroffene Ressource/Link).

---

## 9. Tooling

- `cl_dummy_shm` erzeugt Core-SHM + TOC + Segment-Records deterministisch.
- `cl_shm_dump` ist Truth-Viewer zur ABI-Prüfung.
- Monitor2 wird ausschließlich aus TOC/Segmente lesen, nicht aus Legacy-Strukturen.

---

## 10. Checkliste für Review

Vor jeder Layout-Änderung:
1) Betrifft es SHM? → `packed` verboten
2) Bleibt jede Line 64B? → `_Static_assert`
3) Bleibt Segmentstride gleich? → `_Static_assert`
4) Bleibt Root 4096? → `_Static_assert`
5) Wird Source/Quality/State/Reason abgebildet? → Pflicht
6) Ist es append-only? → Pflicht
7) Laufen Dummy + Dump auf Linux/BSD/OmniOS? → Pflicht

---


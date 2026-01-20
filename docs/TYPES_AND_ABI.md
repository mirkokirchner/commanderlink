# CommanderLink – TYPES & ABI CONTRACT

## Zweck
Dieses Dokument definiert verbindlich,
welche C-Datentypen in CommanderLink erlaubt sind
und wo sie eingesetzt werden dürfen.

Es verhindert ABI-Drift, Padding-Fehler und Plattform-Inkonsistenzen.

---

## 1. Grundregel

> **Shared Memory und Wire sind heilig.**

Alles, was dort liegt, muss:
- exakt
- reproduzierbar
- ABI-stabil
- endian-klar
sein.

---

## 2. Erlaubte Typen in SHM & Wire

### 2.1 Integer

Erlaubt:
- `uint8_t`
- `uint16_t`
- `uint32_t`
- `uint64_t`
- `int16_t`, `int32_t`, `int64_t` (nur wenn nötig)

Nicht erlaubt:
- `int`
- `long`
- `size_t`
- `bool`
- `enum` ohne explizite Größe

---

### 2.2 Atomics

Erlaubt:
- `_Atomic(uint8_t)`
- `_Atomic(uint32_t)`
- `_Atomic(uint64_t)`

Verwendung:
- Counters
- Flags
- Epochs

Nicht erlaubt:
- Atomics auf Structs
- Atomics auf Arrays

---

### 2.3 Bitfelder

**Verboten.**

Bitfelder sind ABI- und Endian-abhängig.

Stattdessen:
- Bitmasken in `uint32_t` oder `uint64_t`

---

### 2.4 Floating Point

In SHM/Wire:
- **verboten**

Stattdessen:
- `*_permille`
- `*_ppm`
- `*_x10`, `*_x100`

---

## 3. Typen im Hot Path (intern)

Erlaubt:
- `uint_fast32_t`, `uint_fast64_t`
- `size_t` (nur lokal)
- `uintptr_t`
- SIMD-Typen (`__m128i`, `__m256i`, `__m512i`)

Nicht erlaubt:
- Speicherung dieser Typen im SHM/Wire

---

## 4. Endianness-Regeln

- SHM: **Host-Order**
- Wire: **Little-Endian** (`*_le`)

Konvertierung:
- Nur an der Wire-Grenze
- Niemals im Monitor

---

## 5. Alignment-Regeln

- Cacheline-Einheiten: exakt 64 Byte
- Root-Strukturen: 4096 Byte
- DMA-Segmente: 4096 Byte

Kein implizites Compiler-Padding erlaubt.

---

## 6. Schema-Versionierung

Jede SHM-Root-Struktur enthält:
- `schema_version`
- `endian_magic`

Neue Felder:
- nur in reservierten Bereichen
- Schema-Version erhöhen

---

## 7. ABI-Tests

Pflicht:
- `_Static_assert(sizeof(...))`
- `_Static_assert(_Alignof(...))`
- `cl_abi_report` grün

Ohne ABI-Grün kein Merge.

---

## 8. Merksatz

> **Alles, was nicht exakt ist, ist ein Bug.**


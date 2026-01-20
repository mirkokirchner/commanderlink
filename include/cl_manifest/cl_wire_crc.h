#pragma once
/* ============================================================================
 * CommanderLink – Wire Integrity (CRC8 Header + CRC32C Trailer) – FINAL
 * Datei: include/cl_manifest/cl_wire_crc.h
 * ============================================================================
 *
 * Ziel:
 *   - CPU soll rechnen, nicht verwalten.
 *   - Starke Integrität nutzt hardware-nahe Primitive, wenn verfügbar.
 *
 * Vertrag:
 *   1) Header Early-Reject:
 *      - CRC8 über CLWP Header (16B), hdr_crc8 Feld vorher 0.
 *
 *   2) Payload Strong Integrity:
 *      - CRC32C (Castagnoli) als Trailer (4B, little-endian)
 *      - Trailer ist nur vorhanden, wenn CL_WF_PAYLOAD_CRC32C gesetzt ist.
 *      - payload_len im Header zählt NUR den Payload (ohne Trailer).
 *      - Auf dem Draht folgt nach payload_len Bytes ggf. crc32c_le (4B).
 *
 *   3) Optional: Batch-CRC32C (High-throughput)
 *      - Wenn CL_WF_BATCH_CRC32C gesetzt ist, trägt der Frame eine Batch-CRC32C
 *        als Trailer (siehe cl_wire_batch_crc_tail16_t).
 *      - Nutzung: große Transfers, amortisierte Integrität.
 *
 * SIMD/HW:
 *   - Auf x86_64 kann CRC32C per SSE4.2 Instruktion gerechnet werden.
 *   - Die Nutzung ist capability-/gate-gebunden (possible/effective).
 * ============================================================================
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "cl_wire.h"

/* ----------------------------- CRC8 Vertrag --------------------------------
 *
 * CRC8 ist ausschließlich Early-Reject.
 * Polynom wird hier normativ festgelegt.
 *
 * CRC8-ATM (poly 0x07) ist klein, stabil und ausreichend als "quick check".
 * Starke Integrität ist CRC32C (Trailer).
 */
#define CL_CRC8_POLY_U8   0x07u
#define CL_CRC8_INIT_U8   0x00u
#define CL_CRC8_XOROUT_U8 0x00u

/* Berechnungs-API (Implementation in .c, nicht hier als Heuristik):
 *   - cl_crc8_hdr16() berechnet hdr_crc8 für cl_wire_hdr16_t (hdr_crc8=0).
 */
uint8_t cl_crc8_hdr16(const cl_wire_hdr16_t *h);

/* ---------------------------- CRC32C Trailer ------------------------------ */

/* CRC32C (Castagnoli) */
#define CL_CRC32C_INIT_U32   0xFFFFFFFFu
#define CL_CRC32C_XOROUT_U32 0xFFFFFFFFu

/* 4B Trailer – little-endian */
typedef struct __attribute__((packed)) cl_wire_crc32c_tail4 {
    uint32_t crc32c_le;
} cl_wire_crc32c_tail4_t;

_Static_assert(sizeof(cl_wire_crc32c_tail4_t) == 4, "crc32c tail muss 4B sein");

/* Optionaler Batch-CRC Trailer – 16B (konstant, maschinenfreundlich)
 *
 * Anwendung:
 *   - Ein Frame mit CL_WF_BATCH_CRC32C trägt am Ende dieses Trailer-Struct.
 *   - seq_end48 beschreibt den letzten seq im Batch (inkl.).
 *   - batch_id ist der Batch-Kontext.
 *   - crc32c schützt die aggregierte Batch-Payload (vertraglich definiert).
 *
 * Hinweis:
 *   - Diese 16B sind bewusst konstant (keine variablen Strukturen).
 */
typedef struct __attribute__((packed)) cl_wire_batch_crc_tail16 {
    uint16_t magic_le;      /* eigener Trailer-Magic */
    uint16_t batch_id_le;   /* passt zum Header batch_id */
    uint8_t  seq_end48_le[6];
    uint32_t crc32c_le;     /* CRC32C über vereinbarte Batch-Daten */
    uint16_t reserved0;     /* muss 0 sein */
} cl_wire_batch_crc_tail16_t;

#define CL_WIRE_BATCH_CRC_MAGIC_U16 0xC3C3u

_Static_assert(sizeof(cl_wire_batch_crc_tail16_t) == 16, "batch crc tail muss 16B sein");

/* --------------------- CRC32C: Hardware-naher Update-Pfad ------------------
 *
 * Diese Inlines sind rein mechanisch:
 *   - keine Policy, keine Autodetektion
 *   - nur: wenn Compiler+CPU SSE4.2 erlaubt, dann kann man _mm_crc32_* nutzen.
 *
 * Hinweis:
 *   - CPUID/Gate-Entscheidung gehört in HAL/CORE/FLOW (possible/effective),
 *     nicht in diesen Header.
 */
#if defined(__SSE4_2__)
#include <nmmintrin.h> /* _mm_crc32_u8/u64 */

static inline uint32_t cl_crc32c_sse42_u8(uint32_t crc, const uint8_t *p, size_t n) {
    /* crc ist laufender Zustand (init typ. CL_CRC32C_INIT_U32) */
    size_t i = 0;

    /* 8-byte chunks */
    while (i + 8 <= n) {
        uint64_t v;
        memcpy(&v, p + i, sizeof(v));
        crc = (uint32_t)_mm_crc32_u64((uint64_t)crc, v);
        i += 8;
    }

    /* tail */
    while (i < n) {
        crc = _mm_crc32_u8(crc, p[i]);
        i++;
    }

    return crc;
}

#endif /* __SSE4_2__ */

/* CRC32C API (Implementation in .c, policy-/gate-gebunden):
 *   - cl_crc32c_update() rechnet CRC32C über bytes
 *   - Implementation darf SSE4.2 nutzen, wenn possible/effective.
 */
uint32_t cl_crc32c_update(uint32_t crc, const void *data, size_t len);

/* -------------------------- Trailer-Verträge -------------------------------
 *
 * Payload CRC32C Trailer (CL_WF_PAYLOAD_CRC32C):
 *   wire frame layout:
 *     [cl_wire_hdr16_t][payload bytes (payload_len)][cl_wire_crc32c_tail4_t]
 *
 * Batch CRC32C Trailer (CL_WF_BATCH_CRC32C):
 *   wire frame layout:
 *     [cl_wire_hdr16_t][payload bytes (payload_len)][cl_wire_batch_crc_tail16_t]
 *
 * Achtung:
 *   - payload_len zählt NICHT den Trailer mit.
 *   - Receiver muss anhand flags den Trailer erwarten und prüfen.
 */


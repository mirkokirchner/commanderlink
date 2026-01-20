#pragma once
/* ============================================================================
 * CommanderLink – CLWP Wire Header (16B) – FINAL Endausbau
 * Datei: include/cl_manifest/cl_wire.h
 * ============================================================================
 *
 * Grundsatz:
 *   - Wire ist ein physikalisches Artefakt (nicht "C-API").
 *   - Wire ist explizit little-endian.
 *   - Header ist exakt 16 Byte und packed.
 *
 * Vertrag (CLWP):
 *   - 16-bit Magic (LE)
 *   - 8-bit ver_mode: high nibble = version, low nibble = mode
 *   - 8-bit flags
 *   - 48-bit seq (LE, 6 bytes)
 *   - 16-bit batch_id (LE)
 *   - 16-bit payload_len (LE)  -> Länge des Payloads OHNE Trailer
 *   - 8-bit hdr_crc8           -> Early-Reject (Header-CRC)
 *   - 8-bit reserved           -> muss 0 sein
 *
 * Hinweis:
 *   - Starke Integrität erfolgt NICHT im Header (sonst müsste der Header wachsen),
 *     sondern über Payload-Trailer (CRC32C) gemäß cl_wire_crc.h.
 * ============================================================================
 */

#include <stdint.h>
#include <string.h> /* memcpy */

/* ----------------------------- Konstanten -------------------------------- */

#define CL_WIRE_VER_1 1u

/* 16-bit Wire Magic (auf dem Draht LE kodiert) */
#define CL_WIRE_MAGIC_U16 0xC1C1u

/* Flags (bitweise, 8-bit) */
#define CL_WF_PAYLOAD_CRC32C   (1u << 0) /* Payload hat CRC32C Trailer (4B)          */
#define CL_WF_BATCH_CRC32C     (1u << 1) /* Frame trägt/abschließt Batch-CRC32C     */
#define CL_WF_COMPRESSED       (1u << 2) /* Payload komprimiert (z.B. LZ4)          */
#define CL_WF_FEC              (1u << 3) /* Payload ist FEC-geschützt / shard       */
#define CL_WF_URGENT           (1u << 4) /* Priorität im Budget (trotzdem budgetiert) */
#define CL_WF_SYNC             (1u << 5) /* Synchronisationsframe                   */
#define CL_WF_OOB              (1u << 6) /* OOB im Data-Plane-Kanal (nicht Discovery) */
#define CL_WF_RSVD             (1u << 7) /* reserviert (muss 0 sein)                */

/* ver/mode nibble packing */
#define CL_WIRE_VER_GET(vm)        (((vm) >> 4) & 0x0Fu)
#define CL_WIRE_MODE_GET(vm)       ((vm) & 0x0Fu)
#define CL_WIRE_VER_MODE(ver,mode) ((((ver)&0x0Fu) << 4) | ((mode)&0x0Fu))

/* Modes (low nibble) – Vertrag (semantisch, nicht backend-spezifisch) */
typedef enum {
    CL_WM_RAW           = 0, /* Zero-Transform: Payload ist "as-is"            */
    CL_WM_FLOW          = 1, /* FLOW ohne zusätzliche Transform-Trailer        */
    CL_WM_FLOW_LZ4      = 2, /* FLOW mit LZ4 (Payload-Format in FLOW-Contract) */
    CL_WM_FLOW_FEC      = 3, /* FLOW mit FEC                                    */
    CL_WM_FLOW_LZ4_FEC  = 4, /* FLOW mit LZ4 + FEC                               */
    CL_WM_OOB           = 5, /* OOB-Frame (nicht Beacon/Discovery)              */
    CL_WM_SYNC          = 6  /* SYNC/Control Frames                              */
} cl_wire_mode_t;

/* --------------------------- CLWP Header 16B ------------------------------ */

typedef struct __attribute__((packed)) cl_wire_hdr16 {
    uint16_t magic_le;        /* CL_WIRE_MAGIC_U16, LE */
    uint8_t  ver_mode;        /* ver/mode nibble */
    uint8_t  flags;           /* CL_WF_* */

    uint8_t  seq48_le[6];     /* 48-bit seq, LE (least-significant byte first) */

    uint16_t batch_id_le;     /* batch group, LE */
    uint16_t payload_len_le;  /* payload length (excludes any trailer), LE */

    uint8_t  hdr_crc8;        /* CRC8 over header (siehe unten) */
    uint8_t  reserved0;       /* muss 0 sein */
} cl_wire_hdr16_t;

_Static_assert(sizeof(cl_wire_hdr16_t) == 16, "cl_wire_hdr16_t muss exakt 16B sein");

/* ------------------------- Seq48 load/store --------------------------------
 * Physik:
 *   - seq48 wird als 6 Bytes little-endian gespeichert.
 *   - Zugriff erfolgt über memcpy, um Strict-Aliasing sauber zu halten.
 */
static inline uint64_t cl_wire_seq48_load(const uint8_t seq48_le[6]) {
    uint64_t v = 0;
    /* Little-endian: Byte 0 ist LSB */
    v |= (uint64_t)seq48_le[0] << 0;
    v |= (uint64_t)seq48_le[1] << 8;
    v |= (uint64_t)seq48_le[2] << 16;
    v |= (uint64_t)seq48_le[3] << 24;
    v |= (uint64_t)seq48_le[4] << 32;
    v |= (uint64_t)seq48_le[5] << 40;
    return v;
}

static inline void cl_wire_seq48_store(uint8_t seq48_le[6], uint64_t seq) {
    /* nur low 48 bits gültig */
    seq48_le[0] = (uint8_t)((seq >> 0) & 0xFFu);
    seq48_le[1] = (uint8_t)((seq >> 8) & 0xFFu);
    seq48_le[2] = (uint8_t)((seq >> 16) & 0xFFu);
    seq48_le[3] = (uint8_t)((seq >> 24) & 0xFFu);
    seq48_le[4] = (uint8_t)((seq >> 32) & 0xFFu);
    seq48_le[5] = (uint8_t)((seq >> 40) & 0xFFu);
}

/* ------------------------- Header CRC8 Vertrag -----------------------------
 *
 * Ziel:
 *   - hdr_crc8 ist ausschließlich Early-Reject, nicht "starke Integrität".
 *   - Starke Integrität erfolgt über CRC32C Trailer (cl_wire_crc.h).
 *
 * Definition (normativ):
 *   - hdr_crc8 wird über die 16B Header-Bytes berechnet,
 *     wobei hdr_crc8 selbst als 0 gesetzt wird.
 *   - reserved0 muss 0 sein.
 *
 * CRC8-Polynom:
 *   - Der konkrete CRC8-Algorithmus ist implementation-defined,
 *     ABER MUSS global einheitlich sein (manifestiert in cl_wire_crc.h).
 */


#pragma once
/* ============================================================================
 * CommanderLink – Runtime TOC (Table of Contents) – FINAL Endausbau
 * Datei: include/cl_runtime/cl_toc.h
 * ============================================================================
 *
 * Vertrag:
 *   - TOC ist die physikalische Verkabelung der Runtime-SHM.
 *   - TOC ist rein beschreibend (keine Policy, keine Logik).
 *
 * Layout:
 *   - cl_toc_header_t: exakt 64B (1 Cacheline)
 *   - cl_toc_entry_t : exakt 64B (1 Cacheline)
 *   - cl_toc_t       : Header + entries[CL_TOC_MAX_ENTRIES]
 *
 * Semantik:
 *   - Segment-ID: CL_<NAME>_SEG_<stride>
 *   - Segmentgröße steht im TOC (stride_bytes), nicht im Segment-ID-Namen.
 *     (Der Name trägt den stride nur als symbolische Eindeutigkeit.)
 * ============================================================================
 */

#include <stdint.h>

#include "cl_gates.h"

/* --------------------------------------------------------------------------
 * Architekturannahme
 * -------------------------------------------------------------------------- */
#if !defined(__x86_64__) && !defined(_M_X64)
#error "CommanderLink TOC ist x86_64-first"
#endif

/* --------------------------------------------------------------------------
 * TOC Version / Limits
 * -------------------------------------------------------------------------- */
#define CL_TOC_VERSION      1u
#define CL_TOC_MAX_ENTRIES  128u

/* --------------------------------------------------------------------------
 * Segment-IDs (SSOT für TOC type)
 *
 * Hinweis:
 *   Diese IDs sind für TOC/Tools/Monitor.
 *   Die semantische Rolle bleibt wichtiger als die Zahl selbst.
 * -------------------------------------------------------------------------- */
typedef enum cl_seg_type {

    /* 256 */
    CL_LINK_SEG_256            = 0x0100,
    CL_BUDGET_SEG_256          = 0x0101,
    CL_MEM_SEG_256             = 0x0102,
    CL_OVERLAY_SEG_256         = 0x0103,
    CL_WATCHDOG_SEG_256        = 0x0104,
    CL_PCIE_SEG_256            = 0x0105,
    CL_TIME_SEG_256            = 0x0106,
    CL_MESH_NEIGHBOR_SEG_256   = 0x0107,
    CL_MESH_PEER_SEG_256       = 0x0108,
    CL_ORACLE_SEG_256          = 0x0109,
    CL_SERVICE_SEG_256         = 0x010A,
    CL_IDENTITY_SEG_256        = 0x010B,
    CL_FLOW_CMD_SEG_256        = 0x010C,

    /* 512 */
    CL_NIC_SEG_512             = 0x0200,
    CL_BOARD_SEG_512           = 0x0201,

    /* 1024 */
    CL_CPU_SEG_1024            = 0x0300,

    /* 4096 */
    CL_DMA_SEG_4096            = 0x0400,
    CL_HISTORY_SEG_4096        = 0x0401,
    CL_FORENSICS_SEG_4096      = 0x0402,
    CL_ZFS_SEG_4096            = 0x0403
} cl_seg_type_t;

/* --------------------------------------------------------------------------
 * TOC Flags (physikalische Eigenschaften, keine Policy)
 * -------------------------------------------------------------------------- */
typedef uint32_t cl_toc_flags_t;

/* Speicherklasse */
#define CL_TOC_FLAG_RESIDENT        (1u << 0)  /* Core-SHM                         */
#define CL_TOC_FLAG_RECLAIMABLE     (1u << 1)  /* Bulk-SHM                         */

/* Bedeutung */
#define CL_TOC_FLAG_CRITICAL        (1u << 2)
#define CL_TOC_FLAG_OBSERVABLE      (1u << 3)

/* Änderungsfrequenz */
#define CL_TOC_FLAG_HOT             (1u << 4)
#define CL_TOC_FLAG_WARM            (1u << 5)
#define CL_TOC_FLAG_COLD            (1u << 6)

/* Instanzierung */
#define CL_TOC_FLAG_FIXED_COUNT     (1u << 7)
#define CL_TOC_FLAG_VARIABLE_COUNT  (1u << 8)

/* Rolle */
#define CL_TOC_FLAG_DATA            (1u << 9)
#define CL_TOC_FLAG_CONTROL         (1u << 10)
#define CL_TOC_FLAG_FORENSICS       (1u << 11)

/* --------------------------------------------------------------------------
 * Source/Access/Presence/Layout
 * -------------------------------------------------------------------------- */
typedef uint16_t cl_toc_source_t;
#define CL_TOC_SOURCE_DIRECT        (1u << 0)
#define CL_TOC_SOURCE_BEST_EFFORT   (1u << 1)
#define CL_TOC_SOURCE_ESTIMATED     (1u << 2)
#define CL_TOC_SOURCE_UNSUPPORTED   (1u << 3)

typedef uint16_t cl_toc_access_t;
#define CL_TOC_ACCESS_READ          (1u << 0)
#define CL_TOC_ACCESS_WRITE         (1u << 1)
#define CL_TOC_ACCESS_ATOMIC        (1u << 2)

typedef uint16_t cl_toc_presence_t;
#define CL_TOC_PRESENT              (1u << 0)
#define CL_TOC_RECLAIMED            (1u << 1)
#define CL_TOC_PARTIAL              (1u << 2)

typedef uint16_t cl_toc_layout_t;
#define CL_TOC_LAYOUT_CACHELINE     (1u << 0)
#define CL_TOC_LAYOUT_ARRAY         (1u << 1)

/* --------------------------------------------------------------------------
 * TOC Entry – exakt 64B
 * -------------------------------------------------------------------------- */
typedef struct cl_toc_entry {

    uint16_t       type;           /* cl_seg_type_t */
    uint16_t       version;        /* Segment-spezifische Version */
    uint32_t       flags;          /* cl_toc_flags_t */

    uint32_t       stride_bytes;   /* 256/512/1024/4096 */
    uint32_t       count;          /* Instanzen */

/* offset_bytes:
 *   SSOT-Regel (bitgenau, projektweit):
 *   - Wenn (flags & CL_TOC_FLAG_RECLAIMABLE) == 0:
 *       offset_bytes ist relativ zur Core-SHM Base (CL_CORE_SHM_NAME mapping).
 *   - Wenn (flags & CL_TOC_FLAG_RECLAIMABLE) != 0:
 *       offset_bytes ist relativ zur Bulk-SHM Base (CL_BULK_SHM_NAME mapping).
 *
 *   Consumer (Monitor/Tools/HAL/CORE/LINK/FLOW) dürfen dies niemals raten,
 *   sondern müssen die Base ausschließlich über das RECLAIMABLE-Flag wählen.
 */

    uint64_t       offset_bytes;   /* Offset relativ zum jeweiligen SHM-Beginn */

    uint16_t       source_flags;   /* cl_toc_source_t  */
    uint16_t       access_flags;   /* cl_toc_access_t  */
    uint16_t       presence_flags; /* cl_toc_presence_t */
    uint16_t       layout_flags;   /* cl_toc_layout_t */

    cl_gate_mask_t gates_possible;
    cl_gate_mask_t gates_effective;

    uint64_t       epoch;

    uint64_t       reserved0;      /* macht 64B exakt */

} cl_toc_entry_t;

_Static_assert(sizeof(cl_toc_entry_t) == 64, "cl_toc_entry_t muss exakt 64B sein");

/* --------------------------------------------------------------------------
 * TOC Header – exakt 64B
 * -------------------------------------------------------------------------- */
typedef struct cl_toc_header {

    uint32_t version;        /* CL_TOC_VERSION */
    uint32_t entry_count;    /* Anzahl gültiger Entries */

    uint64_t total_bytes;    /* Core oder Core+Bulk (konkret, nicht geraten) */
    uint64_t build_id;       /* ABI-/Build-Fingerprint */
    uint64_t epoch;          /* TOC epoch */

    uint64_t reserved[4];    /* macht 64B exakt */

} cl_toc_header_t;

_Static_assert(sizeof(cl_toc_header_t) == 64, "cl_toc_header_t muss exakt 64B sein");

/* --------------------------------------------------------------------------
 * Gesamter TOC
 * -------------------------------------------------------------------------- */
typedef struct cl_toc {

    cl_toc_header_t header;
    cl_toc_entry_t  entries[CL_TOC_MAX_ENTRIES];

} cl_toc_t;


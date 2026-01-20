#pragma once
/* ============================================================================
 * CommanderLink – Segment: DMA Meta (4096B) – FINAL Endausbau
 * Datei: include/cl_runtime/cl_seg_dma_4096.h
 * ============================================================================
 *
 * Zweck:
 *   - RX/TX Ring Meta, Slot Meta; Payload liegt backend-spezifisch außerhalb SHM.
 *
 * Maximalmodell:
 *   - alignment/headroom/atom geometry sichtbar
 *   - crc/fec counters sichtbar
 *
 * Layout:
 *   4096B = 64 Cachelines:
 *     RX hdr: 4 lines (256B)
 *     TX hdr: 4 lines (256B)
 *     Slot meta: 56 lines
 * ============================================================================
 */

#include <stdint.h>
#include <stdatomic.h>

#include "../cl_common/cl_align.h"
#include "../cl_manifest/cl_enums.h"
#include "cl_quality.h"

/* Slot state */
typedef enum { CL_DMA_SLOT_FREE=0, CL_DMA_SLOT_BUSY=1, CL_DMA_SLOT_READY=2 } cl_dma_slot_state_t;

/* Ring header line 64B */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint32_t) ring_size;
        _Atomic(uint32_t) ring_mask;
        _Atomic(uint32_t) head;
        _Atomic(uint32_t) tail;

        _Atomic(uint64_t) slots_ready;
        _Atomic(uint64_t) slots_busy;

        _Atomic(uint32_t) alignment_bytes;     /* expected 64 */
        _Atomic(uint32_t) headroom_bytes;      /* expected 64 */

        _Atomic(uint32_t) payload_quantum_bytes; /* 64 */
        _Atomic(uint32_t) reserved0;

        uint8_t src;
        uint8_t qual;
        uint16_t reserved1;
        uint32_t reserved2;
    };
    uint8_t raw[64];
} cl_dma_ring_hdr_64_t;

CL_STATIC_ASSERT(sizeof(cl_dma_ring_hdr_64_t)==64, "dma ring hdr 64");

/* Slot meta line 64B */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) seq;
        _Atomic(uint64_t) ts_ns;
        _Atomic(uint64_t) opaque_ref64;  /* UMEM offset / netmap token / ptr */
        _Atomic(uint64_t) reserved0;

        _Atomic(uint32_t) state;         /* cl_dma_slot_state_t */
        _Atomic(uint32_t) payload_len;

        _Atomic(uint32_t) crc32c;        /* optional: per-slot/atom CRC summary */
        _Atomic(uint32_t) reserved1;

        _Atomic(uint64_t) reserved2;
    };
    uint8_t raw[64];
} cl_dma_slot_meta_64_t;

CL_STATIC_ASSERT(sizeof(cl_dma_slot_meta_64_t)==64, "dma slot meta 64");

/* Segment 4096 */
typedef struct CL_ALIGNAS(4096) {
    cl_dma_ring_hdr_64_t rx_hdr[4];
    cl_dma_ring_hdr_64_t tx_hdr[4];
    cl_dma_slot_meta_64_t slot[56];
} cl_dma_seg_4096_t;

CL_STATIC_ASSERT(sizeof(cl_dma_seg_4096_t)==4096, "dma seg 4096");


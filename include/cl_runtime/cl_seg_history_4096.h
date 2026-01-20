#pragma once
/* ============================================================================
 * CommanderLink – Segment: History (4096B) – FINAL Endausbau
 * Datei: include/cl_runtime/cl_seg_history_4096.h
 * ============================================================================
 *
 * Zweck:
 *   - Oracle Trenddaten (eine Metrik pro Segmentinstanz).
 *   - Maximalmodell: Trends werden als Werte+Quelle/Qualität abgebildet.
 *
 * Layout:
 *   Header 64B + 63 Points à 64B.
 * ============================================================================
 */

#include <stdint.h>
#include <stdatomic.h>

#include "../cl_common/cl_align.h"
#include "cl_quality.h"

/* Metric IDs (extend-only) */
typedef enum {
    CL_HIST_TX_BPS = 1,
    CL_HIST_RX_BPS = 2,
    CL_HIST_LOSS_PPM = 3,
    CL_HIST_JITTER_P99_US = 4,
    CL_HIST_CPU_PRESSURE = 5,
    CL_HIST_MEM_PRESSURE = 6,
    CL_HIST_STORAGE_SINK = 7,
    CL_HIST_THERMAL = 8
} cl_hist_metric_id_t;

/* Header 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint32_t) metric_id;
        _Atomic(uint32_t) write_idx;
        _Atomic(uint32_t) count;
        _Atomic(uint32_t) reserved0;

        _Atomic(uint64_t) last_ts_ns;

        uint8_t src;
        uint8_t qual;
        uint16_t reserved1;
        uint32_t reserved2;

        uint64_t reserved3;
        uint64_t reserved4;
    };
    uint8_t raw[64];
} cl_hist_hdr_64_t;

CL_STATIC_ASSERT(sizeof(cl_hist_hdr_64_t)==64, "hist hdr 64");

/* Point 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) ts_ns;
        _Atomic(uint64_t) value;
        _Atomic(uint64_t) reserved0;
        _Atomic(uint64_t) reserved1;
        _Atomic(uint64_t) reserved2;
        _Atomic(uint64_t) reserved3;
        _Atomic(uint64_t) reserved4;
        _Atomic(uint64_t) reserved5;
    };
    uint8_t raw[64];
} cl_hist_point_64_t;

CL_STATIC_ASSERT(sizeof(cl_hist_point_64_t)==64, "hist point 64");

typedef struct CL_ALIGNAS(4096) {
    cl_hist_hdr_64_t hdr;
    cl_hist_point_64_t pts[63];
} cl_history_seg_4096_t;

CL_STATIC_ASSERT(sizeof(cl_history_seg_4096_t)==4096, "history seg 4096");


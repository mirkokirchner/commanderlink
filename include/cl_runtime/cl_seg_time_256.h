#pragma once
/* ============================================================================
 * CommanderLink – Segment: Timebase (256B) – FINAL Endausbau
 * Datei: include/cl_runtime/cl_seg_time_256.h
 * ============================================================================
 */

#include <stdint.h>
#include <stdatomic.h>

#include "../cl_common/cl_align.h"
#include "cl_quality.h"

/* HOT 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) mono_now_ns;
        _Atomic(uint64_t) wall_now_ns;
        _Atomic(uint64_t) boot_id_ns;
        uint64_t reserved0;
        uint64_t reserved1;
        uint64_t reserved2;
        uint64_t reserved3;
        uint64_t reserved4;
    };
    uint8_t raw[64];
} cl_time_hot_64_t;

CL_STATIC_ASSERT(sizeof(cl_time_hot_64_t) == 64, "time hot 64");

/* WARM 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(int64_t)  clock_offset_ns;
        _Atomic(uint32_t) time_quality_ppb;
        uint32_t reserved0;
        uint64_t reserved1;
        uint64_t reserved2;
        uint64_t reserved3;
        uint64_t reserved4;
        uint64_t reserved5;
    };
    uint8_t raw[64];
} cl_time_warm_64_t;

CL_STATIC_ASSERT(sizeof(cl_time_warm_64_t) == 64, "time warm 64");

/* COLD 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        uint8_t src;
        uint8_t qual;
        uint16_t reserved0;
        uint32_t reserved1;
        uint64_t reserved2;
        uint64_t reserved3;
        uint64_t reserved4;
        uint64_t reserved5;
    };
    uint8_t raw[64];
} cl_time_cold_64_t;

CL_STATIC_ASSERT(sizeof(cl_time_cold_64_t) == 64, "time cold 64");

typedef union CL_ALIGNAS(8) { struct { uint64_t r[8]; }; uint8_t raw[64]; } cl_time_rsv_64_t;

typedef struct CL_ALIGNAS(256) {
    cl_time_hot_64_t  hot;
    cl_time_warm_64_t warm;
    cl_time_cold_64_t cold;
    cl_time_rsv_64_t  rsv;
} cl_time_seg_256_t;

CL_STATIC_ASSERT(sizeof(cl_time_seg_256_t) == 256, "time seg 256");


#pragma once
/* ============================================================================
 * CommanderLink – Segment: Memory (256B) – FINAL Endausbau
 * Datei: include/cl_runtime/cl_seg_mem_256.h
 * ============================================================================
 *
 * Maximalmodell:
 * - total/free/used/ecc
 * - memory bandwidth hints (best-effort)
 * - DIMM mode strings optional via reserved/bulk later
 * ============================================================================
 */

#include <stdint.h>
#include <stdatomic.h>

#include "../cl_common/cl_align.h"
#include "../cl_manifest/cl_enums.h"
#include "cl_quality.h"

/* HOT 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) total_bytes;
        _Atomic(uint64_t) used_bytes;
        _Atomic(uint64_t) free_bytes;
        _Atomic(uint64_t) ecc_err_cnt;

        _Atomic(uint32_t) pressure_permille;
        _Atomic(uint32_t) reason_code;

        uint8_t src;
        uint8_t qual;
        uint16_t reserved0;
        uint32_t reserved1;
    };
    uint8_t raw[64];
} cl_mem_hot_64_t;

CL_STATIC_ASSERT(sizeof(cl_mem_hot_64_t)==64, "mem hot 64");

/* WARM 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) bw_read_bps;
        _Atomic(uint64_t) bw_write_bps;

        _Atomic(uint32_t) timing_tcl;
        _Atomic(uint32_t) timing_trcd;
        _Atomic(uint32_t) timing_trp;
        _Atomic(uint32_t) timing_tras;

        _Atomic(uint32_t) channel_state; /* cl_feature_state_t */
        _Atomic(uint32_t) reserved0;

        _Atomic(uint64_t) last_update_ns;
    };
    uint8_t raw[64];
} cl_mem_warm_64_t;

CL_STATIC_ASSERT(sizeof(cl_mem_warm_64_t)==64, "mem warm 64");

/* COLD 64 (inventory strings) */
typedef union CL_ALIGNAS(8) {
    struct {
        char dimm_mode[16];    /* "DDR4-2133" / "XMP" etc best-effort */
        char channel_mode[16]; /* "DUAL" "SINGLE" etc */
        char reserved_str[16];

        uint8_t src;
        uint8_t qual;
        uint16_t reserved0;

        uint32_t reserved1;
        uint64_t reserved2;
    };
    uint8_t raw[64];
} cl_mem_cold_64_t;

CL_STATIC_ASSERT(sizeof(cl_mem_cold_64_t)==64, "mem cold 64");

/* RSV 64 */
typedef union CL_ALIGNAS(8) { struct { uint64_t r[8]; }; uint8_t raw[64]; } cl_mem_rsv_64_t;

typedef struct CL_ALIGNAS(256) {
    cl_mem_hot_64_t  hot;
    cl_mem_warm_64_t warm;
    cl_mem_cold_64_t cold;
    cl_mem_rsv_64_t  rsv;
} cl_mem_seg_256_t;

CL_STATIC_ASSERT(sizeof(cl_mem_seg_256_t)==256, "mem seg 256");


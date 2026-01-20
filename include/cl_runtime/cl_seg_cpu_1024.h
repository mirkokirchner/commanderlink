#pragma once
/* ============================================================================
 * CommanderLink – Segment: CPU Core (1024B) – FINAL Endausbau
 * Datei: include/cl_runtime/cl_seg_cpu_1024.h
 * ============================================================================
 *
 * Record = 16 Cachelines à 64B.
 * Maximalmodell:
 * - ISA possible/effective/state
 * - Thermik/Throttle/Headroom (state+reason)
 * - Hersteller/Modell/µcode (COLD strings)
 * ============================================================================
 */

#include <stdint.h>
#include <stdatomic.h>

#include "../cl_common/cl_align.h"
#include "../cl_manifest/cl_enums.h"
#include "../cl_manifest/cl_features.h"
#include "cl_quality.h"

/* HOT 0 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) tsc_val;
        _Atomic(uint64_t) last_update_ns;

        _Atomic(uint32_t) local_seq;
        _Atomic(uint32_t) load_permille;

        _Atomic(uint32_t) temp_c_x10;
        _Atomic(uint32_t) throttling;       /* 0/1 */

        _Atomic(uint32_t) headroom_permille;/* best-effort */
        _Atomic(uint32_t) reason_code;      /* cl_reason_code_t */

        _Atomic(uint32_t) constraint_flags; /* thermal/power/budget */
        _Atomic(uint32_t) reserved0;
    };
    uint8_t raw[64];
} cl_cpu_hot_64a_t;

CL_STATIC_ASSERT(sizeof(cl_cpu_hot_64a_t)==64, "cpu hot 64a");

/* HOT 1 (ISA) */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint32_t) isa_possible_mask;  /* cl_cpu_isa_mask_t */
        _Atomic(uint32_t) isa_effective;      /* cl_cpu_isa_effective_t */

        _Atomic(uint16_t) isa_state;          /* cl_feature_state_t */
        _Atomic(uint16_t) avx512_risk;        /* cl_avx512_risk_t (manifest) */
        _Atomic(uint16_t) simd_effective;     /* cl_simd_effective_t (manifest) */
        _Atomic(uint16_t) reserved0;

        _Atomic(uint64_t) inst_retired;
        _Atomic(uint64_t) cycles;

        _Atomic(uint64_t) uops_issued;
        _Atomic(uint64_t) uops_stalled;
    };
    uint8_t raw[64];
} cl_cpu_hot_64b_t;

CL_STATIC_ASSERT(sizeof(cl_cpu_hot_64b_t)==64, "cpu hot 64b");

/* WARM lines 2..7 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint64_t) l1d_misses;
        _Atomic(uint64_t) itlb_misses;
        _Atomic(uint64_t) dtlb_misses;
        _Atomic(uint64_t) llc_misses;

        _Atomic(uint64_t) reserved0;
        _Atomic(uint64_t) reserved1;
        _Atomic(uint64_t) reserved2;
        _Atomic(uint64_t) reserved3;
    };
    uint8_t raw[64];
} cl_cpu_warm_64_t;

CL_STATIC_ASSERT(sizeof(cl_cpu_warm_64_t)==64, "cpu warm 64");

/* COLD 8 (IDs) */
typedef union CL_ALIGNAS(8) {
    struct {
        uint32_t vendor_id;
        uint32_t device_id;
        uint32_t model_id;
        uint32_t stepping;

        uint32_t numa_node;
        uint32_t reserved0;

        uint8_t src;
        uint8_t qual;
        uint16_t reserved1;
        uint32_t reserved2;

        uint64_t reserved3;
    };
    uint8_t raw[64];
} cl_cpu_cold_64a_t;

CL_STATIC_ASSERT(sizeof(cl_cpu_cold_64a_t)==64, "cpu cold 64a");

/* COLD 9 (strings vendor/model) Teil 1*/
typedef union CL_ALIGNAS(8) {
    struct {
        char vendor_str[16];   /* "GenuineIntel" etc */
        char model_str_a[16];  /* Modelname Teil A */
        char model_str_b[16];  /* Modelname Teil B */

        uint8_t src;
        uint8_t qual;
        uint16_t reserved0;
        uint32_t reserved1;
        uint64_t reserved2;
    };
    uint8_t raw[64];
} cl_cpu_cold_64b_t;

CL_STATIC_ASSERT(sizeof(cl_cpu_cold_64b_t)==64, "cpu cold 64b");

/* COLD 10 (strings) TEIL 2 */
typedef union CL_ALIGNAS(8) {
    struct {
        char model_str_c[32];  /* Modelname Teil C (weitere 32 Zeichen) */
        char ucode_str[16];    /* Microcode/Revision kurz */
        
        uint8_t src;
        uint8_t qual;
        uint16_t reserved0;
        uint32_t reserved1;
        uint64_t reserved2;
    };
    uint8_t raw[64];
} cl_cpu_cold_64c_t;

CL_STATIC_ASSERT(sizeof(cl_cpu_cold_64c_t)==64, "cpu cold 64c");

/* COLD 11..15 reserved/topology */
typedef union CL_ALIGNAS(8) { struct { uint64_t r[8]; }; uint8_t raw[64]; } cl_cpu_cold_64r_t;
CL_STATIC_ASSERT(sizeof(cl_cpu_cold_64r_t)==64, "cpu cold r 64");

/* Record 1024 */
typedef struct CL_ALIGNAS(1024) {
    cl_cpu_hot_64a_t hot_a;           /* 0 */
    cl_cpu_hot_64b_t hot_b;           /* 1 */
    cl_cpu_warm_64_t warm[6];         /* 2..7 */
    cl_cpu_cold_64a_t cold_a;         /* 8 */
    cl_cpu_cold_64b_t cold_b;         /* 9 */
    cl_cpu_cold_64c_t cold_c;         /* 9 */
    cl_cpu_cold_64r_t cold_r[5];      /* 11..15 */
} cl_cpu_seg_1024_t;

CL_STATIC_ASSERT(sizeof(cl_cpu_seg_1024_t)==1024, "cpu seg 1024");


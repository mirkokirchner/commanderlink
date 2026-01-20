#pragma once
/* ============================================================================
 * CommanderLink – Segment: PCIe Summary (256B) – FINAL Endausbau
 * Datei: include/cl_runtime/cl_seg_pcie_256.h
 * ============================================================================
 *
 * Maximalmodell:
 * - throughput + error counters
 * - vendor/device + human label (best-effort)
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
        _Atomic(uint64_t) tx_bps;
        _Atomic(uint64_t) rx_bps;

        _Atomic(uint32_t) lcrc_err;
        _Atomic(uint32_t) tlp_retry;

        _Atomic(uint32_t) reason_code;
        _Atomic(uint32_t) constraint_flags;

        _Atomic(uint64_t) last_update_ns;
    };
    uint8_t raw[64];
} cl_pcie_hot_64_t;

CL_STATIC_ASSERT(sizeof(cl_pcie_hot_64_t)==64, "pcie hot 64");

/* WARM 64 */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint32_t) eye_margin_db_x100;
        _Atomic(uint32_t) lane_state; /* cl_feature_state_t */
        _Atomic(uint64_t) reserved0;
        _Atomic(uint64_t) reserved1;
        _Atomic(uint64_t) reserved2;
        _Atomic(uint64_t) reserved3;
        _Atomic(uint64_t) reserved4;
        _Atomic(uint64_t) reserved5;
    };
    uint8_t raw[64];
} cl_pcie_warm_64_t;

CL_STATIC_ASSERT(sizeof(cl_pcie_warm_64_t)==64, "pcie warm 64");

/* COLD 64 (IDs + name) */
typedef union CL_ALIGNAS(8) {
    struct {
        uint32_t vendor_id;
        uint32_t device_id;

        uint16_t domain;
        uint8_t bus;
        uint8_t dev;
        uint8_t func;
        uint8_t reserved0[3];

        char name[32]; /* best-effort short label */

        uint8_t src;
        uint8_t qual;
        uint16_t reserved1;
    };
    uint8_t raw[64];
} cl_pcie_cold_64_t;

CL_STATIC_ASSERT(sizeof(cl_pcie_cold_64_t)==64, "pcie cold 64");

/* RSV 64 */
typedef union CL_ALIGNAS(8) { struct { uint64_t r[8]; }; uint8_t raw[64]; } cl_pcie_rsv_64_t;

typedef struct CL_ALIGNAS(256) {
    cl_pcie_hot_64_t  hot;
    cl_pcie_warm_64_t warm;
    cl_pcie_cold_64_t cold;
    cl_pcie_rsv_64_t  rsv;
} cl_pcie_seg_256_t;

CL_STATIC_ASSERT(sizeof(cl_pcie_seg_256_t)==256, "pcie seg 256");


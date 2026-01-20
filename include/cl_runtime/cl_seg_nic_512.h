#pragma once
/* ============================================================================
 * CommanderLink – Segment: NIC (512B) – FINAL Endausbau
 * Datei: include/cl_runtime/cl_seg_nic_512.h
 * ============================================================================
 *
 * Record = 8 Cachelines.
 * Maximalmodell:
 * - Ports/Queues/Caps possible/effective/state
 * - DOM/SFP inventory strings (vendor/part/serial) + thresholds
 * - Loss/Jitter/Burst + reasons
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
        _Atomic(uint64_t) rx_bps;
        _Atomic(uint64_t) tx_bps;

        _Atomic(uint32_t) local_seq;
        _Atomic(uint32_t) link_speed_mbps;

        _Atomic(uint16_t) mtu_effective;
        _Atomic(uint16_t) mtu_max;
        _Atomic(uint16_t) mtu_min;
        _Atomic(uint16_t) ports_count;

        _Atomic(uint32_t) queues_count;
        _Atomic(uint32_t) reason_code;

        _Atomic(uint32_t) constraint_flags;
        _Atomic(uint32_t) reserved0;
    };
    uint8_t raw[64];
} cl_nic_hot_64_t;

CL_STATIC_ASSERT(sizeof(cl_nic_hot_64_t)==64, "nic hot 64");

/* WARM A 64 (queues/drops) */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint32_t) rx_queue_fill_permille;
        _Atomic(uint32_t) tx_queue_fill_permille;
        _Atomic(uint32_t) rx_dropped;
        _Atomic(uint32_t) tx_dropped;
        _Atomic(uint32_t) rx_overruns;
        _Atomic(uint32_t) tx_overruns;
        _Atomic(uint32_t) reorder_events;
        _Atomic(uint32_t) burst_max;          /* max consecutive losses (warm) */

        _Atomic(uint32_t) crc_fail_1s;        /* atoms/packets */
        _Atomic(uint32_t) crc_fail_total;

        _Atomic(uint32_t) fec_recover_1s;
        _Atomic(uint32_t) fec_recover_total;
    };
    uint8_t raw[64];
} cl_nic_warm_64a_t;

CL_STATIC_ASSERT(sizeof(cl_nic_warm_64a_t)==64, "nic warm a 64");

/* WARM B 64 (DOM/thermal) */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint32_t) dom_temp_c_x10;
        _Atomic(uint32_t) dom_warn_c_x10;
        _Atomic(uint32_t) dom_crit_c_x10;
        _Atomic(uint32_t) dom_alarm_bits;

        _Atomic(uint32_t) thermal_state;        /* cl_thermal_state_t */
        _Atomic(uint32_t) pacing_scale_permille;/* 1000/950/700/0 */

        _Atomic(uint16_t) dom_state;            /* cl_feature_state_t */
        _Atomic(uint16_t) reserved0;
        _Atomic(uint32_t) reserved1;

        _Atomic(uint32_t) loss_ppm;
        _Atomic(uint32_t) jitter_p99_us;
    };
    uint8_t raw[64];
} cl_nic_warm_64b_t;

CL_STATIC_ASSERT(sizeof(cl_nic_warm_64b_t)==64, "nic warm b 64");

/* WARM C 64 (aggregation + backend/profile) */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint32_t) group_id;
        _Atomic(uint32_t) active_ports_mask;

        _Atomic(uint32_t) backend_state;
        _Atomic(uint32_t) backend_active;

        _Atomic(uint32_t) profile_effective;
        _Atomic(uint32_t) constraint_flags;

        _Atomic(uint32_t) reason_code;
        _Atomic(uint32_t) reserved0;

        _Atomic(uint64_t) link_down_events;
        _Atomic(uint64_t) mtu_reneg_events;
    };
    uint8_t raw[64];
} cl_nic_warm_64c_t;

CL_STATIC_ASSERT(sizeof(cl_nic_warm_64c_t)==64, "nic warm c 64");

/* COLD A 64 (PCI identity + names) */
typedef union CL_ALIGNAS(8) {
    struct {
        uint16_t pci_vendor_id;
        uint16_t pci_device_id;
        uint16_t pci_domain;
        uint16_t reserved0;

        uint8_t pci_bus;
        uint8_t pci_dev;
        uint8_t pci_func;
        uint8_t reserved1;

        char iface_name[16];
        char driver_name[16];

        uint8_t src;
        uint8_t qual;
        uint16_t reserved2;

        uint32_t reserved3;
        uint64_t reserved4;
    };
    uint8_t raw[64];
} cl_nic_cold_64a_t;

CL_STATIC_ASSERT(sizeof(cl_nic_cold_64a_t)==64, "nic cold a 64");

/* COLD B 64 (caps/state) */
typedef union CL_ALIGNAS(8) {
    struct {
        _Atomic(uint32_t) caps_zerocopy;
        _Atomic(uint32_t) caps_rdma;

        _Atomic(uint16_t) bypass_state;   /* cl_feature_state_t */
        _Atomic(uint16_t) rdma_state;     /* cl_feature_state_t */
        _Atomic(uint16_t) rss_state;      /* cl_feature_state_t */
        _Atomic(uint16_t) flow_dir_state; /* cl_feature_state_t */

        _Atomic(uint32_t) backend_possible_mask; /* bitmask cl_backend_active_t */
        _Atomic(uint32_t) reserved0;

        uint8_t src;
        uint8_t qual;
        uint16_t reserved1;

        uint32_t reserved2;
        uint64_t reserved3;
    };
    uint8_t raw[64];
} cl_nic_cold_64b_t;

CL_STATIC_ASSERT(sizeof(cl_nic_cold_64b_t)==64, "nic cold b 64");

/* COLD C 64 (SFP strings) */
typedef union CL_ALIGNAS(8) {
    struct {
        char sfp_vendor[16];
        char sfp_part[16];
        char sfp_serial[16];

        uint8_t src;
        uint8_t qual;
        uint16_t reserved0;

        uint32_t reserved1;
        uint64_t reserved2;
    };
    uint8_t raw[64];
} cl_nic_cold_64c_t;

CL_STATIC_ASSERT(sizeof(cl_nic_cold_64c_t)==64, "nic cold c 64");

/* Record 512 */
typedef struct CL_ALIGNAS(512) {
    cl_nic_hot_64_t    hot;
    cl_nic_warm_64a_t  warm_a;
    cl_nic_warm_64b_t  warm_b;
    cl_nic_warm_64c_t  warm_c;
    cl_nic_cold_64a_t  cold_a;
    cl_nic_cold_64b_t  cold_b;
    cl_nic_cold_64c_t  cold_c;
    /* reserved line for future */
    uint8_t            rsv[64];
} cl_nic_seg_512_t;

CL_STATIC_ASSERT(sizeof(cl_nic_seg_512_t)==512, "nic seg 512");


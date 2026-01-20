#pragma once
/* ============================================================================
 * CommanderLink – Core SHM Root Layout (Runtime) – FINAL
 * Datei: include/cl_runtime/cl_shm_layout.h
 * ============================================================================
 *
 * Ziel:
 *   Root ist die Notfall-Wahrheit: minimal, deterministisch, 4096B.
 *
 * Maschinenregel:
 *   - raw[4096] Normfläche.
 *   - Host-Order.
 *   - 64-bit-first Feldreihenfolge.
 * ============================================================================
 */

#include <stdint.h>
#include <stdatomic.h>

#include "../cl_common/cl_align.h"
#include "../cl_manifest/cl_ids.h"
#include "../cl_manifest/cl_enums.h"
#include "cl_schema.h"

#define CL_CORE_SHM_NAME   "/cl_core_root"
#define CL_BULK_SHM_NAME   "/cl_bulk"

#define CL_ROOT_MAGIC_U32          0x54524C43u /* "CLRT" in Host-Order */
#define CL_ROOT_FLAG_FAILFAST_ABI  (1u << 0)


typedef union CL_ALIGNAS(CL_SHM_PAGE_BYTES) {
    struct {
        /* 64-bit first */
        uint64_t abi_layout_checksum;

        _Atomic(uint64_t) seq_cnt;
        _Atomic(uint64_t) uptime_ns;
        _Atomic(uint64_t) bulk_epoch;

        uint64_t boot_id_ns;

        uint64_t toc_offset;
        uint64_t toc_size;

		/* 32-bit */
        uint32_t root_magic;      /* CL_ROOT_MAGIC_U32 */
        uint32_t root_flags;      /* CL_ROOT_FLAG_*    */

        uint32_t schema_version;
        uint32_t endian_magic;

        _Atomic(uint32_t) global_health;
        _Atomic(uint32_t) budget_state;

        _Atomic(uint32_t) path_state;
        _Atomic(uint32_t) backend_active;
        _Atomic(uint32_t) mode_active;
        _Atomic(uint32_t) profile_active;

        _Atomic(uint32_t) rx_gate;
        _Atomic(uint32_t) tx_gate;

        _Atomic(uint32_t) constraint_flags;
        _Atomic(uint32_t) reason_code;

        _Atomic(uint32_t) bulk_present;

        uint32_t overlay_ip_host_u32;
        uint32_t overlay_failopen_events;
        uint16_t overlay_routes;
        uint16_t mtu_effective;

        /* 8-bit */
        _Atomic(uint8_t) active_index;
        uint8_t overlay_prefixlen;
        uint8_t overlay_enabled;
        uint8_t reserved_u8;

        /* Trust quick view */
        _Atomic(uint32_t) trust_policy_required;
        _Atomic(uint32_t) trusted_peer_count;
    };
    uint8_t raw[CL_SHM_PAGE_BYTES];
} cl_root_4096_t;

CL_STATIC_ASSERT(sizeof(cl_root_4096_t) == CL_SHM_PAGE_BYTES, "root must be 4096B");

/* Back-compat */
typedef cl_root_4096_t cl_root_t;


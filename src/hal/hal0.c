#define _POSIX_C_SOURCE 200809L
/*
 * ============================================================================
 * CommanderLink – HAL0 (Stage-1 Minimal Endausbau) – FINAL
 * Datei: src/hal/hal0.c
 * ============================================================================
 *
 * Stage-1 Pflichten:
 *   - Kein reiner Heartbeat-Stub: HAL schreibt echte Wahrnehmung
 *   - Mindestens TIME/CPU/NIC werden befüllt
 *   - src/qual werden korrekt gesetzt (liegen in deinem Layout NICHT im HOT)
 *   - Keine Policy, kein Redirect, kein OS-Plumbing
 *
 * WICHTIG:
 *   Diese Datei ist feldgenau an deinen aktuellen Runtime-Header angepasst:
 *     TIME:  hot.mono_now_ns / hot.wall_now_ns / hot.boot_id_ns   (atomic u64)
 *            cold.src / cold.qual                                (u8)
 *     CPU:   hot_a.reason_code / hot_a.constraint_flags           (atomic u32)
 *            hot_b.isa_possible_mask / hot_b.isa_effective        (atomic u32)
 *            hot_b.isa_state / hot_b.simd_effective               (atomic u16)
 *            cold_a.src / cold_a.qual                             (u8)
 *     NIC:   hot.reason_code                                      (atomic u32)
 *            cold_a.src / cold_a.qual                             (u8)
 * ============================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdatomic.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>      /* O_RDWR */
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>

#include "cl_runtime/cl_runtime.h"
#include "cl_runtime/cl_service_seg_256.h"
#include "cl_runtime/cl_quality.h"
#include "cl_runtime/cl_commit.h"

/* ---------------- Zeit ---------------- */
static uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

/* ---------------- SHM map ---------------- */
static int map_core_rw(void **out_base, size_t *out_sz) {
    int fd = shm_open(CL_CORE_SHM_NAME, O_RDWR, 0);
    if (fd < 0) return -1;

    struct stat st;
    if (fstat(fd, &st) != 0) { close(fd); return -2; }
    if (st.st_size <= 0) { close(fd); return -3; }

    void *p = mmap(NULL, (size_t)st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (p == MAP_FAILED) return -4;

    *out_base = p;
    *out_sz = (size_t)st.st_size;
    return 0;
}

/* ---------------- Locator ---------------- */
typedef struct hal_loc {
    cl_service_seg_256_t *svc;
    cl_service_slot_32_t *slot_hal;

    cl_time_seg_256_t    *time;
    cl_cpu_seg_1024_t    *cpu0;
    cl_nic_seg_512_t     *nic0;

    cl_toc_entry_t       *e_time;
    cl_toc_entry_t       *e_cpu0;
    cl_toc_entry_t       *e_nic0;
} hal_loc_t;

/* ---------------- Service slot mapping (driftfest, identisch zu cld/core0) -- */


/* Bounds helper: avoid UB on TOC offsets. */
#define HAL_BOUNDS_OK(_off, _need, _sz)             (((uint64_t)(_off)) + ((uint64_t)(_need)) <= ((uint64_t)(_sz)))

static int hal_locate(void *core_base, size_t core_sz, hal_loc_t *out) {
    memset(out, 0, sizeof(*out));

    cl_root_t *r = (cl_root_t*)core_base;
    if (r->root_magic != CL_ROOT_MAGIC_U32) return -20;
    if (r->schema_version != CL_SCHEMA_VERSION_U32) return -21;
    if (r->endian_magic != CL_ENDIAN_MAGIC_U32) return -22;
    if (r->toc_size < sizeof(cl_toc_header_t)) return -1;
    if ((uint64_t)r->toc_offset + (uint64_t)r->toc_size > (uint64_t)core_sz) return -2;

    cl_toc_t *toc = (cl_toc_t*)((uint8_t*)core_base + (size_t)r->toc_offset);

    for (uint32_t i = 0; i < toc->header.entry_count && i < CL_TOC_MAX_ENTRIES; i++) {
        cl_toc_entry_t *e = &toc->entries[i];

        /* Stage-1: HAL0 schreibt nur RESIDENT Core-Segmente; RECLAIMABLE skip */
        if ((e->flags & CL_TOC_FLAG_RECLAIMABLE) != 0u) continue;

        if ((uint16_t)e->type == (uint16_t)CL_SERVICE_SEG_256) {
            if (!HAL_BOUNDS_OK(e->offset_bytes, sizeof(cl_service_seg_256_t), core_sz)) return -30;
            out->svc = (cl_service_seg_256_t*)((uint8_t*)core_base + (size_t)e->offset_bytes);
            out->slot_hal = cl_service_slot(out->svc, CL_SVC_HAL0);
        } else if ((uint16_t)e->type == (uint16_t)CL_TIME_SEG_256) {
            if (!HAL_BOUNDS_OK(e->offset_bytes, sizeof(cl_time_seg_256_t), core_sz)) return -30;
            out->time = (cl_time_seg_256_t*)((uint8_t*)core_base + (size_t)e->offset_bytes);
            out->e_time = e;
        } else if ((uint16_t)e->type == (uint16_t)CL_CPU_SEG_1024) {
            if (!HAL_BOUNDS_OK(e->offset_bytes, sizeof(cl_cpu_seg_1024_t), core_sz)) return -30;
            out->cpu0 = (cl_cpu_seg_1024_t*)((uint8_t*)core_base + (size_t)e->offset_bytes);
            out->e_cpu0 = e;
        } else if ((uint16_t)e->type == (uint16_t)CL_NIC_SEG_512) {
            if (!HAL_BOUNDS_OK(e->offset_bytes, sizeof(cl_nic_seg_512_t), core_sz)) return -30;
            out->nic0 = (cl_nic_seg_512_t*)((uint8_t*)core_base + (size_t)e->offset_bytes);
            out->e_nic0 = e;
        }
    }

    if (!out->svc || !out->slot_hal) return -10;
    return 0;
}

/* ---------------- x86 CPUID best-effort ---------------- */
#if defined(__x86_64__) || defined(_M_X64)
static void cpuid(uint32_t leaf, uint32_t subleaf,
                  uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx)
{
#if defined(__clang__) || defined(__GNUC__)
    uint32_t a,b,c,d;
    __asm__ volatile("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(leaf), "c"(subleaf));
    *eax=a; *ebx=b; *ecx=c; *edx=d;
#else
    *eax=*ebx=*ecx=*edx=0;
#endif
}
#endif

int main(void) {
    void *core = NULL;
    size_t core_sz = 0;

    int rc = map_core_rw(&core, &core_sz);
    if (rc != 0) {
        printf("hal0: cannot map core shm: rc=%d errno=%d (%s)\n", rc, errno, strerror(errno));
        return 2;
    }

    hal_loc_t loc;
    rc = hal_locate(core, core_sz, &loc);
    if (rc != 0) {
        printf("hal0: locate failed rc=%d\n", rc);
        return 3;
    }

    uint64_t epoch_ctr = 1;

    for (;;) {
        /* Heartbeat */
        atomic_store(&loc.slot_hal->last_heartbeat_ns, now_ns());

        /* ---------------- TIME ---------------- */
        if (loc.time) {
            uint64_t t = now_ns();

            atomic_store(&loc.time->hot.mono_now_ns, t);
            atomic_store(&loc.time->hot.wall_now_ns, t);

            /* boot_id_ns nur initial setzen (Marker) */
            uint64_t bid = atomic_load(&loc.time->hot.boot_id_ns);
            if (bid == 0ull) {
                atomic_store(&loc.time->hot.boot_id_ns, t);
            }

            /* src/qual liegen in deinem TIME Layout im cold */
            loc.time->cold.src  = (uint8_t)CL_SRC_OS_IFACE;
            loc.time->cold.qual = (uint8_t)CL_QUAL_BEST_EFFORT;

            /* Commit (SSOT): TOC epoch publish ist der Segment-Commit-Marker */
            if (loc.e_time) cl_commit_epoch_store_release(loc.e_time, epoch_ctr++);
        }

        /* ---------------- CPU0 ---------------- */
        if (loc.cpu0) {
            atomic_store(&loc.cpu0->hot_a.reason_code, 0u);
            atomic_store(&loc.cpu0->hot_a.constraint_flags, 0u);

#if defined(__x86_64__) || defined(_M_X64)
            uint32_t a,b,c,d;
            cpuid(1,0,&a,&b,&c,&d);
            uint32_t sse42 = (c >> 20) & 1u;

            cpuid(7,0,&a,&b,&c,&d);
            uint32_t avx2    = (b >> 5) & 1u;
            uint32_t avx512f = (b >> 16) & 1u;

            uint32_t mask = (sse42 ? 1u : 0u) | (avx2 ? 2u : 0u) | (avx512f ? 4u : 0u);

            atomic_store(&loc.cpu0->hot_b.isa_possible_mask, mask);
            atomic_store(&loc.cpu0->hot_b.isa_effective, mask);

            /* Stage-1: 1 = "active-like" (später echte enum) */
            atomic_store(&loc.cpu0->hot_b.isa_state, (uint16_t)1u);
            atomic_store(&loc.cpu0->hot_b.simd_effective, (uint16_t)1u);

            loc.cpu0->cold_a.src  = (uint8_t)CL_SRC_OS_IFACE;
            loc.cpu0->cold_a.qual = (uint8_t)CL_QUAL_BEST_EFFORT;
#else
            atomic_store(&loc.cpu0->hot_b.isa_possible_mask, 0u);
            atomic_store(&loc.cpu0->hot_b.isa_effective, 0u);
            atomic_store(&loc.cpu0->hot_b.isa_state, (uint16_t)0u);
            atomic_store(&loc.cpu0->hot_b.simd_effective, (uint16_t)0u);

            loc.cpu0->cold_a.src  = (uint8_t)CL_SRC_OS_IFACE;
            loc.cpu0->cold_a.qual = (uint8_t)CL_QUAL_UNSUPPORTED;
#endif

            if (loc.e_cpu0) cl_commit_epoch_store_release(loc.e_cpu0, epoch_ctr++);
        }

        /* ---------------- NIC0 ---------------- */
        if (loc.nic0) {
            atomic_store(&loc.nic0->hot.reason_code, 0u);

            /* src/qual liegen in deinem NIC Layout im cold_a */
            loc.nic0->cold_a.src  = (uint8_t)CL_SRC_OS_IFACE;
            loc.nic0->cold_a.qual = (uint8_t)CL_QUAL_BEST_EFFORT;

            if (loc.e_nic0) cl_commit_epoch_store_release(loc.e_nic0, epoch_ctr++);
        }

        struct timespec ts = {0};
        ts.tv_nsec = 250000000L;
        nanosleep(&ts, NULL);
    }
}


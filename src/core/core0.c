#define _POSIX_C_SOURCE 200809L
/*
 * ============================================================================
 * CommanderLink – CORE0 (Stage-1 Minimal Endausbau) – SSOT (FINAL)
 * Datei: src/core/core0.c
 * ============================================================================
 *
 * Stage-1 Pflichten (MERGE GATE v1.0):
 *   - Identity Authority (P0):
 *       VALID nur wenn state==VALID und Tags != 0
 *       NICHT blind auf INVALID überschreiben (cld ist Producer)
 *   - TOC entry epoch publish (u64, Release) für Identity (+ Service empfohlen)
 *   - redirect_allowed Truth im Service-Global:
 *       Stage-1 bleibt konservativ 0 (Trust/Failsafe fehlen)
 *   - Service Heartbeat stabil (Slot korrekt, driftfest)
 *
 * SSOT-Details:
 *   - Service slot mapping darf NICHT hart verdrahtet sein (driftgefährlich):
 *     -> svc_slot() identisch zu cld, nutze CL_SVC_CORE0
 *   - TOC epoch publish: Release-Store auf e->epoch (Commit-Marker)
 * ============================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdatomic.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>      /* O_RDWR */
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>

#include "cl_runtime/cl_runtime.h"
#include "cl_runtime/cl_abi_fingerprint.h"
#include "cl_runtime/cl_service_seg_256.h"
#include "cl_runtime/cl_identity_seg_256.h"
#include "cl_runtime/cl_commit.h"
#include "cl_manifest/cl_enums.h"

/* ---------------- Zeit ---------------- */
static uint64_t now_ns(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) return 0;
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

/* ---------------- Service slot mapping (driftfest, identisch zu cld) ------ */
static cl_service_slot_32_t* svc_slot(cl_service_seg_256_t *s, cl_service_id_t id) {
    switch (id) {
        case CL_SVC_CORE0:   return &s->g1.s0;
        case CL_SVC_HAL0:    return &s->g1.s1;
        case CL_SVC_LINK0:   return &s->g2.s2;
        case CL_SVC_FLOW0:   return &s->g2.s3;
        case CL_SVC_ORACLE0: return &s->g3.s4;
        case CL_SVC_MONITOR: return &s->g3.s5;
        default: return NULL;
    }
}

/* ---------------- Locator ---------------- */
typedef struct core_loc {
    cl_root_t *root;
    cl_toc_t  *toc;

    cl_toc_entry_t *e_service;
    cl_toc_entry_t *e_identity;

    cl_toc_entry_t *e_time;
    cl_toc_entry_t *e_cpu0;
    cl_toc_entry_t *e_nic0;

    cl_service_seg_256_t  *svc;
    cl_identity_seg_256_t *ids;

    cl_time_seg_256_t      *time;
    cl_cpu_seg_1024_t      *cpu0;
    cl_nic_seg_512_t       *nic0;

    cl_service_slot_32_t  *slot_core;
    cl_service_slot_32_t  *slot_hal;
} core_loc_t;

static int core_locate(void *core_base, size_t core_sz, core_loc_t *out) {
    memset(out, 0, sizeof(*out));

    out->root = (cl_root_t*)core_base;

    /* Root sanity (minimal) */
    if (out->root->root_magic != CL_ROOT_MAGIC_U32) return -10;
    if (out->root->schema_version != CL_SCHEMA_VERSION_U32) return -11;
    if (out->root->endian_magic != CL_ENDIAN_MAGIC_U32) return -12;

    /* TOC bounds */
    if (out->root->toc_size < sizeof(cl_toc_header_t)) return -13;
    if ((uint64_t)out->root->toc_offset + (uint64_t)out->root->toc_size > (uint64_t)core_sz) return -14;

    out->toc = (cl_toc_t*)((uint8_t*)core_base + (size_t)out->root->toc_offset);
    if (out->toc->header.entry_count > CL_TOC_MAX_ENTRIES) return -15;

    for (uint32_t i = 0; i < out->toc->header.entry_count; i++) {
        cl_toc_entry_t *e = &out->toc->entries[i];

        /* Resident offsets sind Core-Base relativ; RECLAIMABLE überspringen */
        if ((e->flags & CL_TOC_FLAG_RECLAIMABLE) != 0u) continue;

        if ((uint16_t)e->type == (uint16_t)CL_SERVICE_SEG_256 && e->stride_bytes == 256u && e->count == 1u) {
            if ((uint64_t)e->offset_bytes + 256ull <= (uint64_t)core_sz) {
                out->e_service = e;
                out->svc = (cl_service_seg_256_t*)((uint8_t*)core_base + (size_t)e->offset_bytes);
            }
        } else if ((uint16_t)e->type == (uint16_t)CL_IDENTITY_SEG_256 && e->stride_bytes == 256u && e->count == 1u) {
            if ((uint64_t)e->offset_bytes + 256ull <= (uint64_t)core_sz) {
                out->e_identity = e;
                out->ids = (cl_identity_seg_256_t*)((uint8_t*)core_base + (size_t)e->offset_bytes);
            }
        } else if ((uint16_t)e->type == (uint16_t)CL_TIME_SEG_256 && e->stride_bytes == 256u && e->count == 1u) {
            if ((uint64_t)e->offset_bytes + 256ull <= (uint64_t)core_sz) {
                out->e_time = e;
                out->time = (cl_time_seg_256_t*)((uint8_t*)core_base + (size_t)e->offset_bytes);
            }
        } else if ((uint16_t)e->type == (uint16_t)CL_CPU_SEG_1024 && e->stride_bytes == 1024u && e->count == 1u) {
            if ((uint64_t)e->offset_bytes + 1024ull <= (uint64_t)core_sz) {
                out->e_cpu0 = e;
                out->cpu0 = (cl_cpu_seg_1024_t*)((uint8_t*)core_base + (size_t)e->offset_bytes);
            }
        } else if ((uint16_t)e->type == (uint16_t)CL_NIC_SEG_512 && e->stride_bytes == 512u && e->count == 1u) {
            if ((uint64_t)e->offset_bytes + 512ull <= (uint64_t)core_sz) {
                out->e_nic0 = e;
                out->nic0 = (cl_nic_seg_512_t*)((uint8_t*)core_base + (size_t)e->offset_bytes);
            }
        }
    }

    if (!out->svc || !out->ids || !out->e_identity) return -20;

    out->slot_core = svc_slot(out->svc, CL_SVC_CORE0);
    if (!out->slot_core) return -21;

    out->slot_hal = svc_slot(out->svc, CL_SVC_HAL0);
    if (!out->slot_hal) return -22;

    return 0;
}

/* ---------------- Identity VALID Definition (Stage-1) ---------------- */
static int identity_is_valid(const cl_identity_seg_256_t *ids) {
    const uint32_t st = atomic_load_explicit(&ids->hot.identity_state, memory_order_acquire);
    const uint64_t node = (uint64_t)ids->hot.node_tag64;
    const uint64_t mesh = (uint64_t)ids->hot.mesh_tag64;

    if (st != (uint32_t)CL_IDENTITY_VALID) return 0;
    if (node == 0ull) return 0;
    if (mesh == 0ull) return 0;
    return 1;
}

int main(void) {
    /* Unbuffered stdout: nur bounded prints (Start + Zustandswechsel) */
    setvbuf(stdout, NULL, _IONBF, 0);

    void *core = NULL;
    size_t core_sz = 0;

    int rc = map_core_rw(&core, &core_sz);
    if (rc != 0) {
        printf("core0: map core shm failed rc=%d errno=%d (%s)\n", rc, errno, strerror(errno));
        return 2;
    }

    core_loc_t loc;
    rc = core_locate(core, core_sz, &loc);
    if (rc != 0) {
        printf("core0: locate failed rc=%d\n", rc);
        return 3;
    }

    printf("core0: started pid=%d\n", (int)getpid());

    /* Defense-in-depth: ABI muss zum Build passen (cld macht failfast). */
    const uint64_t want_abi = cl_abi_fingerprint_u64();
    if (loc.root->abi_layout_checksum != want_abi) {
        printf("core0: ABI mismatch: shm=0x%016" PRIx64 " build=0x%016" PRIx64 " -> STOP\n",
               loc.root->abi_layout_checksum, want_abi);
        return 5;
    }

    uint64_t epoch_ctr = 1;
    int last_valid = -1;

    uint64_t last_e_time = 0, last_e_cpu = 0, last_e_nic = 0;
    uint64_t last_e_time_ts = 0, last_e_cpu_ts = 0, last_e_nic_ts = 0;

    for (;;) {
        /* Heartbeat (Stage-1 Pflicht) */
        uint64_t t = now_ns();
        if (t == 0) {
            /* harte Realität: keine Zeitbasis -> exit */
            printf("core0: clock_gettime failed\n");
            return 4;
        }
        atomic_store_explicit(&loc.slot_core->last_heartbeat_ns, t, memory_order_relaxed);

        /* HAL liveness gate (Stage-1): wenn HAL nicht tickt -> P0 Reason setzen. */
        uint64_t hal_hb = atomic_load_explicit(&loc.slot_hal->last_heartbeat_ns, memory_order_acquire);
        const uint64_t hal_age_ns = (hal_hb == 0ull) ? UINT64_MAX : (t - hal_hb);
        const int hal_alive = (hal_hb != 0ull) && (hal_age_ns <= 1000000000ull); /* 1s */

        /* HAL epoch freshness (Stage-1 contract): epochs must advance periodically.
         * Threshold is intentionally conservative for early bring-up.
         */
        int hal_stale = 0;
        const uint64_t stale_ns = 2000000000ull; /* 2s */

        if (loc.e_time) {
            const uint64_t e = cl_commit_epoch_load_acquire(loc.e_time);
            if (e == 0ull) { hal_stale = 1; }
            else if (e != last_e_time) { last_e_time = e; last_e_time_ts = t; }
            else if (last_e_time_ts != 0ull && (t - last_e_time_ts) > stale_ns) { hal_stale = 1; }
        }
        if (loc.e_cpu0) {
            const uint64_t e = cl_commit_epoch_load_acquire(loc.e_cpu0);
            if (e == 0ull) { hal_stale = 1; }
            else if (e != last_e_cpu) { last_e_cpu = e; last_e_cpu_ts = t; }
            else if (last_e_cpu_ts != 0ull && (t - last_e_cpu_ts) > stale_ns) { hal_stale = 1; }
        }
        if (loc.e_nic0) {
            const uint64_t e = cl_commit_epoch_load_acquire(loc.e_nic0);
            if (e == 0ull) { hal_stale = 1; }
            else if (e != last_e_nic) { last_e_nic = e; last_e_nic_ts = t; }
            else if (last_e_nic_ts != 0ull && (t - last_e_nic_ts) > stale_ns) { hal_stale = 1; }
        }

        /* Identity check */
        int valid = identity_is_valid(loc.ids);

        if (valid != last_valid) {
            printf("core0: identity_valid=%d\n", valid);
            last_valid = valid;
        }

        if (!valid || !hal_alive || hal_stale) {
            /* P0 Gate: CORE setzt redirect_allowed konservativ 0.
             * Identity-State NICHT blind überschreiben (cld ist Producer).
             * Reason kann gesetzt werden, ohne state zu zerstören.
             */
            const uint32_t why = (!valid) ? (uint32_t)CL_REASON_P0_IDENTITY_INVALID
                              : (!hal_alive) ? (uint32_t)CL_REASON_P0_HAL_MISSING
                                             : (uint32_t)CL_REASON_P0_HAL_STALE;
            atomic_store_explicit(&loc.ids->hot.reason_code, why, memory_order_relaxed);

            atomic_store_explicit(&loc.svc->g0.redirect_allowed, 0u, memory_order_relaxed);
        } else {
            /* Identity valid: Reason clear */
            atomic_store_explicit(&loc.ids->hot.reason_code, (uint32_t)CL_REASON_NONE, memory_order_relaxed);

            /* Publish epochs once (Cut-Point) */
            uint64_t e_id = cl_commit_epoch_load_acquire(loc.e_identity);
            if (e_id == 0u) cl_commit_epoch_store_release(loc.e_identity, epoch_ctr++);

            if (loc.e_service) {
                uint64_t e_sv = cl_commit_epoch_load_acquire(loc.e_service);
                if (e_sv == 0u) cl_commit_epoch_store_release(loc.e_service, epoch_ctr++);
            }

            /* Stage-1 bleibt konservativ: redirect_allowed = 0 (Trust/Failsafe fehlen) */
            atomic_store_explicit(&loc.svc->g0.redirect_allowed, 0u, memory_order_relaxed);

            /* Ingest is validation-only in Stage-1; payload use comes later. */
        }

        struct timespec ts = {0};
        ts.tv_nsec = 250000000L;
        nanosleep(&ts, NULL);
    }
}


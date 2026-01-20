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
#include <string.h>
#include <errno.h>
#include <fcntl.h>      /* O_RDWR */
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>

#include "cl_runtime/cl_runtime.h"
#include "cl_runtime/cl_service_seg_256.h"
#include "cl_runtime/cl_identity_seg_256.h"
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

    cl_service_seg_256_t  *svc;
    cl_identity_seg_256_t *ids;

    cl_service_slot_32_t  *slot_core;
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
        }
    }

    if (!out->svc || !out->ids || !out->e_identity) return -20;

    out->slot_core = svc_slot(out->svc, CL_SVC_CORE0);
    if (!out->slot_core) return -21;

    return 0;
}

/* ---------------- Epoch Publish (u64 Release) ----------------
 * SSOT: epoch ist u64. Wir publizieren via Release-Store als Commit.
 */
static void toc_epoch_publish_u64(cl_toc_entry_t *e, uint64_t new_epoch) {
    atomic_store_explicit((_Atomic(uint64_t) *)&e->epoch, new_epoch, memory_order_release);
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

    uint64_t epoch_ctr = 1;
    int last_valid = -1;

    for (;;) {
        /* Heartbeat (Stage-1 Pflicht) */
        uint64_t t = now_ns();
        if (t == 0) {
            /* harte Realität: keine Zeitbasis -> exit */
            printf("core0: clock_gettime failed\n");
            return 4;
        }
        atomic_store_explicit(&loc.slot_core->last_heartbeat_ns, t, memory_order_relaxed);

        /* Identity check */
        int valid = identity_is_valid(loc.ids);

        if (valid != last_valid) {
            printf("core0: identity_valid=%d\n", valid);
            last_valid = valid;
        }

        if (!valid) {
            /* P0 Gate: CORE setzt redirect_allowed konservativ 0.
             * Identity-State NICHT blind überschreiben (cld ist Producer).
             * Reason kann gesetzt werden, ohne state zu zerstören.
             */
            atomic_store_explicit(&loc.ids->hot.reason_code,
                                  (uint32_t)CL_REASON_P0_IDENTITY_INVALID,
                                  memory_order_relaxed);

            atomic_store_explicit(&loc.svc->g0.redirect_allowed, 0u, memory_order_relaxed);
        } else {
            /* Identity valid: Reason clear */
            atomic_store_explicit(&loc.ids->hot.reason_code, (uint32_t)CL_REASON_NONE, memory_order_relaxed);

            /* Publish epochs once (Cut-Point) */
            uint64_t e_id = atomic_load_explicit((_Atomic(uint64_t) *)&loc.e_identity->epoch, memory_order_acquire);
            if (e_id == 0u) toc_epoch_publish_u64(loc.e_identity, epoch_ctr++);

            if (loc.e_service) {
                uint64_t e_sv = atomic_load_explicit((_Atomic(uint64_t) *)&loc.e_service->epoch, memory_order_acquire);
                if (e_sv == 0u) toc_epoch_publish_u64(loc.e_service, epoch_ctr++);
            }

            /* Stage-1 bleibt konservativ: redirect_allowed = 0 (Trust/Failsafe fehlen) */
            atomic_store_explicit(&loc.svc->g0.redirect_allowed, 0u, memory_order_relaxed);
        }

        struct timespec ts = {0};
        ts.tv_nsec = 250000000L;
        nanosleep(&ts, NULL);
    }
}


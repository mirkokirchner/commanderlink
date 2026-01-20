#define _POSIX_C_SOURCE 200809L
/*
 * ==========================================================================
 * CommanderLink – FLOW0 (Stage-1) – Service Heartbeat Contract
 * File: src/flow/flow0.c
 * ==========================================================================
 *
 * Stage-1 Pflicht (Contract Hygiene):
 *   - FLOW0 existiert als Prozess und publiziert seinen Heartbeat deterministisch
 *   - Keine Policy, kein Redirect, keine Dataplane-Logik
 *   - Kein Slot-Drift: Service-Slot wird über SSOT Service-ID CL_SVC_FLOW0 gewählt
 *
 * SSOT:
 *   - Service-Segment: include/cl_runtime/cl_service_seg_256.h
 *   - Service-ID: CL_SVC_FLOW0 => Slot g2.s3
 *
 * Hinweis:
 *   FLOW0 wird später zur Command-Boundary (Credits/Pacing/Flow Control).
 */

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "cl_runtime/cl_runtime.h" /* SSOT Frontdoor */

/* -------------------------------------------------------------------------
 * Time (monotonic)
 * ------------------------------------------------------------------------- */
static uint64_t now_ns(void) {
    struct timespec ts;
    (void)clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

/* -------------------------------------------------------------------------
 * SHM mapping (RW)
 * ------------------------------------------------------------------------- */
static int map_core_rw(void **out_base, size_t *out_sz) {
    int fd = shm_open(CL_CORE_SHM_NAME, O_RDWR, 0);
    if (fd < 0) return -1;

    struct stat st;
    if (fstat(fd, &st) != 0) {
        close(fd);
        return -2;
    }
    if (st.st_size <= 0) {
        close(fd);
        return -3;
    }

    void *p = mmap(NULL, (size_t)st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (p == MAP_FAILED) return -4;

    *out_base = p;
    *out_sz = (size_t)st.st_size;
    return 0;
}

/* -------------------------------------------------------------------------
 * SSOT: service slot mapping (must match cld/core0)
 * ------------------------------------------------------------------------- */
static cl_service_slot_32_t *svc_slot(cl_service_seg_256_t *s, cl_service_id_t id) {
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

/* -------------------------------------------------------------------------
 * Locate Service Segment via TOC (bounds + presence)
 * ------------------------------------------------------------------------- */
static int locate_service(void *core_base, size_t core_sz,
                          cl_root_t **out_root,
                          cl_service_seg_256_t **out_svc,
                          cl_service_slot_32_t **out_slot) {
    if (!core_base || core_sz < sizeof(cl_root_t)) return -1;

    cl_root_t *root = (cl_root_t *)core_base;

    /* minimal root sanity (strict but non-fatal here; cld/core0 are the hard gates) */
    if (root->root_magic != CL_ROOT_MAGIC_U32) return -2;
    if (root->schema_version != CL_SCHEMA_VERSION_U32) return -3;
    if (root->toc_size < sizeof(cl_toc_header_t)) return -4;
    if ((uint64_t)root->toc_offset + (uint64_t)root->toc_size > (uint64_t)core_sz) return -5;

    cl_toc_header_t *th = (cl_toc_header_t *)((uint8_t *)core_base + (size_t)root->toc_offset);
    if (th->entry_count > CL_TOC_MAX_ENTRIES) return -6;

    cl_toc_entry_t *te = (cl_toc_entry_t *)((uint8_t *)th + sizeof(*th));

    cl_service_seg_256_t *svc = NULL;
    for (uint32_t i = 0; i < th->entry_count; i++) {
        const cl_toc_entry_t *e = &te[i];
        if ((uint16_t)e->type != (uint16_t)CL_SERVICE_SEG_256) continue;
        if (e->presence_flags & CL_TOC_RECLAIMED) return -7;

        /* service is RESIDENT in Stage-1; still bounds-check defensively */
        const uint64_t off = e->offset_bytes;
        const uint64_t need = (uint64_t)sizeof(cl_service_seg_256_t);
        if (off + need > (uint64_t)core_sz) return -8;

        svc = (cl_service_seg_256_t *)((uint8_t *)core_base + (size_t)off);
        break;
    }
    if (!svc) return -9;

    cl_service_slot_32_t *slot = svc_slot(svc, CL_SVC_FLOW0);
    if (!slot) return -10;

    *out_root = root;
    *out_svc = svc;
    *out_slot = slot;
    return 0;
}

int main(void) {
    void *core = NULL;
    size_t core_sz = 0;

    int rc = map_core_rw(&core, &core_sz);
    if (rc != 0) {
        fprintf(stderr, "flow0: cannot map core shm: rc=%d errno=%d (%s)\n",
                rc, errno, strerror(errno));
        return 2;
    }

    cl_root_t *root = NULL;
    cl_service_seg_256_t *svc = NULL;
    cl_service_slot_32_t *slot = NULL;
    rc = locate_service(core, core_sz, &root, &svc, &slot);
    if (rc != 0) {
        fprintf(stderr, "flow0: locate service failed rc=%d\n", rc);
        return 3;
    }

    const pid_t pid = getpid();
    fprintf(stderr, "flow0: started pid=%d\n", (int)pid);

    /* Stage-1: publish minimal liveness truth.
     * Note: cld sets expected/running/pid when it spawns us. If run manually,
     *       we still self-publish pid/running and heartbeat to the SSOT slot.
     */
    atomic_store(&slot->pid, (uint32_t)pid);
    atomic_store(&slot->running, 1u);

    for (;;) {
        atomic_store(&slot->last_heartbeat_ns, now_ns());

        /* No policy writes. No redirect. No flow cmd.
         * If you want explicit standby reason, do it via a dedicated flow segment later.
         */
        (void)root;
        (void)svc;

        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 250000000L; /* 250ms */
        nanosleep(&ts, NULL);
    }
}


#define _POSIX_C_SOURCE 200809L
/*
 * ============================================================================
 * CommanderLink – FLOW0 (Stufe-1 Minimal Endausbau) – MERGE GATE Stage 1
 * Datei: src/flow/flow0.c
 * ============================================================================
 *
 * Pflichten Stage-1:
 *   - Kein Heartbeat-Stub: mindestens Status/Reason sichtbar setzen
 *   - Keine OS-Aktionen (kein Redirect, kein Routing)
 *   - FLOW bleibt in Stage-1 im Standby, bis CORE/LINK/Trust/Dataplane existieren
 *
 * Hinweis:
 *   - Später (Stage-2) kommt hier die echte Credits/Pacing-Logik + Command Boundary.
 * ============================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdatomic.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>

#include "cl_runtime/cl_runtime.h"
#include "cl_runtime/cl_service_seg_256.h"

static uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

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

typedef struct flow_loc {
    cl_service_seg_256_t *svc;
    cl_service_slot_32_t *slot_flow;
    cl_root_t            *root;
} flow_loc_t;

static int flow_locate(void *core_base, size_t core_sz, flow_loc_t *out) {
    (void)core_sz;
    memset(out, 0, sizeof(*out));

    out->root = (cl_root_t*)core_base;

    cl_toc_t *toc = (cl_toc_t*)((uint8_t*)core_base + (size_t)out->root->toc_offset);
    for (uint32_t i = 0; i < toc->header.entry_count && i < CL_TOC_MAX_ENTRIES; i++) {
        cl_toc_entry_t *e = &toc->entries[i];
        if ((uint16_t)e->type == (uint16_t)CL_SERVICE_SEG_256) {
            out->svc = (cl_service_seg_256_t*)((uint8_t*)core_base + (size_t)e->offset_bytes);
            out->slot_flow = &out->svc->g2.s3;
            break;
        }
    }
    return (out->svc && out->slot_flow) ? 0 : -1;
}

int main(void) {
    void *core = NULL;
    size_t core_sz = 0;

    int rc = map_core_rw(&core, &core_sz);
    if (rc != 0) {
        printf("flow0: cannot map core shm: rc=%d errno=%d (%s)\n", rc, errno, strerror(errno));
        return 2;
    }

    flow_loc_t loc;
    rc = flow_locate(core, core_sz, &loc);
    if (rc != 0) {
        printf("flow0: locate failed\n");
        return 3;
    }

    for (;;) {
        /* Heartbeat */
        atomic_store(&loc.slot_flow->last_heartbeat_ns, now_ns());

        /* Stage-1 Status/Reason:
         * - FLOW ist Standby bis CORE Trust/Gates + LINK dataplane existieren.
         * - Wir setzen keinen Redirect und keine Kommandos.
         * - Reason 0 = "OK/Standby".
         *
         * Wenn du feste Reason-Codes hast, ersetze 0 durch den passenden Code.
         */
        if (loc.root) {
            /* Optional: root reason/constraint, falls Felder existieren */
            /* atomic_store(&loc.root->reason_code, 0u); */
        }

        struct timespec ts = {0};
        ts.tv_nsec = 250000000L;
        nanosleep(&ts, NULL);
    }
}


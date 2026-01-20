#define _POSIX_C_SOURCE 200809L
/*
 * ============================================================================
 * CommanderLink – LINK0 (Stufe-1 Minimal Endausbau) – MERGE GATE Stage 1
 * Datei: src/link/link0.c
 * ============================================================================
 *
 * Pflichten Stage-1:
 *   - Kein Heartbeat-Stub: mindestens Status/Reason sichtbar setzen
 *   - Redirect darf NICHT aktiv werden (Emergency Failback ist Gate)
 *   - effective_redirect bleibt 0 (LINK-only Feld im Service-Segment)
 *
 * Keine OS-Änderungen:
 *   - kein Routing, kein TUN, kein pf/ipfw, kein netlink
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

typedef struct link_loc {
    cl_service_seg_256_t *svc;
    cl_service_slot_32_t *slot_link;
    cl_root_t            *root;
} link_loc_t;

static int link_locate(void *core_base, size_t core_sz, link_loc_t *out) {
    (void)core_sz;
    memset(out, 0, sizeof(*out));

    out->root = (cl_root_t*)core_base;

    cl_toc_t *toc = (cl_toc_t*)((uint8_t*)core_base + (size_t)out->root->toc_offset);
    for (uint32_t i = 0; i < toc->header.entry_count && i < CL_TOC_MAX_ENTRIES; i++) {
        cl_toc_entry_t *e = &toc->entries[i];
        if ((uint16_t)e->type == (uint16_t)CL_SERVICE_SEG_256) {
            out->svc = (cl_service_seg_256_t*)((uint8_t*)core_base + (size_t)e->offset_bytes);
            out->slot_link = &out->svc->g2.s2;
            break;
        }
    }
    return (out->svc && out->slot_link) ? 0 : -1;
}

int main(void) {
    void *core = NULL;
    size_t core_sz = 0;

    int rc = map_core_rw(&core, &core_sz);
    if (rc != 0) {
        printf("link0: cannot map core shm: rc=%d errno=%d (%s)\n", rc, errno, strerror(errno));
        return 2;
    }

    link_loc_t loc;
    rc = link_locate(core, core_sz, &loc);
    if (rc != 0) {
        printf("link0: locate failed\n");
        return 3;
    }

    /* Stage-1: Redirect verboten -> effective_redirect bleibt 0 */
    atomic_store(&loc.svc->g0.effective_redirect, 0u);

    /* Optional: desired_redirect bleibt Wunsch, aber LINK setzt effective nicht */
    /* atomic_store(&loc.svc->g0.desired_redirect, atomic_load(&loc.svc->g0.desired_redirect)); */

    for (;;) {
        /* Heartbeat */
        atomic_store(&loc.slot_link->last_heartbeat_ns, now_ns());

        /* Status/Reason minimal sichtbar:
         * Wir setzen keinen Redirect, kein OS plumbing.
         * Reason: 0 => "OK (Stage-1 standby)". Wenn du feste Reason Codes hast, ersetze 0 durch den passenden Code.
         */
        if (loc.root) {
            /* Diese Felder existieren nicht in jedem Root-Layout; falls dein Root sie nicht hat, auskommentieren. */
            /* atomic_store(&loc.root->reason_code, 0u); */
            /* atomic_store(&loc.root->constraint_flags, 0u); */
        }

        /* effective_redirect dauerhaft 0 halten */
        atomic_store(&loc.svc->g0.effective_redirect, 0u);

        struct timespec ts = {0};
        ts.tv_nsec = 250000000L;
        nanosleep(&ts, NULL);
    }
}


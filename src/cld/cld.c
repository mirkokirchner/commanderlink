#define _POSIX_C_SOURCE 200809L
/*
 * ============================================================================
 * CommanderLink – cld (einziger Dienst/Supervisor) – Stage 1 SSOT
 * Datei: src/cld/cld.c
 * ============================================================================
 *
 * Ziel dieser Stage:
 *   - Der Dienst startet deterministisch.
 *   - SHM wird nur dann erzeugt, wenn sie fehlt/kaputt ist (Genesis-Check).
 *   - Identity wird als P0-Gate erzwungen (ohne Identity: STOP).
 *   - Service-State wird initialisiert (Forensik bleibt erhalten).
 *   - Module werden in fester Reihenfolge gestartet: core0 -> hal0 -> link0 -> flow0
 *   - ORACLE ist Stage-1: expected=0 (Oracle Mode später).
 *   - Keine Policy-Entscheidungen, nur Betrieb/Lifecycle.
 *
 * ABI/FAILFAST:
 *   - Wenn die SHM ein anderes ABI hat als der Build, und FAILFAST aktiv ist:
 *       => STOP mit klarer Meldung (clinit --recreate nötig).
 *     (Wir zerstören NICHT automatisch, weil das ein Admin-Akt ist.)
 * ============================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdatomic.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#include "cl_runtime/cl_runtime.h"
#include "cl_runtime/cl_service_seg_256.h"
#include "cl_runtime/cl_identity_seg_256.h"
#include "cl_runtime/cl_abi_fingerprint.h"   /* NEW: shared runtime ABI fingerprint */

#include "cl_identity.h" /* src/cld/ */

static uint64_t now_ns(void) {
    struct timespec ts;
    (void)clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

static const char* detect_os_tag(void) {
#if defined(__FreeBSD__)
    return "bsd";
#elif defined(__linux__)
    return "linux";
#elif defined(__sun)
    return "omnios";
#else
    return "unknown";
#endif
}

static int map_shm_ro(const char *name, void **out_base, size_t *out_sz) {
    int fd = shm_open(name, O_RDONLY, 0);
    if (fd < 0) return -1;

    struct stat st;
    if (fstat(fd, &st) != 0) { close(fd); return -2; }
    if (st.st_size <= 0)      { close(fd); return -3; }

    void *p = mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);
    if (p == MAP_FAILED) return -4;

    *out_base = p;
    *out_sz = (size_t)st.st_size;
    return 0;
}

static int map_shm_rw(const char *name, void **out_base, size_t *out_sz) {
    int fd = shm_open(name, O_RDWR, 0);
    if (fd < 0) return -1;

    struct stat st;
    if (fstat(fd, &st) != 0) { close(fd); return -2; }
    if (st.st_size <= 0)      { close(fd); return -3; }

    void *p = mmap(NULL, (size_t)st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (p == MAP_FAILED) return -4;

    *out_base = p;
    *out_sz = (size_t)st.st_size;
    return 0;
}

static void unmap_shm(void *base, size_t sz) {
    if (base && sz) (void)munmap(base, sz);
}

/* clinit einmalig ausführen (kein destroy). */
static int run_wait(const char *path, char *const argv[]) {
    pid_t pid = fork();
    if (pid < 0) return -1;
    if (pid == 0) {
        execv(path, argv);
        _exit(127);
    }
    int st = 0;
    if (waitpid(pid, &st, 0) < 0) return -2;
    if (WIFEXITED(st)) return (int)WEXITSTATUS(st);
    if (WIFSIGNALED(st)) return 128 + (int)WTERMSIG(st);
    return -3;
}

/* Service slot pointer: ordnet Service-ID auf festen Slot in 256B Segment. */
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

/* Genesis-check (driftfest):
 * - Root basics ok?
 * - TOC bounds ok?
 * - Pflichtsegmente vorhanden und innerhalb Core-SHM bounds?
 *   Stage-1 Pflicht: Service + Identity + FlowCmd
 *
 * Hinweis: ABI-mismatch wird NICHT hier hart beendet – das machen wir nach RW-map,
 *         weil wir dann root_flags (FAILFAST) zuverlässig auswerten.
 */
static int genesis_check_ro(void) {
    void *base = NULL;
    size_t sz = 0;

    int rc = map_shm_ro(CL_CORE_SHM_NAME, &base, &sz);
    if (rc != 0) return 0;

    const cl_root_t *r = (const cl_root_t*)base;

    if (r->root_magic     != CL_ROOT_MAGIC_U32)     { unmap_shm(base, sz); return 0; }
    if (r->schema_version != CL_SCHEMA_VERSION_U32) { unmap_shm(base, sz); return 0; }
    if (r->endian_magic   != CL_ENDIAN_MAGIC_U32)   { unmap_shm(base, sz); return 0; }

    if (r->toc_size < sizeof(cl_toc_header_t))      { unmap_shm(base, sz); return 0; }
    if ((uint64_t)r->toc_offset + (uint64_t)r->toc_size > (uint64_t)sz) {
        unmap_shm(base, sz); return 0;
    }

    const cl_toc_t *toc = (const cl_toc_t*)((const uint8_t*)base + (size_t)r->toc_offset);
    if (toc->header.entry_count > CL_TOC_MAX_ENTRIES) { unmap_shm(base, sz); return 0; }

    int have_service = 0, have_identity = 0, have_flowcmd = 0;

    for (uint32_t i = 0; i < toc->header.entry_count; i++) {
        const cl_toc_entry_t *e = &toc->entries[i];

        if ((uint16_t)e->type == (uint16_t)CL_SERVICE_SEG_256 &&
            e->stride_bytes == 256u && e->count == 1u &&
            (uint64_t)e->offset_bytes + 256ull <= (uint64_t)sz)
            have_service = 1;

        if ((uint16_t)e->type == (uint16_t)CL_IDENTITY_SEG_256 &&
            e->stride_bytes == 256u && e->count == 1u &&
            (uint64_t)e->offset_bytes + 256ull <= (uint64_t)sz)
            have_identity = 1;

        if ((uint16_t)e->type == (uint16_t)CL_FLOW_CMD_SEG_256 &&
            e->stride_bytes == 256u && e->count == 1u &&
            (uint64_t)e->offset_bytes + 256ull <= (uint64_t)sz)
            have_flowcmd = 1;
    }

    unmap_shm(base, sz);
    return (have_service && have_identity && have_flowcmd) ? 1 : 0;
}

static uint64_t backoff_ns(uint32_t pow) {
    if (pow == 0) return 0;
    if (pow == 1) return 1000000000ull;
    if (pow == 2) return 2000000000ull;
    if (pow == 3) return 4000000000ull;
    if (pow == 4) return 8000000000ull;
    return 10000000000ull;
}

typedef struct cl_proc {
    const char *name;
    cl_service_id_t sid;
    char path[256];

    pid_t pid;
    uint32_t backoff_pow;
    uint64_t next_restart_ns;
} cl_proc_t;

static pid_t spawn_process(const char *path, const char *name) {
    pid_t pid = fork();
    if (pid < 0) return -1;
    if (pid == 0) {
        execl(path, name, (char*)NULL);
        _exit(127);
    }
    return pid;
}

/* waitpid drain: holt alle gestorbenen Kinder in einem Loop ab (keine Stauung). */
static void reap_all(cl_proc_t *procs, size_t n, cl_service_seg_256_t *svc) {
    for (;;) {
        int status = 0;
        pid_t dead = waitpid(-1, &status, WNOHANG);
        if (dead <= 0) break;

        for (size_t i = 0; i < n; i++) {
            if (procs[i].pid == dead) {
                cl_service_slot_32_t *sl = svc_slot(svc, procs[i].sid);

                atomic_store(&sl->running, 0u);
                atomic_store(&sl->pid, 0u);

                if (WIFEXITED(status)) {
                    atomic_store(&sl->last_exit_code, (int32_t)WEXITSTATUS(status));
                    atomic_store(&sl->last_signal, 0);
                } else if (WIFSIGNALED(status)) {
                    atomic_store(&sl->last_exit_code, -1);
                    atomic_store(&sl->last_signal, (int32_t)WTERMSIG(status));
                }

                atomic_fetch_add(&sl->crash_count, 1u);

                procs[i].pid = -1;
                if (procs[i].backoff_pow < 10) procs[i].backoff_pow++;
                procs[i].next_restart_ns = now_ns() + backoff_ns(procs[i].backoff_pow);
                break;
            }
        }
    }
}

int main(void) {
    const char *os = detect_os_tag();

    char clinit_path[256];
    snprintf(clinit_path, sizeof(clinit_path), "bin/%s/clinit", os);

    /* 1) Genesis: wenn SHM fehlt oder Layout nicht passt -> clinit einmalig */
    if (!genesis_check_ro()) {
        printf("cld: genesis missing/invalid -> running clinit once\n");
        char *args[] = { (char*)clinit_path, NULL };
        int rc = run_wait(clinit_path, args);
        if (rc != 0) {
            printf("cld: clinit failed rc=%d\n", rc);
            return 2;
        }
        if (!genesis_check_ro()) {
            printf("cld: genesis still invalid after clinit\n");
            return 3;
        }
    }

    /* 2) Core-SHM RW mapping */
    void *core_base = NULL;
    size_t core_sz = 0;
    int rc = map_shm_rw(CL_CORE_SHM_NAME, &core_base, &core_sz);
    if (rc != 0) {
        printf("cld: cannot map core shm RW: rc=%d errno=%d (%s)\n", rc, errno, strerror(errno));
        return 4;
    }

    cl_root_t *root = (cl_root_t*)core_base;

    /* 2a) ABI-Failfast: wenn ABI nicht passt, STOP (kein Auto-Recreate) */
    const uint64_t want = cl_abi_fingerprint_u64();
    if (root->abi_layout_checksum != want) {
        const uint32_t ff = (uint32_t)(root->root_flags & CL_ROOT_FLAG_FAILFAST_ABI);
        printf("cld: ABI mismatch: shm=0x%016" PRIx64 " build=0x%016" PRIx64 "\n",
               root->abi_layout_checksum, want);

        if (ff) {
            printf("cld: FAILFAST_ABI set -> STOP. Run: clinit --recreate (and restart cld)\n");
            unmap_shm(core_base, core_sz);
            return 6;
        }
        printf("cld: WARN: FAILFAST_ABI not set; continuing (not recommended)\n");
    }

    cl_toc_t  *toc  = (cl_toc_t*)((uint8_t*)core_base + (size_t)root->toc_offset);

    /* 3) Service-Segment finden (SSOT über TOC) */
    cl_service_seg_256_t *svc = NULL;
    for (uint32_t i = 0; i < toc->header.entry_count; i++) {
        cl_toc_entry_t *e = &toc->entries[i];
        if ((uint16_t)e->type == (uint16_t)CL_SERVICE_SEG_256) {
            svc = (cl_service_seg_256_t*)((uint8_t*)core_base + (size_t)e->offset_bytes);
            break;
        }
    }
    if (!svc) {
        printf("cld: service segment not found\n");
        unmap_shm(core_base, core_sz);
        return 5;
    }

    /* 4) Identity Gate (P0): identity.bin load/create + publish in SHM */
    struct cl_identity_out id;
    memset(&id, 0, sizeof(id));

    errno = 0;
    int idrc = cl_identity_load_or_create(&id);
    if (idrc != 0) {
        /* Contract: kein Fail ohne Grund -> errno ausgeben */
        printf("cld: identity load/create failed rc=%d errno=%d (%s) (P0 stop)\n",
               idrc, errno, strerror(errno));
        unmap_shm(core_base, core_sz);
        return 10;
    }

    errno = 0;
    int prc = cl_identity_publish_to_shm(core_base, core_sz, &id);
    if (prc != 0) {
        printf("cld: identity publish to shm failed rc=%d errno=%d (%s) (P0 stop)\n",
               prc, errno, strerror(errno));
        unmap_shm(core_base, core_sz);
        return 11;
    }

    /* 5) Service init (Forensik NICHT löschen) */
    atomic_fetch_add(&svc->g0.boot_epoch, 1);
    atomic_store(&svc->g0.boot_id_ns, now_ns());
    atomic_store(&svc->g0.service_count, (uint32_t)CL_SVC_MAX);

    /* Stage-1 Defaults: ACCELERATE / desired=0 / effective=0 / allowed=0 */
    atomic_store(&svc->g0.desired_profile,  (uint32_t)CL_DESIRED_PROFILE_ACCELERATE);
    atomic_store(&svc->g0.desired_redirect, 0u);
    atomic_store(&svc->g0.effective_redirect, 0u);
    atomic_store(&svc->g0.redirect_allowed, 0u);

    /* expected setzen (Stage-1: ORACLE=0, MONITOR=0) */
    for (int i = 0; i < (int)CL_SVC_MAX; i++) {
        cl_service_slot_32_t *sl = svc_slot(svc, (cl_service_id_t)i);
        if (!sl) continue;

        if (i == (int)CL_SVC_MONITOR)      atomic_store(&sl->expected, 0u);
        else if (i == (int)CL_SVC_ORACLE0) atomic_store(&sl->expected, 0u);
        else                               atomic_store(&sl->expected, 1u);

        atomic_store(&sl->running, 0u);
        atomic_store(&sl->pid, 0u);
        atomic_store(&sl->last_heartbeat_ns, 0u);
    }

    /* 6) Startreihenfolge Stage-1 */
    cl_proc_t procs[] = {
        { "core0", CL_SVC_CORE0, {0}, -1, 0, 0 },
        { "hal0",  CL_SVC_HAL0,  {0}, -1, 0, 0 },
        { "link0", CL_SVC_LINK0, {0}, -1, 0, 0 },
        { "flow0", CL_SVC_FLOW0, {0}, -1, 0, 0 },
    };

    snprintf(procs[0].path, sizeof(procs[0].path), "bin/%s/core0", os);
    snprintf(procs[1].path, sizeof(procs[1].path), "bin/%s/hal0", os);
    snprintf(procs[2].path, sizeof(procs[2].path), "bin/%s/link0", os);
    snprintf(procs[3].path, sizeof(procs[3].path), "bin/%s/flow0", os);

    for (size_t i = 0; i < sizeof(procs)/sizeof(procs[0]); i++) {
        cl_service_slot_32_t *sl = svc_slot(svc, procs[i].sid);

        pid_t pid = spawn_process(procs[i].path, procs[i].name);
        if (pid < 0) {
            printf("cld: spawn %s failed: errno=%d (%s)\n", procs[i].name, errno, strerror(errno));
            atomic_store(&sl->running, 0u);
            atomic_store(&sl->pid, 0u);
            atomic_store(&sl->last_exit_code, -1);
            atomic_store(&sl->last_signal, 0);

            procs[i].pid = -1;
            procs[i].backoff_pow = 1;
            procs[i].next_restart_ns = now_ns() + backoff_ns(procs[i].backoff_pow);
            continue;
        }

        procs[i].pid = pid;
        atomic_store(&sl->running, 1u);
        atomic_store(&sl->pid, (uint32_t)pid);
        procs[i].backoff_pow = 0;
        procs[i].next_restart_ns = 0;
    }

    /* 7) Supervisor loop */
    const uint64_t heartbeat_timeout_ns = 5ull * 1000000000ull;
    const uint64_t poll_ns = 250ull * 1000000ull;

    for (;;) {
        reap_all(procs, sizeof(procs)/sizeof(procs[0]), svc);

        uint64_t tnow = now_ns();

        for (size_t i = 0; i < sizeof(procs)/sizeof(procs[0]); i++) {
            cl_service_slot_32_t *sl = svc_slot(svc, procs[i].sid);
            uint32_t expected = atomic_load(&sl->expected);
            if (!expected) continue;

            if (procs[i].pid > 0) {
                uint64_t hb = atomic_load(&sl->last_heartbeat_ns);
                if (hb > 0 && (tnow - hb) > heartbeat_timeout_ns) {
                    printf("cld: %s heartbeat timeout; kill pid=%d\n", procs[i].name, (int)procs[i].pid);

                    /* Stage-1: effective_redirect bleibt 0, daher kein OS-Blackhole möglich.
                       Vor Aktivierung von Redirect: LINK-owned emergency failback implementieren. */
                    (void)kill(procs[i].pid, SIGKILL);
                }
            } else {
                if (procs[i].next_restart_ns != 0 && tnow >= procs[i].next_restart_ns) {
                    pid_t pid = spawn_process(procs[i].path, procs[i].name);
                    if (pid > 0) {
                        procs[i].pid = pid;
                        atomic_store(&sl->running, 1u);
                        atomic_store(&sl->pid, (uint32_t)pid);
                        atomic_store(&sl->last_heartbeat_ns, 0u);
                        procs[i].next_restart_ns = 0;
                    } else {
                        if (procs[i].backoff_pow < 10) procs[i].backoff_pow++;
                        procs[i].next_restart_ns = tnow + backoff_ns(procs[i].backoff_pow);
                    }
                }
            }
        }

        struct timespec ts;
        ts.tv_sec  = (time_t)(poll_ns / 1000000000ull);
        ts.tv_nsec = (long)(poll_ns % 1000000000ull);
        nanosleep(&ts, NULL);
    }
}


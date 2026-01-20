#define _POSIX_C_SOURCE 200809L
/* ============================================================================
 * CommanderLink – Identity Gate (cld-only) – FINAL
 * Datei: src/cld/cl_identity.c
 * ============================================================================
 *
 * SSOT:
 *   - identity.bin ist fixed-size, CRC32C-geschützt, reserved bytes müssen 0 sein.
 *   - Pfade:
 *       Linux/illumos: /var/lib/commanderlink/identity.bin
 *       FreeBSD/GhostBSD: /var/db/commanderlink/identity.bin
 *   - cld erzeugt IDs nur beim First Boot (Datei fehlt).
 *   - cld published in CL_IDENTITY_SEG_256.
 *   - cld bump’t KEINE TOC entry epoch (CORE published).
 *
 * Hinweis:
 *   - HW fingerprint ist best-effort und nur Input für HASH64 zusammen mit salt.
 *   - Keine Telemetrie, keine Heuristik.
 * ============================================================================
 */

#include "cl_identity.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdatomic.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/utsname.h>

#if defined(__FreeBSD__)
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

#include "cl_manifest/cl_identity_file.h"
#include "cl_runtime/cl_runtime.h"
#include "cl_runtime/cl_identity_seg_256.h"
#include "cl_runtime/cl_quality.h"

/* ---------------- Pfade (SSOT) ---------------- */
static const char* identity_path(void) {
#if defined(__FreeBSD__)
    return "/var/db/commanderlink/identity.bin";
#elif defined(__sun)
    return "/var/lib/commanderlink/identity.bin";
#else
    return "/var/lib/commanderlink/identity.bin";
#endif
}

static const char* identity_dir(void) {
#if defined(__FreeBSD__)
    return "/var/db/commanderlink";
#else
    return "/var/lib/commanderlink";
#endif
}

/* ---------------- Zeit ---------------- */
static uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

/* ---------------- Hash64 (FNV-1a 64) ---------------- */
static uint64_t fnv1a64_init(void) { return 1469598103934665603ull; }
static uint64_t fnv1a64_update(uint64_t h, const void *p, size_t n) {
    const uint8_t *b = (const uint8_t*)p;
    for (size_t i = 0; i < n; i++) {
        h ^= (uint64_t)b[i];
        h *= 1099511628211ull;
    }
    return h;
}

/* ---------------- CRC32C (Castagnoli) bitwise ----------------
 * Für identity.bin ist Performance sekundär. Korrektheit/Audit primär.
 * poly (reflected) = 0x82F63B78
 */
static uint32_t crc32c_update_bitwise(uint32_t crc, const void *data, size_t len) {
    const uint8_t *p = (const uint8_t*)data;
    const uint32_t poly = 0x82F63B78u;

    crc = ~crc;
    for (size_t i = 0; i < len; i++) {
        crc ^= p[i];
        for (int k = 0; k < 8; k++) {
            uint32_t mask = (uint32_t)-(int32_t)(crc & 1u);
            crc = (crc >> 1) ^ (poly & mask);
        }
    }
    return ~crc;
}

static uint32_t identity_crc32c(const cl_identity_file_rec_v1_t *r) {
    cl_identity_file_rec_v1_t tmp = *r;
    tmp.crc32c = 0;
    return crc32c_update_bitwise(0u, &tmp, sizeof(tmp));
}

/* ---------------- RNG (CSPRNG) ---------------- */
static int rand_bytes(uint8_t *out, size_t n) {
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) return -1;
    size_t off = 0;
    while (off < n) {
        ssize_t r = read(fd, out + off, n - off);
        if (r <= 0) { close(fd); return -2; }
        off += (size_t)r;
    }
    close(fd);
    return 0;
}

/* ---------------- HW Fingerprint (best-effort, deterministisch) ----------------
 * Ziel: stabiler Input. Kein „Sensor“, keine Telemetrie.
 */
static void hw_fingerprint_best_effort(uint8_t out[64], size_t *out_len) {
    memset(out, 0, 64);
    *out_len = 0;

#if defined(__FreeBSD__)
    /* kern.hostuuid, falls vorhanden */
    char uuid[64];
    size_t len = sizeof(uuid);
    if (sysctlbyname("kern.hostuuid", uuid, &len, NULL, 0) == 0 && len > 0 && len <= sizeof(uuid)) {
        memcpy(out, uuid, len);
        *out_len = len;
        return;
    }
#endif

    /* uname als fallback */
    struct utsname u;
    if (uname(&u) == 0) {
        size_t l = 0;

        l = strnlen(u.sysname, sizeof(u.sysname));
        memcpy(out + *out_len, u.sysname, l); *out_len += l;

        l = strnlen(u.nodename, sizeof(u.nodename));
        if (*out_len + l < 64) { memcpy(out + *out_len, u.nodename, l); *out_len += l; }

        l = strnlen(u.machine, sizeof(u.machine));
        if (*out_len + l < 64) { memcpy(out + *out_len, u.machine, l); *out_len += l; }

        return;
    }

    /* worst-case: leer */
    *out_len = 0;
}

/* ---------------- Dir ensure ---------------- */
static int ensure_dir_exists(const char *dir) {
    struct stat st;
    if (stat(dir, &st) == 0) {
        if (S_ISDIR(st.st_mode)) return 0;
        return -1;
    }
    if (mkdir(dir, 0700) != 0) return -2;
    return 0;
}

/* ---------------- Atomic write (temp+fsync+rename) ---------------- */
static int atomic_write_file(const char *path, const void *data, size_t len) {
    char tmp[512];
    snprintf(tmp, sizeof(tmp), "%s.tmp", path);

    int fd = open(tmp, O_CREAT | O_TRUNC | O_WRONLY, 0600);
    if (fd < 0) return -1;

    size_t off = 0;
    while (off < len) {
        ssize_t w = write(fd, (const uint8_t*)data + off, len - off);
        if (w <= 0) { close(fd); return -2; }
        off += (size_t)w;
    }

    if (fsync(fd) != 0) { close(fd); return -3; }
    if (close(fd) != 0) return -4;

    if (rename(tmp, path) != 0) return -5;

    /* dir fsync best-effort */
    int dfd = open(identity_dir(), O_RDONLY);
    if (dfd >= 0) { (void)fsync(dfd); close(dfd); }

    return 0;
}

/* ---------------- Load/Verify ---------------- */
static int load_identity(cl_identity_file_rec_v1_t *out) {
    const char *path = identity_path();
    int fd = open(path, O_RDONLY);
    if (fd < 0) return -1;

    ssize_t r = read(fd, out, sizeof(*out));
    close(fd);
    if (r != (ssize_t)sizeof(*out)) return -2;

    if (out->magic_u32 != CL_IDENTITY_MAGIC_U32) return -3;
    if (out->version_u32 != CL_IDENTITY_VERSION_U32) return -4;
    if (out->reserved0 != 0u) return -5;

    uint32_t want = identity_crc32c(out);
    if (want != out->crc32c) return -6;

    return 0;
}

/* ---------------- Create ---------------- */
static void create_identity(cl_identity_file_rec_v1_t *r) {
    memset(r, 0, sizeof(*r));
    r->magic_u32 = CL_IDENTITY_MAGIC_U32;
    r->version_u32 = CL_IDENTITY_VERSION_U32;
    r->created_time_ns = now_ns();
    r->flags = 0u;
    r->reserved0 = 0u;

    /* node_salt + mesh_tag64 via CSPRNG */
    (void)rand_bytes(r->node_salt, sizeof(r->node_salt));

    uint8_t mt[8];
    (void)rand_bytes(mt, sizeof(mt));
    uint64_t m = 0;
    memcpy(&m, mt, sizeof(m));
    r->mesh_tag64 = m;

    /* node_tag64 = HASH64(hw_fp || salt) */
    uint8_t fp[64];
    size_t fp_len = 0;
    hw_fingerprint_best_effort(fp, &fp_len);

    uint64_t h = fnv1a64_init();
    if (fp_len > 0) h = fnv1a64_update(h, fp, fp_len);
    h = fnv1a64_update(h, r->node_salt, sizeof(r->node_salt));
    r->node_tag64 = h;

    r->crc32c = identity_crc32c(r);
}

/* ---------------- Public API ---------------- */
int cl_identity_load_or_create(struct cl_identity_out *out) {
    if (!out) return -100;
    memset(out, 0, sizeof(*out));

    (void)ensure_dir_exists(identity_dir());

    cl_identity_file_rec_v1_t r;
    int rc = load_identity(&r);
    if (rc == 0) {
        out->node_tag64 = r.node_tag64;
        out->mesh_tag64 = r.mesh_tag64;
        out->created_time_ns = r.created_time_ns;
        out->flags = r.flags;
        out->crc32c_ok = 1u;
        out->file_epoch = 1u; /* loaded */
        return 0;
    }

    /* Datei fehlt -> erzeugen; andere Fehler -> INVALID */
    if (rc == -1) {
        create_identity(&r);
        int wrc = atomic_write_file(identity_path(), &r, sizeof(r));
        if (wrc != 0) return -200;

        out->node_tag64 = r.node_tag64;
        out->mesh_tag64 = r.mesh_tag64;
        out->created_time_ns = r.created_time_ns;
        out->flags = r.flags;
        out->crc32c_ok = 1u;
        out->file_epoch = 2u; /* created */
        return 0;
    }

    out->crc32c_ok = 0u;
    return -300;
}

/* ---------------- Publish in SHM Identity Segment ---------------- */
int cl_identity_publish_to_shm(void *core_base, size_t core_sz,
                               const struct cl_identity_out *in)
{
    if (!core_base || !in) return -1;

    cl_root_t *root = (cl_root_t*)core_base;
    if (root->toc_size < sizeof(cl_toc_header_t)) return -2;
    if ((uint64_t)root->toc_offset + (uint64_t)root->toc_size > (uint64_t)core_sz) return -3;

    cl_toc_t *toc = (cl_toc_t*)((uint8_t*)core_base + (size_t)root->toc_offset);

    cl_identity_seg_256_t *seg = NULL;

    for (uint32_t i = 0; i < toc->header.entry_count && i < CL_TOC_MAX_ENTRIES; i++) {
        cl_toc_entry_t *e = &toc->entries[i];
        if ((uint16_t)e->type == (uint16_t)CL_IDENTITY_SEG_256) {
            /* Core-base offset */
            if ((uint64_t)e->offset_bytes + 256ull > (uint64_t)core_sz) return -4;
            seg = (cl_identity_seg_256_t*)((uint8_t*)core_base + (size_t)e->offset_bytes);
            break;
        }
    }
    if (!seg) return -5;

    /* cld ist Producer; CORE ist Authority und published TOC epoch */
    atomic_fetch_add(&seg->hot.identity_epoch, 1u);

    atomic_store(&seg->hot.identity_state, (uint32_t)CL_IDENTITY_VALID);
    atomic_store(&seg->hot.reason_code, 0u);

    seg->hot.node_tag64 = (cl_node_tag64_t)in->node_tag64;
    seg->hot.mesh_tag64 = (cl_mesh_tag64_t)in->mesh_tag64;


    atomic_store(&seg->hot.flags, in->flags);

    /* src/qual: nutze CL_QUAL_DIRECT. Wenn du Alias CL_QUAL_DIREKT hast, ist das ok. */
    seg->hot.src  = (uint8_t)CL_QUAL_DIRECT;
    seg->hot.qual = (uint8_t)CL_QUAL_DIRECT;

    atomic_store(&seg->warm.file_epoch, in->file_epoch);
    atomic_store(&seg->warm.created_time_ns, in->created_time_ns);
    atomic_store(&seg->warm.crc32c_ok, in->crc32c_ok);
    atomic_store(&seg->warm.last_load_ns, now_ns());

    return 0;
}


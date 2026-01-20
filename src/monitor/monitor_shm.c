#define _POSIX_C_SOURCE 200809L
#include "monitor_shm.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdatomic.h>
#include <errno.h>

/* UI-Fehlertexte (statisch, SSOT-neutral) */
static const char *ERR_NONE        = "OK";
static const char *ERR_CORE_OPEN   = "Core SHM open failed";
static const char *ERR_CORE_STAT   = "Core fstat failed";
static const char *ERR_CORE_MMAP   = "Core mmap failed";
static const char *ERR_TOO_SMALL   = "Core size < 4096B";
static const char *ERR_ROOT_MAGIC  = "Root Magic invalid";
static const char *ERR_TOC_BOUNDS  = "TOC offset outside Core";
static const char *ERR_TOC_SIZE    = "TOC header exceeds Core";
static const char *ERR_TOC_ENTRIES = "TOC entries exceed Core";

/* --------------------------------------------------------- */

void mon_shm_init(mon_shm_t *s) {
    if (!s) return;
    s->fd_core  = -1;
    s->base_core = MAP_FAILED;
    s->size_core = 0;

    s->fd_bulk  = -1;
    s->base_bulk = MAP_FAILED;
    s->size_bulk = 0;

    s->root = NULL;
    s->toc  = NULL;
}

void mon_shm_close(mon_shm_t *s) {
    if (!s) return;

    if (s->base_bulk != MAP_FAILED) {
        munmap(s->base_bulk, s->size_bulk);
        s->base_bulk = MAP_FAILED;
    }
    if (s->fd_bulk != -1) {
        close(s->fd_bulk);
        s->fd_bulk = -1;
    }

    if (s->base_core != MAP_FAILED) {
        munmap(s->base_core, s->size_core);
        s->base_core = MAP_FAILED;
    }
    if (s->fd_core != -1) {
        close(s->fd_core);
        s->fd_core = -1;
    }

    s->size_core = 0;
    s->size_bulk = 0;
    s->root = NULL;
    s->toc  = NULL;
}

void mon_shm_reset(mon_shm_t *s) {
    mon_shm_close(s);
}

/* --------------------------------------------------------- */
/* Helper: map SHM read-only */

static int map_helper(const char *name, int *fd_out, void **base_out, size_t *size_out) {
    int fd = shm_open(name, O_RDONLY, 0);
    if (fd == -1) return -1;

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        close(fd);
        return -2;
    }

    void *base = mmap(NULL, (size_t)sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (base == MAP_FAILED) {
        close(fd);
        return -3;
    }

    *fd_out   = fd;
    *base_out = base;
    *size_out = (size_t)sb.st_size;
    return 0;
}

/* --------------------------------------------------------- */
/* Validation */

static int validate_root(mon_shm_t *s, mon_shm_status_t *st) {
    if (s->size_core < sizeof(cl_root_4096_t)) {
        st->error_text = ERR_TOO_SMALL;
        return 0;
    }

    const cl_root_4096_t *r = (const cl_root_4096_t *)s->base_core;
    if (r->root_magic != CL_ROOT_MAGIC_U32) {
        st->error_text = ERR_ROOT_MAGIC;
        return 0;
    }

    s->root = r;
    return 1;
}

static int validate_toc(mon_shm_t *s, mon_shm_status_t *st) {
    if (!s->root) return 0;

    uint64_t off = s->root->toc_offset;
    if (off >= s->size_core) {
        st->error_text = ERR_TOC_BOUNDS;
        return 0;
    }

    if (off + sizeof(cl_toc_header_t) > s->size_core) {
        st->error_text = ERR_TOC_SIZE;
        return 0;
    }

    const cl_toc_t *t = (const cl_toc_t *)((const uint8_t*)s->base_core + off);
    size_t toc_bytes = sizeof(cl_toc_header_t) +
                       (t->header.entry_count * sizeof(cl_toc_entry_t));

    if (off + toc_bytes > s->size_core) {
        st->error_text = ERR_TOC_ENTRIES;
        return 0;
    }

    s->toc = t;
    return 1;
}

/* --------------------------------------------------------- */
/* MAIN POLL */

mon_shm_status_t mon_shm_poll(mon_shm_t *s) {
    mon_shm_status_t st = {0, 0, 0, 0, 0, ERR_NONE};

    /* 1) Core map */
    if (s->base_core == MAP_FAILED) {
        errno = 0;
        int rc = map_helper(CL_CORE_SHM_NAME, &s->fd_core, &s->base_core, &s->size_core);
        st.last_errno = errno;

        if (rc == -1) { st.error_text = ERR_CORE_OPEN; return st; }
        if (rc == -2) { st.error_text = ERR_CORE_STAT; return st; }
        if (rc == -3) { st.error_text = ERR_CORE_MMAP; return st; }
    }
    st.mapped_core = 1;

    /* 2) Validate */
    if (!validate_root(s, &st)) return st;
    st.root_ok = 1;

    if (!validate_toc(s, &st)) return st;
    st.toc_ok = 1;

    /* 3) Bulk (optional) */
    uint32_t bulk_present = atomic_load(&s->root->bulk_present);
    if (bulk_present) {
        if (s->base_bulk == MAP_FAILED) {
            map_helper(CL_BULK_SHM_NAME, &s->fd_bulk, &s->base_bulk, &s->size_bulk);
        }
        if (s->base_bulk != MAP_FAILED) st.mapped_bulk = 1;
    } else {
        if (s->base_bulk != MAP_FAILED) {
            munmap(s->base_bulk, s->size_bulk);
            close(s->fd_bulk);
            s->base_bulk = MAP_FAILED;
            s->fd_bulk = -1;
            s->size_bulk = 0;
        }
    }

    st.error_text = ERR_NONE;
    return st;
}

/* --------------------------------------------------------- */
/* Segment Access */

const void* mon_shm_get_segment(mon_shm_t *s, uint16_t type, uint32_t idx) {
    if (!s || !s->toc) return NULL;

    for (uint32_t i = 0; i < s->toc->header.entry_count; i++) {
        const cl_toc_entry_t *e = &s->toc->entries[i];
        if (e->type != type) continue;
        if (idx >= e->count) return NULL;

        void   *base  = s->base_core;
        size_t  limit = s->size_core;

        if (e->flags & CL_TOC_FLAG_RECLAIMABLE) {
            if (s->base_bulk == MAP_FAILED) return NULL;
            base  = s->base_bulk;
            limit = s->size_bulk;
        }

        uint64_t off = e->offset_bytes + (idx * (uint64_t)e->stride_bytes);
        if (off + e->stride_bytes > limit) return NULL;

        return (const void *)((const uint8_t*)base + off);
    }
    return NULL;
}


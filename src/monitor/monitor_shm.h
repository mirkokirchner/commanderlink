#ifndef CL_MONITOR_SHM_H
#define CL_MONITOR_SHM_H

#include <stddef.h>
#include <stdint.h>

/* SSOT: Runtime-Wahrheit */
#include <cl_runtime/cl_shm_layout.h>
#include <cl_runtime/cl_toc.h>

/* Safety: SHM-Name MUSS aus Runtime kommen */
#ifndef CL_CORE_SHM_NAME
#error "SSOT VIOLATION: CL_CORE_SHM_NAME missing (include path wrong)"
#endif

#ifndef CL_BULK_SHM_NAME
#error "SSOT VIOLATION: CL_BULK_SHM_NAME missing (include path wrong)"
#endif

typedef struct {
    /* CORE (resident) */
    int     fd_core;
    void   *base_core;
    size_t  size_core;

    /* BULK (reclaimable) */
    int     fd_bulk;
    void   *base_bulk;
    size_t  size_bulk;

    /* Cached pointers (gültig nur wenn root_ok && toc_ok) */
    const cl_root_4096_t *root;
    const cl_toc_t       *toc;
} mon_shm_t;

typedef struct {
    int mapped_core;     /* Core-SHM gemappt */
    int mapped_bulk;     /* Bulk-SHM gemappt (optional) */
    int root_ok;         /* Root Magic + Bounds OK */
    int toc_ok;          /* TOC Header + Bounds OK */
    int last_errno;      /* errno der letzten Fehlstelle */
    const char *error_text;
} mon_shm_status_t;

/* Lifecycle */
void mon_shm_init(mon_shm_t *s);
void mon_shm_close(mon_shm_t *s);
void mon_shm_reset(mon_shm_t *s);

/* Poll = einzige SSOT-Funktion im UI-Loop */
mon_shm_status_t mon_shm_poll(mon_shm_t *s);

/* Segment-Zugriff (Core oder Bulk je nach TOC-Flag) */
const void* mon_shm_get_segment(mon_shm_t *s, uint16_t type, uint32_t idx);

#endif /* CL_MONITOR_SHM_H */


#pragma once
#define _POSIX_C_SOURCE 200809L
/* ============================================================================
 * CommanderLink – Identity Gate (cld-only) – API – FINAL
 * Datei: src/cld/cl_identity.h
 * ============================================================================
 *
 * SSOT:
 *   - Identity liefert ausschließlich cld.
 *   - identity.bin ist fixed-size und CRC32C-geschützt.
 *   - cld published Identity in CL_IDENTITY_SEG_256.
 *   - TOC entry epoch bleibt 0 bis CORE published (Cut-Point).
 * ============================================================================
 */

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct cl_identity_out {
    uint64_t node_tag64;
    uint64_t mesh_tag64;
    uint64_t created_time_ns;
    uint64_t file_epoch;     /* 1=geladen, 2=neu erzeugt */
    uint32_t flags;
    uint32_t crc32c_ok;      /* 1/0 */
};

/* Lädt identity.bin oder erzeugt sie (First Boot) deterministisch.
 * Return:
 *   0  = OK (out gefüllt)
 *  <0  = Fehler (Datei corrupt/invalid → P0 stop)
 */
int cl_identity_load_or_create(struct cl_identity_out *out);

/* Published Identity in das SHM Identity-Segment (resident 256B).
 * Erwartet: Core-SHM ist RW gemappt, TOC enthält CL_IDENTITY_SEG_256.
 * Return:
 *   0  = OK
 *  <0  = Fehler (kein Segment, bounds fail, etc.)
 */
int cl_identity_publish_to_shm(void *core_base, size_t core_sz,
                               const struct cl_identity_out *in);

#ifdef __cplusplus
}
#endif


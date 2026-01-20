#pragma once
/* ============================================================================
 * CommanderLink – identity.bin (Persistenz-Record) – FINAL SSOT
 * Datei: include/cl_manifest/cl_identity_file.h
 * ============================================================================
 *
 * SSOT:
 *   - identity.bin liefert ausschließlich cld.
 *   - fixed-size, CRC32C-geschützt.
 *   - reserved bytes müssen 0 sein.
 *
 * Pfade:
 *   - Linux/illumos: /var/lib/commanderlink/identity.bin
 *   - FreeBSD/GhostBSD: /var/db/commanderlink/identity.bin
 *
 * Inhalt:
 *   magic(u32), version(u32), created_time_ns(u64)
 *   node_salt[32], node_tag64(u64), mesh_tag64(u64)
 *   flags(u32), reserved(u32), crc32c(u32)
 * ============================================================================
 */

#include <stdint.h>

#define CL_IDENTITY_MAGIC_U32   0x49444E54u /* "IDNT" */
#define CL_IDENTITY_VERSION_U32 1u

/* Record size ist bewusst klein und fixed. */
typedef struct __attribute__((packed)) cl_identity_file_rec_v1 {
    uint32_t magic_u32;
    uint32_t version_u32;

    uint64_t created_time_ns;

    uint8_t  node_salt[32];  /* CSPRNG */
    uint64_t node_tag64;     /* HASH64(hw_fingerprint || node_salt) */
    uint64_t mesh_tag64;     /* CSPRNG64 oder später Domain-Join */

    uint32_t flags;
    uint32_t reserved0;      /* muss 0 sein */

    uint32_t crc32c;         /* CRC32C über alle Bytes außer crc32c (crc32c=0 beim rechnen) */
} cl_identity_file_rec_v1_t;

_Static_assert(sizeof(cl_identity_file_rec_v1_t) == 4+4+8+32+8+8+4+4+4,
               "identity record size drift");


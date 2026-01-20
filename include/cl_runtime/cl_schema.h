#pragma once
/* ============================================================================
 * CommanderLink – Schema/ABI Contract (Runtime) – FINAL
 * Datei: include/cl_runtime/cl_schema.h
 * ============================================================================
 *
 * Zweck:
 *   - Einmalige, zentrale Definition der Runtime-Schema-Version.
 *   - Endianness Magic (Host sanity).
 *   - Page-Größe/Alignment (Root-Page).
 *
 * Maschinenregel:
 *   - SHM ist Host-Order.
 *   - endian_magic muss beim Attach geprüft werden.
 *   - Schema bump nur bei semantisch relevanten Layout-Änderungen.
 * ============================================================================
 */

#include <stdint.h>

#define CL_SCHEMA_VERSION_U32  0x00000111u
#define CL_ENDIAN_MAGIC_U32    0x12345678u
#define CL_SHM_PAGE_BYTES      4096u

#define CL_ABI_CHECKSUM_NONE   0ull



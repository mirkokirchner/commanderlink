#pragma once
/* ============================================================================
 * CommanderLink – Endianness Helpers (Common) – FINAL
 * Datei: include/cl_common/cl_endian.h
 * ============================================================================
 *
 * Maschinenregeln:
 *   - SHM ist Host-Order.
 *   - Wire/OOB ist explizit little-endian (_le) und wird an der Grenze konvertiert.
 * ============================================================================
 */

#include <stdint.h>

#if defined(__clang__) || defined(__GNUC__)
  #define CL_BSWAP16(x) __builtin_bswap16((uint16_t)(x))
  #define CL_BSWAP32(x) __builtin_bswap32((uint32_t)(x))
  #define CL_BSWAP64(x) __builtin_bswap64((uint64_t)(x))
#else
  static inline uint16_t CL_BSWAP16(uint16_t x) { return (uint16_t)((x<<8) | (x>>8)); }
  static inline uint32_t CL_BSWAP32(uint32_t x) {
      return ((x & 0x000000FFu) << 24) |
             ((x & 0x0000FF00u) <<  8) |
             ((x & 0x00FF0000u) >>  8) |
             ((x & 0xFF000000u) >> 24);
  }
  static inline uint64_t CL_BSWAP64(uint64_t x) {
      return ((x & 0x00000000000000FFull) << 56) |
             ((x & 0x000000000000FF00ull) << 40) |
             ((x & 0x0000000000FF0000ull) << 24) |
             ((x & 0x00000000FF000000ull) <<  8) |
             ((x & 0x000000FF00000000ull) >>  8) |
             ((x & 0x0000FF0000000000ull) >> 24) |
             ((x & 0x00FF000000000000ull) >> 40) |
             ((x & 0xFF00000000000000ull) >> 56);
  }
#endif

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  #define CL_HOST_IS_LE 1
#else
  #define CL_HOST_IS_LE 0
#endif

static inline uint16_t cl_host_to_le16(uint16_t v) { return CL_HOST_IS_LE ? v : CL_BSWAP16(v); }
static inline uint32_t cl_host_to_le32(uint32_t v) { return CL_HOST_IS_LE ? v : CL_BSWAP32(v); }
static inline uint64_t cl_host_to_le64(uint64_t v) { return CL_HOST_IS_LE ? v : CL_BSWAP64(v); }

static inline uint16_t cl_le16_to_host(uint16_t v) { return CL_HOST_IS_LE ? v : CL_BSWAP16(v); }
static inline uint32_t cl_le32_to_host(uint32_t v) { return CL_HOST_IS_LE ? v : CL_BSWAP32(v); }
static inline uint64_t cl_le64_to_host(uint64_t v) { return CL_HOST_IS_LE ? v : CL_BSWAP64(v); }


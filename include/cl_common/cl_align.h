#pragma once
/* ============================================================================
 * CommanderLink – Alignment/ABI Grundbausteine (Common) – FINAL
 * Datei: include/cl_common/cl_align.h
 * ============================================================================
 *
 * Zweck:
 *   Einheitliche OS-freie Makros für Alignment und Compile-Time-Gates.
 *
 * Maschinenregel:
 *   Alignment ist Geometrie (Cacheline/Page/Stride), kein Compiler-Zufall.
 * ============================================================================
 */

#include <stddef.h>
#include <stdint.h>

#if defined(__GNUC__) || defined(__clang__)
  #define CL_ALIGNAS(N) __attribute__((aligned(N)))
#else
  #define CL_ALIGNAS(N) _Alignas(N)
#endif

#define CL_STATIC_ASSERT(expr, msg) _Static_assert((expr), msg)


#pragma once
#include <stdint.h>

/* Compile-time ABI fingerprint for the current build.
 * SSOT: Must match root->abi_layout_checksum written by clinit.
 */
uint64_t cl_abi_fingerprint_u64(void);


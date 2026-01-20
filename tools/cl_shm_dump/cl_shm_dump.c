/*
 * ============================================================================
 * CommanderLink – SHM Dump Tool (read-only) – FINAL Endausbau
 * Datei: tools/cl_shm_dump/cl_shm_dump.c
 * ============================================================================
 *
 * Zweck:
 *   - Read-only Dump von Core-SHM Root + TOC + Segment-Geometrie.
 *   - Keine Policy, keine Interpretation, keine Heuristik.
 *   - Zeigt die physikalische Wahrheit: Offsets/Strides/Counts/Flags/Gates.
 *
 * Plattform:
 *   - Linux / BSD / OmniOS (POSIX shm_open + mmap)
 * ============================================================================
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdatomic.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "../../include/cl_runtime/cl_runtime.h"

/* --------------------------------------------------------------------------
 * Helpers: POSIX SHM Mapping
 * -------------------------------------------------------------------------- */

static int map_shm_ro(const char *name, void **out_base, size_t *out_sz) {
    int fd = shm_open(name, O_RDONLY, 0);
    if (fd < 0) return -1;

    struct stat st;
    if (fstat(fd, &st) != 0) {
        close(fd);
        return -2;
    }
    if (st.st_size <= 0) {
        close(fd);
        return -3;
    }

    void *p = mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);
    if (p == MAP_FAILED) return -4;

    *out_base = p;
    *out_sz = (size_t)st.st_size;
    return 0;
}

static void unmap_shm(void *base, size_t sz) {
    if (base && sz) (void)munmap(base, sz);
}

/* --------------------------------------------------------------------------
 * Helpers: Segment-Type Name
 *
 * Hinweis:
 *   Die Typ-IDs sind manifest-nahe. Die Größe (Stride) steht im TOC.
 * -------------------------------------------------------------------------- */
static const char* seg_type_name(uint32_t t) {
    switch ((cl_seg_type_t)t) {
        case CL_SEG_CPU:           return "CPU";
        case CL_SEG_MEMORY:        return "MEMORY";
        case CL_SEG_PCIe:          return "PCIe";
        case CL_SEG_BOARD:         return "BOARD";
        case CL_SEG_TIMEBASE:      return "TIMEBASE";
        case CL_SEG_BUDGET:        return "BUDGET";
        case CL_SEG_WATCHDOG:      return "WATCHDOG";
        case CL_SEG_NIC:           return "NIC";
        case CL_SEG_LINK_SUMMARY:  return "LINK_SUMMARY";
        case CL_SEG_DMA:           return "DMA";
        case CL_SEG_OVERLAY:       return "OVERLAY";
        case CL_SEG_MESH_NEIGHBOR: return "MESH_NEIGHBOR";
        case CL_SEG_MESH_PEER:     return "MESH_PEER";
        case CL_SEG_ZFS_SUMMARY:   return "ZFS_SUMMARY";
        case CL_SEG_ZFS_POOL:      return "ZFS_POOL";
        case CL_SEG_ZFS_VDEV:      return "ZFS_VDEV";
        case CL_SEG_ZFS_DATASET:   return "ZFS_DATASET";
        case CL_SEG_ZFS_ZVOL:      return "ZFS_ZVOL";
        case CL_SEG_HISTORY:       return "HISTORY";
        case CL_SEG_FORENSICS:     return "FORENSICS";
        default:                   return "UNKNOWN";
    }
}

/* --------------------------------------------------------------------------
 * Helpers: Flags pretty-print (keine Interpretation, nur Anzeige)
 * -------------------------------------------------------------------------- */
static void print_flags_u32(const char *label, uint32_t v) {
    printf("    %-14s 0x%08x\n", label, v);
}

static void print_u16_flags(const char *label, uint16_t v) {
    printf("    %-14s 0x%04x\n", label, (unsigned)v);
}

/* --------------------------------------------------------------------------
 * Dump Root
 * -------------------------------------------------------------------------- */
static int dump_root(const cl_root_t *r, size_t core_sz) {
    printf("ROOT (Core-SHM):\n");
    printf("  root_magic:      0x%08x\n", r->root_magic);
    printf("  schema_version:  0x%08x\n", r->schema_version);
    printf("  endian_magic:    0x%08x\n", r->endian_magic);
    printf("  root_flags:      0x%08x\n", r->root_flags);
    printf("  abi_checksum:    0x%016" PRIx64 "\n", (uint64_t)r->abi_layout_checksum);

    printf("  seq_cnt:         %" PRIu64 "\n", (uint64_t)atomic_load(&r->seq_cnt));
    printf("  uptime_ns:       %" PRIu64 "\n", (uint64_t)atomic_load(&r->uptime_ns));
    printf("  bulk_epoch:      %" PRIu64 "\n", (uint64_t)atomic_load(&r->bulk_epoch));
    printf("  bulk_present:    %u\n", (unsigned)atomic_load(&r->bulk_present));

    printf("  core_total:      %" PRIu64 "\n", (uint64_t)r->core_total_bytes);
    printf("  bulk_total:      %" PRIu64 "\n", (uint64_t)r->bulk_total_bytes);

    printf("  toc_offset:      %" PRIu64 "\n", (uint64_t)r->toc_offset);
    printf("  toc_size:        %" PRIu64 "\n", (uint64_t)r->toc_size);

    printf("  gates_possible:  0x%016" PRIx64 "\n", (uint64_t)atomic_load(&r->gates_possible));
    printf("  gates_effective: 0x%016" PRIx64 "\n", (uint64_t)atomic_load(&r->gates_effective));

    if (r->root_magic != CL_ROOT_MAGIC_U32) {
        printf("  FAIL: root_magic mismatch (expected 0x%08x)\n", (unsigned)CL_ROOT_MAGIC_U32);
        return 1;
    }
    if (r->endian_magic != CL_ENDIAN_MAGIC_U32) {
        printf("  FAIL: endian_magic mismatch (expected 0x%08x)\n", (unsigned)CL_ENDIAN_MAGIC_U32);
        return 2;
    }
    if (r->schema_version != CL_SCHEMA_VERSION_U32) {
        printf("  FAIL: schema_version mismatch (expected 0x%08x)\n", (unsigned)CL_SCHEMA_VERSION_U32);
        return 3;
    }
    if (r->toc_offset + r->toc_size > (uint64_t)core_sz) {
        printf("  FAIL: toc_offset/toc_size out of core mapping\n");
        return 4;
    }
    if (r->toc_size < sizeof(cl_toc_header_t)) {
        printf("  FAIL: toc_size too small\n");
        return 5;
    }
    return 0;
}

/* --------------------------------------------------------------------------
 * Dump TOC
 * -------------------------------------------------------------------------- */
static void dump_toc(const cl_toc_t *toc, uint64_t toc_size_bytes) {
    printf("\nTOC:\n");
    printf("  version:      %u\n", (unsigned)toc->header.version);
    printf("  entry_count:  %u\n", (unsigned)toc->header.entry_count);
    printf("  total_bytes:  %" PRIu64 "\n", (uint64_t)toc->header.total_bytes);
    printf("  build_id:     0x%016" PRIx64 "\n", (uint64_t)toc->header.build_id);
    printf("  epoch:        %" PRIu64 "\n", (uint64_t)toc->header.epoch);

    uint64_t need = (uint64_t)sizeof(cl_toc_header_t) +
                    (uint64_t)toc->header.entry_count * (uint64_t)sizeof(cl_toc_entry_t);

    printf("  toc_size:     %" PRIu64 "\n", toc_size_bytes);
    printf("  need_bytes:   %" PRIu64 "\n", need);

    if (toc->header.entry_count > CL_TOC_MAX_ENTRIES) {
        printf("  WARN: entry_count > CL_TOC_MAX_ENTRIES (%u)\n", (unsigned)CL_TOC_MAX_ENTRIES);
    }
    if (need > toc_size_bytes) {
        printf("  FAIL: TOC size too small for header+entries\n");
        return;
    }

    for (uint32_t i = 0; i < toc->header.entry_count && i < CL_TOC_MAX_ENTRIES; i++) {
        const cl_toc_entry_t *e = &toc->entries[i];

        const uint32_t type   = (uint32_t)e->type;
        const uint32_t flags  = (uint32_t)e->flags;
        const uint32_t stride = (uint32_t)e->stride_bytes;
        const uint32_t count  = (uint32_t)e->count;
        const uint64_t off    = (uint64_t)e->offset_bytes;

        printf("\n  SEG[%u]: %s (type=0x%04x)\n", i, seg_type_name(type), type);
        printf("    offset_bytes   %" PRIu64 "\n", off);
        printf("    stride_bytes   %u\n", stride);
        printf("    count          %u\n", count);

        print_flags_u32("flags", flags);
        print_u16_flags("source",  (uint16_t)e->source_flags);
        print_u16_flags("access",  (uint16_t)e->access_flags);
        print_u16_flags("presence",(uint16_t)e->presence_flags);
        print_u16_flags("layout",  (uint16_t)e->layout_flags);

        printf("    gates_possible 0x%016" PRIx64 "\n", (uint64_t)e->gates_possible);
        printf("    gates_effective 0x%016" PRIx64 "\n", (uint64_t)e->gates_effective);
        printf("    epoch          %" PRIu64 "\n", (uint64_t)e->epoch);
    }
}

int main(void) {
    void *core_base = NULL;
    size_t core_sz = 0;

    int rc = map_shm_ro(CL_CORE_SHM_NAME, &core_base, &core_sz);
    if (rc != 0) {
        printf("FAIL: cannot map CORE SHM (%s): rc=%d errno=%d (%s)\n",
               CL_CORE_SHM_NAME, rc, errno, strerror(errno));
        return 2;
    }

    const cl_root_t *root = (const cl_root_t*)core_base;
    int root_rc = dump_root(root, core_sz);
    if (root_rc != 0) {
        unmap_shm(core_base, core_sz);
        return 3;
    }

    const cl_toc_t *toc = (const cl_toc_t*)((const uint8_t*)core_base + (size_t)root->toc_offset);
    dump_toc(toc, (uint64_t)root->toc_size);

    unmap_shm(core_base, core_sz);
    return 0;
}


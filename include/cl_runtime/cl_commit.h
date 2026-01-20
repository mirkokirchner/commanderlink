#pragma once
/* ============================================================================
 * CommanderLink – Commit helpers (SSOT)
 * Datei: include/cl_runtime/cl_commit.h
 * ============================================================================
 * Zweck:
 *   Zentralisierte Commit-Semantik für SHM-Publisher (Producer) und Reader
 *   (Consumer). In Stage-1 ist der TOC-entry epoch (u64) der normative
 *   Commit-Marker pro Segment-Entry.
 *
 * Contract (verbindlich):
 *   Producer:
 *     1) payload schreiben (normale stores)
 *     2) epoch publish per Release-Store
 *   Consumer:
 *     1) epoch per Acquire-Load lesen
 *     2) payload lesen
 *
 * Hinweis:
 *   Für Stage-1 vermeiden wir zusätzliche per-Segment seq/CRC Felder, um ABI
 *   nicht unnötig zu ändern. Sequenzierung erfolgt über epoch.
 * ============================================================================
 */

#include <stdint.h>
#include <stdatomic.h>

#include "cl_toc.h"

static inline uint64_t cl_commit_epoch_load_acquire(const cl_toc_entry_t *e) {
    return atomic_load_explicit((const _Atomic(uint64_t) *)&e->epoch, memory_order_acquire);
}

static inline void cl_commit_epoch_store_release(cl_toc_entry_t *e, uint64_t epoch) {
    atomic_store_explicit((_Atomic(uint64_t) *)&e->epoch, epoch, memory_order_release);
}

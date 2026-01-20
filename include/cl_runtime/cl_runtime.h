#pragma once
/* ============================================================================
 * CommanderLink – Runtime Frontdoor
 * Datei: include/cl_runtime/cl_runtime.h
 * ============================================================================
 *
 * Zweck:
 *   Diese Datei ist die EINZIGE offizielle Eintrittsstelle in die
 *   CommanderLink Runtime-Wahrheit.
 *
 *   Wer dieses Header includiert, erklärt:
 *     - ich lese Laufzeit-Zustand (SHM)
 *     - ich erwarte keine Policy
 *     - ich erwarte keine Logik
 *
 *   Die Runtime beschreibt:
 *     - WAS zur Laufzeit existiert
 *     - WIE es physikalisch im Speicher liegt
 *     - WIE es beobachtet werden darf
 *
 *   Die Runtime beschreibt NICHT:
 *     - WANN etwas geschieht
 *     - WARUM etwas entschieden wird
 *     - WIE Initialisierung erfolgt
 *
 *   Architektur:
 *     - x86_64 first
 *     - Host-Endianness
 *     - Cacheline-exakt
 *     - ABI-stabil
 *
 *   Plattformen:
 *     - Linux
 *     - FreeBSD / GhostBSD
 *     - OmniOS / illumos
 *
 *   KEINE Policy. KEINE Heuristik. KEINE Logik.
 * ============================================================================
 */

#include <stdint.h>
#include <stdatomic.h>

/* --------------------------------------------------------------------------
 * 1) Fundamentale Runtime-Typen (ohne Abhängigkeiten)
 * -------------------------------------------------------------------------- */

/* Gates = physikalische Schranken */
#include "cl_gates.h"

/* Beobachtungsqualität / Quelle */
#include "cl_quality.h"

/* --------------------------------------------------------------------------
 * 2) Manifest-nahe Definitionen (symbolische Wahrheit)
 * -------------------------------------------------------------------------- */

/* Enums & IDs (reine Zustandskodierung) */
#include "../cl_manifest/cl_enums.h"
#include "../cl_manifest/cl_ids.h"

/* --------------------------------------------------------------------------
 * 3) Runtime-Ordnung & Verkabelung
 * -------------------------------------------------------------------------- */

/* Table of Contents (physikalische SHM-Struktur) */
#include "cl_toc.h"

/* SHM-Layout & Speicherklassen */
#include "cl_shm_layout.h"

/* Runtime-Schema (Pflicht / Optional / Feature-gebunden) */
#include "cl_schema.h"
#include "cl_identity_seg_256.h"

/* BOOTEN VON CL */
#include "cl_service_seg_256.h"

/* --------------------------------------------------------------------------
 * 4) Runtime-Segmente (SHM-Wahrheit)
 * -------------------------------------------------------------------------- */

/* Kernsegmente - INVENTAR */
#include "cl_seg_cpu_1024.h"
#include "cl_seg_budget_256.h"
#include "cl_seg_mem_256.h"
#include "cl_seg_time_256.h"
#include "cl_seg_board_512.h"

/* Netzwerk / Transfer */
#include "cl_seg_nic_512.h"
#include "cl_seg_link_256.h"
#include "cl_seg_overlay_256.h"
#include "cl_seg_mesh_neighbor_256.h"
#include "cl_seg_mesh_peer_256.h"
#include "cl_seg_dma_4096.h"
#include "cl_seg_flow_cmd_256.h"

/* System / Stabilität */
#include "cl_seg_watchdog_256.h"
#include "cl_seg_pcie_256.h"

/* Storage */
#include "cl_seg_zfs_4096.h"

/* Analyse / Langzeit */
#include "cl_seg_oracle_256.h"
#include "cl_seg_history_4096.h"
#include "cl_seg_forensics_4096.h"

/* Datenpaket */


/* --------------------------------------------------------------------------
 * 5) Runtime-Meta (rein beschreibend)
 * -------------------------------------------------------------------------- */

/*
 * cl_runtime_caps_t beschreibt, welche Teile der Runtime
 * in einer konkreten Instanz existieren KÖNNEN.
 *
 * Dies ist KEINE Initialisierung, KEINE Policy, KEINE Logik.
 * Es ist eine deklarative Zusammenfassung für:
 *   - Monitor
 *   - Tools
 *   - Minimal-Boot
 */
typedef struct cl_runtime_caps {

    /* Grundlegende Verfügbarkeit */
    uint8_t has_cpu;
    uint8_t has_memory;
    uint8_t has_budget;

    /* Netzwerk */
    uint8_t has_nic;
    uint8_t has_mesh;
    uint8_t has_dma;

    /* Storage */
    uint8_t has_zfs;

    /* Analyse */
    uint8_t has_oracle;
    uint8_t has_history;
    uint8_t has_forensics;

} cl_runtime_caps_t;

/*
 * Hinweis:
 *   cl_runtime_caps_t wird NICHT von der Runtime gesetzt.
 *   Es ist eine deklarative Beschreibung dessen,
 *   was prinzipiell Teil der Runtime sein kann.
 */



#pragma once
/* ============================================================================
 * CommanderLink – Globale Gates (Systemzustände & physikalische Schranken)
 *
 * Datei:        include/cl_runtime/cl_gates.h
 *
 * Beschreibung:
 *   Dieses Header definiert die globalen Gates von CommanderLink.
 *
 *   Gates sind KEINE Policy.
 *   Gates sind KEINE Heuristik.
 *   Gates sind KEINE Entscheidung.
 *
 *   Gates sind die materialisierte physikalische Realität des Systems:
 *     - thermische Grenzen
 *     - elektrische Grenzen (VRM/Power)
 *     - Budget- und Interaktivitätszwänge
 *     - Sicherheits- und Trust-Zustände
 *
 *   Alle Runtime-Segmente referenzieren diese Gates.
 *   Kein Segment darf eigene Gate-Semantik erfinden.
 *
 *   Architektur:
 *     - x86_64, little-endian Host
 *     - 64-bit Gate-Masken
 *     - atomar lesbar/schreibbar
 *
 *   WICHTIG:
 *     Dieses Header enthält bewusst KEINE Logik.
 *     Auswertung, Reaktion und Policy liegen außerhalb.
 * ========================================================================== */

#ifndef CL_GATES_H
#define CL_GATES_H

#include <stdint.h>

/* --------------------------------------------------------------------------
 * Architekturannahmen (explizit)
 * -------------------------------------------------------------------------- */
#if !defined(__x86_64__) && !defined(_M_X64)
#error "CommanderLink ist x86_64-first. cl_gates.h setzt x86_64 voraus."
#endif

/* --------------------------------------------------------------------------
 * Gate-Typen: Grundsätzliches Modell
 *
 * - Gates werden als Bitmasken in uint64_t geführt.
 * - Mehrere Gates können gleichzeitig aktiv sein.
 * - Gates sind rein deskriptiv.
 *
 * Begriffe:
 *   possible  : physikalisch / logisch möglich
 *   effective : aktuell wirksam (nach allen Überlagerungen)
 *
 * Beispiel:
 *   AVX-512 possible, aber wegen Thermal-Gate nicht effective.
 * -------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------
 * Gate-Klassen (logische Gruppierung, KEINE Bitwerte)
 * -------------------------------------------------------------------------- */
typedef enum cl_gate_class {
    CL_GATE_CLASS_THERMAL = 0,   /* Temperatur / thermische Limits        */
    CL_GATE_CLASS_POWER,        /* Strom, VRM, Package Power             */
    CL_GATE_CLASS_BUDGET,       /* Zeit-/CPU-/IO-Budget                  */
    CL_GATE_CLASS_INTERACTIVE,  /* Nutzer-Interaktivität                 */
    CL_GATE_CLASS_TRUST,        /* Join / Trust / Security               */
    CL_GATE_CLASS_PLATFORM,     /* Plattform- / Umgebungsrestriktionen   */
    CL_GATE_CLASS_INTERNAL      /* interne Schutzmechanismen             */
} cl_gate_class_t;

/* --------------------------------------------------------------------------
 * Gate-Bits (physikalische Zustände)
 *
 * Bitlayout:
 *   - Niedrige Bits: harte physikalische Schranken
 *   - Mittlere Bits: weiche / adaptive Schranken
 *   - Hohe Bits: logische / sicherheitsrelevante Zustände
 * -------------------------------------------------------------------------- */
typedef uint64_t cl_gate_mask_t;

/* --- Thermische Gates ----------------------------------------------------- */
#define CL_GATE_THERMAL_SOFT        (1ULL << 0)   /* Temperatur hoch, Drossel empfohlen */
#define CL_GATE_THERMAL_HARD        (1ULL << 1)   /* Temperatur kritisch, Drossel Pflicht */

/* --- Power / VRM Gates ---------------------------------------------------- */
#define CL_GATE_POWER_SOFT          (1ULL << 2)   /* Power Budget angespannt             */
#define CL_GATE_POWER_HARD          (1ULL << 3)   /* Power Budget überschritten          */
#define CL_GATE_VRM_LIMIT           (1ULL << 4)   /* VRM meldet Limit / Instabilität     */

/* --- Budget Gates --------------------------------------------------------- */
#define CL_GATE_BUDGET_EXCEEDED     (1ULL << 5)   /* Laufzeit-Budget überschritten       */
#define CL_GATE_BUDGET_RECLAIM      (1ULL << 6)   /* Reclaim aktiv (Bulk freigegeben)    */

/* --- Interaktivität ------------------------------------------------------- */
#define CL_GATE_INTERACTIVE_ACTIVE  (1ULL << 7)   /* Nutzer-/Systeminteraktivität aktiv  */

/* --- Trust / Security ----------------------------------------------------- */
#define CL_GATE_TRUST_NONE          (1ULL << 16)  /* kein Trust (Default-Deny)           */
#define CL_GATE_TRUST_ESTABLISHED   (1ULL << 17)  /* Trust ausgehandelt                  */
#define CL_GATE_TRUST_ACTIVE        (1ULL << 18)  /* Trust aktiv, Data Plane erlaubt     */

#define CL_GATE_SECURITY_DEGRADED   (1ULL << 19)  /* Security nicht voll gewährleistet   */

/* --- Plattform / Umgebung ------------------------------------------------ */
#define CL_GATE_PLATFORM_VM         (1ULL << 24)  /* Ausführung in VM                    */
#define CL_GATE_PLATFORM_LIMITED    (1ULL << 25)  /* reduzierte Telemetrie / Caps        */

/* --- Interne Schutzmechanismen ------------------------------------------- */
#define CL_GATE_WATCHDOG_TRIGGERED  (1ULL << 32)  /* Watchdog hat ausgelöst              */
#define CL_GATE_INTERNAL_ERROR      (1ULL << 33)  /* interner Inkonsistenzschutz         */

/* --------------------------------------------------------------------------
 * Gate-Masken (Zusammenfassungen)
 * -------------------------------------------------------------------------- */

/* Alle Gates, die harte Drosselung erzwingen */
#define CL_GATE_MASK_HARD_LIMITS ( \
    CL_GATE_THERMAL_HARD      |   \
    CL_GATE_POWER_HARD        |   \
    CL_GATE_VRM_LIMIT         |   \
    CL_GATE_WATCHDOG_TRIGGERED )

/* Gates, die Interaktivität schützen */
#define CL_GATE_MASK_INTERACTIVE ( \
    CL_GATE_INTERACTIVE_ACTIVE |  \
    CL_GATE_BUDGET_RECLAIM )

/* Gates, die Data Plane grundsätzlich verbieten */
#define CL_GATE_MASK_NO_DATAPLANE ( \
    CL_GATE_TRUST_NONE        |    \
    CL_GATE_SECURITY_DEGRADED |    \
    CL_GATE_WATCHDOG_TRIGGERED )

/* --------------------------------------------------------------------------
 * Gate-Zustandscontainer
 *
 * Wird in Segmenten verwendet, um possible/effective sauber zu trennen.
 * Größe: 16 Byte (2 × u64), cacheline-neutral.
 * -------------------------------------------------------------------------- */
typedef struct cl_gate_state {
    cl_gate_mask_t possible;   /* was physikalisch / logisch möglich ist */
    cl_gate_mask_t effective;  /* was aktuell wirksam ist                */
} cl_gate_state_t;

_Static_assert(sizeof(cl_gate_state_t) == 16,
               "cl_gate_state_t muss exakt 16 Byte groß sein");

/* --------------------------------------------------------------------------
 * Semantik-Hinweise (KEIN Code!)
 *
 * - possible darf nur erweitert werden, niemals implizit eingeschränkt.
 * - effective ist das Ergebnis aller aktuell aktiven Gates.
 * - Kein Modul darf Gates „wegdiskutieren“.
 * - Monitor zeigt possible vs effective explizit getrennt an.
 * -------------------------------------------------------------------------- */

#endif /* CL_GATES_H */


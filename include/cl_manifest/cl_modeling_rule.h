#pragma once
/* ============================================================================
 * CommanderLink – Modeling Rule / Verfassung (Manifest) – FINAL
 * Datei: include/cl_manifest/cl_modeling_rule.h
 * ============================================================================
 *
 * Globale Projektregel (bindend):
 *   - „Das Maximum ist das Minimum.“
 *   - Abwesenheit ist ein Zustand, keine Lücke.
 *   - Alles, was lieferbar ist, wird erfasst; alles andere wird als Zustand
 *     UNSUPPORTED/UNKNOWN/DISABLED modelliert – inklusive Source/Quality/Reason.
 *
 * Konsequenzen:
 *   - SHM modelliert das Maximum (possible/effective/state/source/quality).
 *   - Policies/Gates werten alle verfügbaren Daten aus.
 *   - Monitor zeigt das Maximalmodell lückenlos, markiert Einschränkungen explizit.
 *
 * Sicherheitsregel:
 *   - Join/Trust paranoid (Default-deny). Ohne Trust: keine Routes/Grants/Data Plane.
 *
 * Performance-Regel:
 *   - Standard ist maximale Performance unter offenen Gates (Budget/Thermal/VRM/Security).
 *   - Reduktion ist sofort und lokal, nie global vorsorglich.
 * ============================================================================
 */

#include <stdint.h>

#define CL_MODEL_MAX_IS_MIN 1
#define CL_MODEL_STATE_EXPLICIT 1
#define CL_MODEL_APPEND_ONLY 1
#define CL_MODEL_SHM_HOST_ORDER 1
#define CL_MODEL_SHM_NO_PACKED 1

#define CL_MODEL_ZERO_TRANSFORM_STANDARD 1
#define CL_MODEL_ATOM_BYTES 64u


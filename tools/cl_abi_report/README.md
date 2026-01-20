# cl_abi_report

ABI-/Layout-Prüfgerät für CommanderLink.

## Zweck
- Prüft Root/TOC/Segment-Geometrie gegen reale SHM-Mappings.
- Meldet ABI-Drift deterministisch.

## Ablauf
1) `cl_dummy_shm` starten (erzeugt /cl_core_root)
2) `cl_abi_report` starten (liest /cl_core_root, prüft TOC/Segmente)

## Ausgabe
- `ABI_STABLE` oder `ABI_UNSTABLE`


1) RT FAIL Fehlercode-Tabelle (v1.0)

Diese Codes sind jetzt eindeutig und sollen stabil bleiben:

10 Root magic falsch

11 Endian magic falsch

12 Schema version falsch

13 ABI checksum mismatch + FAILFAST aktiv

14 toc_offset/toc_size out of bounds

15 toc_size kleiner als TOC header

16 entry_count > CL_TOC_MAX_ENTRIES

17 TOC entries passen nicht in toc_size

18 stride nicht erlaubt (nicht 256/512/1024/4096)

19 offset nicht stride-aligned

20 Core-Segment bounds verletzt

21 Bulk-Segment bounds verletzt (Bulk mapped)

22 toc->header.total_bytes ≠ tatsächlich gemappter Bereich

23 Bulk absent, aber reclaimable Entry nicht RECLAIMED

24 Bulk present, aber reclaimable Entry nicht PRESENT (oder PARTIAL)

25 Bulk mapped, aber root.bulk_present==0

26 Bulk nicht mapped, aber root.bulk_present!=0

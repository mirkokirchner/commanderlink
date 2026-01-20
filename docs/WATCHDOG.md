# CommanderLink Watchdog (FINAL)
Datei: docs/WATCHDOG.md

## Zweck
Der Watchdog stellt sicher, dass CommanderLink sich selbst heilen kann:
- Bei Stall oder Panic wird CL neu gestartet.
- Das OS bleibt stabil (Fail-open).

## Prinzipien
- Fail-open für OS: cl0 ist non-persistent; routes sind /32; keine Default-Route Änderungen.
- Fail-closed für CL Data Plane: Bei Unsicherheit TX locked, OOB optional.
- Restart erfolgt über Supervisor (systemd/rc.d/SMF), nicht durch Systemreboot.

## Fortschritt vs. Liveness
Liveness bedeutet: Modul tickt.
Fortschritt bedeutet: System bewegt sich (commit_epoch/seq_cnt/peer seen).

## Restart-Log
Der Watchdog schreibt beim Restart einen Logeintrag in:
- /var/log/commanderlink/cl_watchdog.log

Format: eine Zeile pro Restart (maschinenlesbar)
- ts_ns=<uint64>
- reason=<wd_reason>
- module=<wd_module>
- last_progress_ns=<uint64>
- last_commit_epoch=<uint64>
- backend=<enum>
- profile=<enum>
- constraints=0x...
- mesh=<short>
- node=<short>
- restart_count=<u32>

Zeitumwandlung in lesbar erfolgt im Monitor (age/uptime).

## Monitor Pflichtanzeige
Dashboard:
- WATCHDOG: OK/WARN/STALL/PANIC (farbig, bold bei nicht OK)
- RESTARTS: total count
- LAST: age + reason/module

Forensics:
- zeigt die letzten 5 Restart-bezogenen Events

## Forensics Events (append-only)
- WD_RESTART_REQUEST
- WD_SELF_EXIT
- WD_HARD_KILL
- WD_RESTARTED


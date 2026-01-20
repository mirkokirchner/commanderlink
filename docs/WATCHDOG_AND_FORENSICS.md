# CommanderLink – WATCHDOG & FORENSICS

## Ziel
Automatische Selbstheilung ohne Datenverlust.

---

## 1. Watchdog-Aufgabe

- Überwacht CL-Prozess
- Erkennt Deadlocks / Panics
- Restartet CL

---

## 2. Restart-Regeln

- Immer Fail-Open
- Keine Kernel-Abhängigkeit
- Kein System-Neustart

---

## 3. Forensik-Log

Bei jedem Restart:
- Zeitstempel
- Grund
- letzter Path-State
- letzte Constraint-Flags

---

## 4. Monitor-Integration

- Watchdog-Status sichtbar
- Letzte 5 Restart-Events
- Lesbare Zeitformate

---

## 5. Merksatz

> **Ein Neustart ist kein Fehler, sondern Wartung.**


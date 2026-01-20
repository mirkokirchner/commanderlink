# CommanderLink  
**Deterministic Machine State Engine & Runtime Constitution Layer**

CommanderLink ist kein Monitoring-Tool.  
CommanderLink ist ein **Maschinen-Wahrheitskern**, der auf jedem System eine einzige, deterministische, atomare und signierte Wahrheit über den Zustand der Maschine bereitstellt.

Es ersetzt Heuristiken durch Verträge.  
Es ersetzt Vermutungen durch deterministische Segmente.  
Es ersetzt „Best Effort“ durch **Maschinenrecht**.

CommanderLink ist ein **OS-adjazenter Wahrheitslayer**, der unterhalb klassischer Software arbeitet, ohne das Betriebssystem zu ersetzen.

---

## 🚀 Motivation

Moderne Betriebssysteme liefern keine konsistente Wahrheit über ihren Zustand.  
Sie bestehen aus Heuristiken, adaptiven Schedulern, Power-Management-Tricks, Treiber-Optimierungen und nicht-deterministischen Entscheidungen.

Das Ergebnis:

- unvorhersehbare Performance  
- nicht reproduzierbare Latenzen  
- unklare Ursachen für Ruckler, Drops, Spikes  
- Monitoring, das nur Symptome sieht  
- Telemetrie, die nicht vertrauenswürdig ist  

CommanderLink löst dieses Problem durch:

- **deterministische SHM-Segmente**  
- **atomare Commits**  
- **ABI-stabile Strukturen**  
- **signierte Zustandsübergänge**  
- **strikte HAL/CORE-Trennung**  
- **Runtime-Verfassung (Laws & Contracts)**  

CommanderLink liefert **Maschinenwahrheit**, nicht Interpretationen.

---

## 🧩 Architekturüberblick

CommanderLink besteht aus mehreren klar getrennten Komponenten:

### **HAL0 – Hardware Abstraction Layer**
- Sammelt physische Maschinenzustände (CPU, Memory, NIC, DMA, PCIe, Time, Forensics)
- Schreibt deterministische SHM-Segmente
- Atomic Commit Contract: `PREPARE → WRITE → COMMIT`
- Signiert alle Commits (ephemerer Signer)
- Keine Interpretation, keine Heuristik

### **CORE0 – Runtime Constitution**
- Validiert alle HAL-Segmente
- Erzwingt ABI-Fingerprint, CRC, Signatur, seq_no-Monotonie
- Publiziert gültige Segmente
- Quarantäne bei Verstößen
- Gate-Engine & Budget-Mechanik

### **LINK0 – Mesh & Cluster State**
- Publiziert lokale Maschinenwahrheit an Nachbarn
- Konsolidiert Cluster-Zustände
- Optional, deterministisch, signiert

### **FLOW0 – Event & Command Pipeline**
- Deterministische Ereignisverarbeitung
- Keine Threads, keine Races, keine Heuristik

### **ORACLE0 – Predictive Quality Engine**
- Nutzt deterministische Segmente für Vorhersagen
- Keine ML-Heuristiken, nur Maschinenlogik

### **MONITOR – Read-Only Operator Interface**
- Curses-basierte UI
- Zeigt SHM-Segmente, Health, Forensics
- Strikt read-only

### **SHM-SSOT – Single Source of Truth**
- Atomare, signierte, ABI-stabile Segmentstruktur
- Zero-Copy, Threadless, Deterministic

---

## 📐 Die Laws & Contracts

CommanderLink basiert auf einer Reihe unverhandelbarer Gesetze:

### **Zero-Laws**
1. **Determinismus ist Pflicht**  
2. **Identity ist absolut**  
3. **Zeit ist monoton**  

### **LAW_00–LAW_42 (Auszug)**
- **LAW_01:** Memory is a Contract  
- **LAW_12:** No Implicit Trust Paths  
- **LAW_20:** ABI is Immutable  
- **LAW_21:** Segments are Sacred  
- **LAW_22:** Runtime Gates  
- **LAW_25:** No Cross-Segment Leakage  
- **LAW_28:** Health is First-Class  
- **LAW_31:** Monitor is Read-Only  
- **LAW_40:** Everything must be documented  
- **LAW_41:** No Undefined Behavior  

Die vollständigen Laws befinden sich im Ordner `/docs/laws/`.

---

## 🧬 SHM Segment Layout (SSOT)

CommanderLink definiert ein festes, ABI-stabiles Layout:

- **CPU Segment**  
- **Memory Segment**  
- **NIC Segment**  
- **DMA Segment**  
- **PCIe Segment**  
- **Time Segment**  
- **ZFS Segment**  
- **Forensics Segment**  
- **History Segment**  
- **Mesh Segment**  
- **Budget Segment**  

Alle Segmente sind:

- atomar  
- signiert  
- CRC-validiert  
- ABI-stabil  
- zero-copy  
- deterministisch  

Details: `/include/cl_runtime/`

---

## 🛠️ Build & Run

### Voraussetzungen
- C Compiler (GCC/Clang)
- CMake
- ncurses (für Monitor)
- POSIX-kompatibles OS (Linux, FreeBSD, OmniOS)

### Build
```bash
mkdir build
cd build
cmake ..
make -j$(nproc)

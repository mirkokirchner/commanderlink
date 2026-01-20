commanderlink/
├── include/
│   ├── cl_manifest/
│   │   ├── cl_limits.h
│   │   ├── cl_ids.h
│   │   ├── cl_features.h
│   │   ├── cl_link_modes.h
│   │   ├── cl_wire.h
│   │   ├── cl_oob.h
│   │   └── cl_manifest.h
│   ├── cl_common/
│   │   ├── cl_atomic.h
│   │   ├── cl_endian.h
│   │   ├── cl_crc8.h
│   │   ├── cl_time.h
│   │   └── cl_tags.h
│   ├── cl_runtime/
│   │   ├── cl_toc.h
│   │   ├── cl_global.h
│   │   ├── cl_quality.h
│   │   ├── cl_segments_cpu.h
│   │   ├── cl_segments_nic.h
│   │   ├── cl_segments_mem.h
│   │   ├── cl_segments_pcie.h
│   │   ├── cl_segments_board.h
│   │   ├── cl_segments_time.h
│   │   ├── cl_segments_link.h
│   │   ├── cl_segments_budget.h
│   │   ├── cl_segments_mesh.h
│   │   ├── cl_segments_dma.h
│   │   └── cl_segments_zfs.h
│   ├── cl_hal/
│   │   └── cl_hal.h
│   ├── cl_core/
│   │   └── cl_core.h
│   ├── cl_flow/
│   │   └── cl_flow.h
│   ├── cl_link/
│   │   └── cl_link.h
│   ├── cl_oracle/
│   │   └── cl_oracle.h
│   └── cl_monitor/
│       └── cl_monitor.h
├── src/ (leer oder später)
└── tools/ (leer oder später)

src/monitor/
├── monitor_main.c          # main loop, init ncurses, dispatch tabs
├── monitor_state.c         # UI state (active tab, focus, selection, filter)
├── monitor_state.h
├── monitor_shm.c           # attach core/bulk shm, map pointers, bulk present
├── monitor_shm.h
├── monitor_toc.c           # helpers: find segment by type, compute record ptrs
├── monitor_toc.h
├── monitor_draw.c          # box/lines/colors/helpers, autoscaling layout grid
├── monitor_draw.h
├── monitor_keymap.c        # key handling (F1..F12, arrows, /, s, tab, h)
├── monitor_keymap.h
├── monitor_fmt.c           # formatting helpers (bps, ppm, us, ids short, Q|S)
├── monitor_fmt.h
├── tabs/
│   ├── tab_dashboard.c     # F1
│   ├── tab_inventory.c     # F2
│   ├── tab_cpu.c           # F3
│   ├── tab_memory.c        # F4
│   ├── tab_nic.c           # F5
│   ├── tab_mesh.c          # F6
│   ├── tab_dma.c           # F7
│   ├── tab_zfs.c           # F8
│   ├── tab_pcie.c          # F9
│   ├── tab_gpu.c           # F10
│   ├── tab_oracle.c        # F11
│   └── tab_audit.c         # F12
└── tabs/tab_common.h       # shared tab helpers

Erweiterung: 

commanderlink/
├─ src/
│  ├─ core/                     ← ZENTRALE SYSTEMWAHRHEIT
│  │  ├─ cl_gates.h              ← HIER!
│  │  ├─ cl_segment_types.h
│  │  ├─ cl_toc.h
│  │  ├─ cl_budget.h
│  │  └─ cl_timebase.h
│  │
│  ├─ seg/                      ← RUNTIME-SEGMENTE (SHM)
│  │  ├─ seg_cpu_1024.h
│  │  ├─ seg_memory_256.h
│  │  ├─ seg_nic_512.h
│  │  ├─ seg_pcie_256.h
│  │  ├─ seg_budget_256.h
│  │  ├─ seg_overlay_256.h
│  │  ├─ seg_watchdog_256.h
│  │  ├─ seg_mesh_neighbor_256.h
│  │  └─ seg_dma_4096.h
│  │
│  ├─ wire/                     ← DRAHTREALITÄT
│  │  ├─ cl_wire_common.h
│  │  ├─ cl_wire_clwp.h
│  │  ├─ cl_wire_oob.h
│  │  ├─ cl_wire_modes.h
│  │  └─ cl_wire_limits.h
│  │
│  ├─ runtime/                  ← AKTIVE LOGIK (später)
│  │  ├─ flow/
│  │  ├─ link/
│  │  └─ hal/
│  │
│  ├─ monitor/                  ← PASSIVER LESER
│  ├─ tools/
│  └─ tests/
│
├─ docs/
│  ├─ laws/
│  └─ contracts/
│
└─ Makefile


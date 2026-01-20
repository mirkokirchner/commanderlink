# ============================================================================
# CommanderLink – Makefile (SSOT path, Endausbau-Bootkette)
# ============================================================================
# Baut:
#   - tools/cl_abi_report            (Truth Meter)
#   - src/clinit/clinit              (Genesis, ZERO)
#   - src/cld/cld + cl_identity      (einziger Dienst/Supervisor + Identity)
#   - src/core/core0                 (Stub: heartbeat)
#   - src/hal/hal0                   (Stub: heartbeat)
#   - src/link/link0                 (Stub: heartbeat)
#   - src/flow/flow0                 (Stub: heartbeat)
#   - src/oracle/oracle0             (Stub: heartbeat)
#   - src/monitor/*                  (SSOT UI; run with sudo in dev)
#
# Dummy/Dump: entfernt (nicht Bestandteil SSOT-Endausbaupfads)
# ============================================================================

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
  OS := linux
  LIBCURSES := -Llibs/raylib_drm/lib -lraylib -lGLESv2 -lEGL -ldrm -lgbm -lpthread -lrt -lm -ldl -linput
else ifeq ($(UNAME_S),FreeBSD)
  OS := bsd
  LIBCURSES := -Llibs/raylib_drm/lib -lraylib -lGLESv2 -lEGL -ldrm -lgbm -lpthread -lrt -lm -ldl -linput
else ifeq ($(UNAME_S),SunOS)
  OS := omnios
  LIBCURSES := -Llibs/raylib_drm/lib -lraylib -lGLESv2 -lEGL -ldrm -lgbm -lpthread -lrt -lm -ldl -linput
else
  OS := unknown
  LIBCURSES :=
endif

CC ?= cc

OPT  ?= -O2
CSTD ?= -std=c11
WARN ?= -Wall -Wextra -Wpedantic
DEFS ?= -D_POSIX_C_SOURCE=200809L
INCS ?= -Iinclude -Ilibs/raylib_drm/include

CFLAGS  ?= $(CSTD) $(OPT) $(WARN) $(DEFS) $(INCS)
LDFLAGS ?=
LDLIBS  ?=

ifeq ($(OS),bsd)
  CC ?= clang
endif
ifeq ($(OS),omnios)
  CC ?= gcc
endif

BUILD_DIR := build/$(OS)
BIN_DIR   := bin/$(OS)

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------
ABI_REPORT_SRC := tools/cl_abi_report/cl_abi_report.c

CLINIT_SRC := src/clinit/clinit.c

# cld besteht aus mehreren .c-Dateien (Identity ist Bestandteil des Dienstes)
CLD_SRC := \
  src/cld/cld.c \
  src/cld/cl_identity.c

CORE0_SRC   := src/core/core0.c
HAL0_SRC    := src/hal/hal0.c
LINK0_SRC   := src/link/link0.c
FLOW0_SRC   := src/flow/flow0.c
ORACLE0_SRC := src/oracle/oracle0.c

MONITOR_SRC := src/monitor/monitor.c \
               src/monitor/monitor_shm.c \
               src/monitor/monitor_state.c \
               src/monitor/tabs/tab_dashboard.c \
               src/monitor/monitor_model.c \
                src/clgui/clgui.c

#  src/monitor/monitor.c \
#  src/monitor/monitor_ui.c \
#  src/monitor/monitor_state.c \
#  src/monitor/monitor_input.c \
#  src/monitor/monitor_shm.c \
#  src/monitor/tabs/tab_dashboard.c

# Runtime common (link into multiple binaries)
RUNTIME_COMMON_SRC := src/runtime/cl_abi_fingerprint.c


# ---------------------------------------------------------------------------
# Objects
#   (Listen werden deterministisch aus Sources abgeleitet)
# ---------------------------------------------------------------------------
ABI_REPORT_OBJ := $(BUILD_DIR)/tools/cl_abi_report/cl_abi_report.o

CLINIT_OBJ := $(BUILD_DIR)/src/clinit/clinit.o

CLD_OBJ := $(patsubst src/%.c,$(BUILD_DIR)/src/%.o,$(CLD_SRC))

CORE0_OBJ   := $(BUILD_DIR)/src/core/core0.o
HAL0_OBJ    := $(BUILD_DIR)/src/hal/hal0.o
LINK0_OBJ   := $(BUILD_DIR)/src/link/link0.o
FLOW0_OBJ   := $(BUILD_DIR)/src/flow/flow0.o
ORACLE0_OBJ := $(BUILD_DIR)/src/oracle/oracle0.o

MONITOR_OBJ := $(patsubst src/%.c,$(BUILD_DIR)/src/%.o,$(MONITOR_SRC))

RUNTIME_COMMON_OBJ := $(BUILD_DIR)/src/runtime/cl_abi_fingerprint.o


# ---------------------------------------------------------------------------
# Binaries
# ---------------------------------------------------------------------------
ABI_REPORT_BIN := $(BIN_DIR)/cl_abi_report
CLINIT_BIN     := $(BIN_DIR)/clinit
CLD_BIN        := $(BIN_DIR)/cld

CORE0_BIN   := $(BIN_DIR)/core0
HAL0_BIN    := $(BIN_DIR)/hal0
LINK0_BIN   := $(BIN_DIR)/link0
FLOW0_BIN   := $(BIN_DIR)/flow0
ORACLE0_BIN := $(BIN_DIR)/oracle0

MONITOR_BIN := $(BIN_DIR)/cl_monitor

# Optionaler zusätzlicher Monitor-Lib Hook (falls du später z.B. panel/menu brauchst)
MONITOR_LDLIBS ?=

# ---------------------------------------------------------------------------
# Phony
# ---------------------------------------------------------------------------
.PHONY: all clean dirs help print-vars
.PHONY: abi abi-ct clinit cld
.PHONY: core0 hal0 link0 flow0 oracle0 stubs
.PHONY: monitor
.PHONY: run-init run-abi run-destroy run-cld run-monitor

# ---------------------------------------------------------------------------
# Default build
# ---------------------------------------------------------------------------
all: abi clinit cld stubs monitor

# ---------------------------------------------------------------------------
# Directories
# ---------------------------------------------------------------------------
dirs:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)
	@mkdir -p $(BUILD_DIR)/tools/cl_abi_report
	@mkdir -p $(BUILD_DIR)/src/clinit
	@mkdir -p $(BUILD_DIR)/src/cld
	@mkdir -p $(BUILD_DIR)/src/core
	@mkdir -p $(BUILD_DIR)/src/hal
	@mkdir -p $(BUILD_DIR)/src/link
	@mkdir -p $(BUILD_DIR)/src/flow
	@mkdir -p $(BUILD_DIR)/src/oracle
	@mkdir -p $(BUILD_DIR)/src/monitor
	@mkdir -p $(BUILD_DIR)/src/monitor/tabs
	@mkdir -p $(BUILD_DIR)/src/runtime
	@mkdir -p $(BUILD_DIR)/src/clgui

# ============================================================================
# COMPILER BLOCK (einheitlich für alle .c -> .o)
#   Diese Regel ist SSOT: sie baut jedes src/*.c nach build/<os>/src/*.o
# ============================================================================
$(BUILD_DIR)/src/%.o: src/%.c | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/tools/%.o: tools/%.c | dirs
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================================================
# LINKER BLOCKS (je Binary)
#   Regel: Objekte -> Libs -> LDFLAGS
# ============================================================================
# ABI report
$(ABI_REPORT_BIN): $(ABI_REPORT_OBJ) $(RUNTIME_COMMON_OBJ) | dirs
	$(CC) -o $@ $^ $(LDLIBS) $(LDFLAGS)

abi: $(ABI_REPORT_BIN)

abi-ct: $(ABI_REPORT_OBJ)
	@echo "ABI compile-only OK: $(ABI_REPORT_OBJ)"

# clinit
$(CLINIT_BIN): $(CLINIT_OBJ) $(RUNTIME_COMMON_OBJ) | dirs
	$(CC) -o $@ $^ $(LDLIBS) $(LDFLAGS)

clinit: $(CLINIT_BIN)

# cld (Dienst + Identity)
$(CLD_BIN): $(CLD_OBJ) $(RUNTIME_COMMON_OBJ) | dirs
	$(CC) -o $@ $^ $(LDLIBS) $(LDFLAGS)

cld: $(CLD_BIN)

# Stub services
$(CORE0_BIN): $(CORE0_OBJ) $(RUNTIME_COMMON_OBJ) | dirs
	$(CC) -o $@ $^ $(LDLIBS) $(LDFLAGS)
core0: $(CORE0_BIN)

$(HAL0_BIN): $(HAL0_OBJ) | dirs
	$(CC) -o $@ $^ $(LDLIBS) $(LDFLAGS)
hal0: $(HAL0_BIN)

$(LINK0_BIN): $(LINK0_OBJ) | dirs
	$(CC) -o $@ $^ $(LDLIBS) $(LDFLAGS)
link0: $(LINK0_BIN)

$(FLOW0_BIN): $(FLOW0_OBJ) | dirs
	$(CC) -o $@ $^ $(LDLIBS) $(LDFLAGS)
flow0: $(FLOW0_BIN)

$(ORACLE0_BIN): $(ORACLE0_OBJ) | dirs
	$(CC) -o $@ $^ $(LDLIBS) $(LDFLAGS)
oracle0: $(ORACLE0_BIN)

stubs: core0 hal0 link0 flow0 oracle0

# Monitor
$(MONITOR_BIN): $(MONITOR_OBJ) | dirs
	$(CC) -o $@ $(MONITOR_OBJ) $(LIBCURSES) $(MONITOR_LDLIBS) $(LDFLAGS)

monitor: $(MONITOR_BIN)

# ---------------------------------------------------------------------------
# Run helpers
# ---------------------------------------------------------------------------
run-init: clinit
	@echo "Running clinit (core-only): $(CLINIT_BIN)"
	@$(CLINIT_BIN) || true

run-destroy: clinit
	@echo "Destroying SHM objects via clinit --destroy"
	@$(CLINIT_BIN) --destroy || true

run-abi: abi
	@echo "Running ABI report: $(ABI_REPORT_BIN)"
	@$(ABI_REPORT_BIN) || true

run-cld: cld
	@echo "Running cld (service supervisor): $(CLD_BIN)"
	@$(CLD_BIN) || true

run-monitor: monitor
	@echo "Running monitor with sudo: $(MONITOR_BIN)"
	@sudo $(MONITOR_BIN) || true

# ---------------------------------------------------------------------------
# Clean
# ---------------------------------------------------------------------------
clean:
	rm -rf build bin

# ---------------------------------------------------------------------------
# Diagnostics / help
# ---------------------------------------------------------------------------
print-vars:
	@echo "CommanderLink Makefile (SSOT path)"
	@echo "  UNAME_S        = $(UNAME_S)"
	@echo "  OS             = $(OS)"
	@echo "  CC             = $(CC)"
	@echo "  CFLAGS         = $(CFLAGS)"
	@echo "  BUILD_DIR      = $(BUILD_DIR)"
	@echo "  BIN_DIR        = $(BIN_DIR)"
	@echo "  LIBCURSES      = $(LIBCURSES)"
	@echo "  MONITOR_LDLIBS  = $(MONITOR_LDLIBS)"

help:
	@echo "CommanderLink (CL) – SSOT Build"
	@echo ""
	@echo "Detected OS: $(UNAME_S) -> bin/$(OS)/"
	@echo ""
	@echo "Targets:"
	@echo "  make all         Build abi + clinit + cld + stubs + monitor"
	@echo "  make abi         Build cl_abi_report"
	@echo "  make abi-ct      Compile-only ABI gate"
	@echo "  make clinit      Build clinit"
	@echo "  make cld         Build cld (service supervisor)"
	@echo "  make stubs       Build core0/hal0/link0/flow0/oracle0"
	@echo "  make monitor     Build cl_monitor"
	@echo "  make clean       Remove build/ and bin/"
	@echo ""
	@echo "Run helpers:"
	@echo "  make run-init     Create/refresh SHM (core-only)"
	@echo "  make run-destroy  Remove SHM objects"
	@echo "  make run-abi      Run ABI report"
	@echo "  make run-cld      Run cld (supervisor)"
	@echo "  make run-monitor  Run monitor via sudo"


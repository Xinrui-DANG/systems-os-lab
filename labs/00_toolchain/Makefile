CC ?= gcc
CFLAGS ?= -O2 -Wall -Wextra -std=c11

BUILD_DIR := build
TARGET := $(BUILD_DIR)/cpu_affinity_check
SRC := src/cpu_affinity_check.c

.PHONY: all run profile perf clean help

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET) 0

profile:
	./scripts/collect_machine_profile.sh

perf: $(TARGET)
	./scripts/perf_event_probe.sh ./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

help:
	@echo "Available targets:"
	@echo "  make all      Build cpu_affinity_check"
	@echo "  make run      Run CPU affinity check"
	@echo "  make profile  Collect machine profile"
	@echo "  make perf     Probe perf counters"
	@echo "  make clean    Remove build files"

COMPLETED_LABS := labs/01_memory_hierarchy

.PHONY: all help clean run lab00 run-lab00 profile perf-baseline lab01 run-lab01

all:
	@for lab in $(COMPLETED_LABS); do \
		$(MAKE) -C $$lab; \
	done

run:
	@for lab in $(COMPLETED_LABS); do \
		$(MAKE) -C $$lab run; \
	done

clean:
	@for lab in $(COMPLETED_LABS); do \
		$(MAKE) -C $$lab clean; \
	done

lab01:
	$(MAKE) -C labs/01_memory_hierarchy

run-lab01:
	$(MAKE) -C labs/01_memory_hierarchy run

lab00:
	$(MAKE) -C labs/00_toolchain

run-lab00:
	$(MAKE) -C labs/00_toolchain run

profile:
	$(MAKE) -C labs/00_toolchain profile

perf-baseline:
	$(MAKE) -C labs/00_toolchain perf

help:
	@echo "Available targets:"
	@echo "  make all        Build completed labs only"
	@echo "  make run        Run completed labs only"
	@echo "  make clean      Clean generated files from completed labs"
	@echo "  make lab00      Build labs/00_toolchain"
	@echo "  make run-lab00  Run CPU affinity baseline check"
	@echo "  make profile    Collect machine profile"
	@echo "  make perf-baseline  Probe perf counter availability"
	@echo "  make lab01      Build labs/01_memory_hierarchy"
	@echo "  make run-lab01  Run labs/01_memory_hierarchy"

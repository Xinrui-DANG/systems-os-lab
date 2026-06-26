COMPLETED_LABS := labs/01_memory_hierarchy

.PHONY: all help clean run lab01 run-lab01

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

help:
	@echo "Available targets:"
	@echo "  make all        Build completed labs only"
	@echo "  make run        Run completed labs only"
	@echo "  make clean      Clean generated files from completed labs"
	@echo "  make lab01      Build labs/01_memory_hierarchy"
	@echo "  make run-lab01  Run labs/01_memory_hierarchy"

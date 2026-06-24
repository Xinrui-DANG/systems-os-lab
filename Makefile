.PHONY: all run clean cache-latency sequential-vs-random cache-line-effect run-cache-latency run-sequential-vs-random run-cache-line-effect

all: cache-latency sequential-vs-random cache-line-effect

cache-latency:
	$(MAKE) -C labs/01_memory_hierarchy/cache_latency

sequential-vs-random:
	$(MAKE) -C labs/01_memory_hierarchy/sequential_vs_random

cache-line-effect:
	$(MAKE) -C labs/01_memory_hierarchy/cache_line_effect

run-cache-latency:
	$(MAKE) -C labs/01_memory_hierarchy/cache_latency run

run-sequential-vs-random:
	$(MAKE) -C labs/01_memory_hierarchy/sequential_vs_random run

run-cache-line-effect:
	$(MAKE) -C labs/01_memory_hierarchy/cache_line_effect run

run: run-cache-latency run-sequential-vs-random run-cache-line-effect

clean:
	$(MAKE) -C labs/01_memory_hierarchy/cache_latency clean
	$(MAKE) -C labs/01_memory_hierarchy/sequential_vs_random clean
	$(MAKE) -C labs/01_memory_hierarchy/cache_line_effect clean

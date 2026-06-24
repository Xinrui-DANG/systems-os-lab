.PHONY: all run clean cache-latency sequential-vs-random run-cache-latency run-sequential-vs-random

all: cache-latency sequential-vs-random

cache-latency:
	$(MAKE) -C labs/01_memory_hierarchy/cache_latency

sequential-vs-random:
	$(MAKE) -C labs/01_memory_hierarchy/sequential_vs_random

run-cache-latency:
	$(MAKE) -C labs/01_memory_hierarchy/cache_latency run

run-sequential-vs-random:
	$(MAKE) -C labs/01_memory_hierarchy/sequential_vs_random run

run: run-cache-latency run-sequential-vs-random

clean:
	$(MAKE) -C labs/01_memory_hierarchy/cache_latency clean
	$(MAKE) -C labs/01_memory_hierarchy/sequential_vs_random clean

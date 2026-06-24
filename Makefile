.PHONY: all run clean cache-latency sequential-vs-random cache-line-effect false-sharing prefetch-effect numa-or-l3-cluster run-cache-latency run-sequential-vs-random run-cache-line-effect run-false-sharing run-prefetch-effect run-numa-or-l3-cluster

all: cache-latency sequential-vs-random cache-line-effect false-sharing prefetch-effect numa-or-l3-cluster

cache-latency:
	$(MAKE) -C labs/01_memory_hierarchy/cache_latency

sequential-vs-random:
	$(MAKE) -C labs/01_memory_hierarchy/sequential_vs_random

cache-line-effect:
	$(MAKE) -C labs/01_memory_hierarchy/cache_line_effect

false-sharing:
	$(MAKE) -C labs/01_memory_hierarchy/false_sharing

prefetch-effect:
	$(MAKE) -C labs/01_memory_hierarchy/prefetch_effect

numa-or-l3-cluster:
	$(MAKE) -C labs/01_memory_hierarchy/numa_or_l3_cluster

run-cache-latency:
	$(MAKE) -C labs/01_memory_hierarchy/cache_latency run

run-sequential-vs-random:
	$(MAKE) -C labs/01_memory_hierarchy/sequential_vs_random run

run-cache-line-effect:
	$(MAKE) -C labs/01_memory_hierarchy/cache_line_effect run

run-false-sharing:
	$(MAKE) -C labs/01_memory_hierarchy/false_sharing run

run-prefetch-effect:
	$(MAKE) -C labs/01_memory_hierarchy/prefetch_effect run

run-numa-or-l3-cluster:
	$(MAKE) -C labs/01_memory_hierarchy/numa_or_l3_cluster run

run: run-cache-latency run-sequential-vs-random run-cache-line-effect run-false-sharing run-prefetch-effect run-numa-or-l3-cluster

clean:
	$(MAKE) -C labs/01_memory_hierarchy/cache_latency clean
	$(MAKE) -C labs/01_memory_hierarchy/sequential_vs_random clean
	$(MAKE) -C labs/01_memory_hierarchy/cache_line_effect clean
	$(MAKE) -C labs/01_memory_hierarchy/false_sharing clean
	$(MAKE) -C labs/01_memory_hierarchy/prefetch_effect clean
	$(MAKE) -C labs/01_memory_hierarchy/numa_or_l3_cluster clean

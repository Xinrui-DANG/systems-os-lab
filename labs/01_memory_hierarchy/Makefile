.PHONY: all run clean cache-latency sequential-vs-random cache-line-effect false-sharing prefetch-effect numa-or-l3-cluster run-cache-latency run-sequential-vs-random run-cache-line-effect run-false-sharing run-prefetch-effect run-numa-or-l3-cluster

all: cache-latency sequential-vs-random cache-line-effect false-sharing prefetch-effect numa-or-l3-cluster

cache-latency:
	$(MAKE) -C cache_latency

sequential-vs-random:
	$(MAKE) -C sequential_vs_random

cache-line-effect:
	$(MAKE) -C cache_line_effect

false-sharing:
	$(MAKE) -C false_sharing

prefetch-effect:
	$(MAKE) -C prefetch_effect

numa-or-l3-cluster:
	$(MAKE) -C numa_or_l3_cluster

run-cache-latency:
	$(MAKE) -C cache_latency run

run-sequential-vs-random:
	$(MAKE) -C sequential_vs_random run

run-cache-line-effect:
	$(MAKE) -C cache_line_effect run

run-false-sharing:
	$(MAKE) -C false_sharing run

run-prefetch-effect:
	$(MAKE) -C prefetch_effect run

run-numa-or-l3-cluster:
	$(MAKE) -C numa_or_l3_cluster run

run: run-cache-latency run-sequential-vs-random run-cache-line-effect run-false-sharing run-prefetch-effect run-numa-or-l3-cluster

clean:
	$(MAKE) -C cache_latency clean
	$(MAKE) -C sequential_vs_random clean
	$(MAKE) -C cache_line_effect clean
	$(MAKE) -C false_sharing clean
	$(MAKE) -C prefetch_effect clean
	$(MAKE) -C numa_or_l3_cluster clean

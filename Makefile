.PHONY: all run clean cache-latency

all: cache-latency

cache-latency:
	$(MAKE) -C labs/01_memory_hierarchy/cache_latency

run:
	$(MAKE) -C labs/01_memory_hierarchy/cache_latency run

clean:
	$(MAKE) -C labs/01_memory_hierarchy/cache_latency clean

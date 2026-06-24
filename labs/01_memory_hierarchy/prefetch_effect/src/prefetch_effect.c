#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    const char *name;
    double ns_per_access;
    double mib_per_second;
} result_t;

static volatile uint64_t sum_sink = 0;
static volatile size_t idx_sink = 0;

static double now_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return (double)ts.tv_sec * 1e9 + (double)ts.tv_nsec;
}

static void shuffle(size_t *a, size_t n)
{
    for (size_t i = n - 1; i > 0; --i) {
        size_t j = (size_t)rand() % (i + 1);
        size_t tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
    }
}

static result_t measure_sequential(const uint64_t *data,
                                   size_t n,
                                   size_t passes)
{
    uint64_t sum = 0;
    size_t accesses = n * passes;

    /*
     * Plain sequential access. Hardware prefetchers are usually very good at
     * this pattern because future addresses are predictable.
     */
    double t0 = now_ns();
    for (size_t pass = 0; pass < passes; ++pass) {
        for (size_t i = 0; i < n; ++i) {
            sum += data[i];
        }
    }
    double t1 = now_ns();

    sum_sink = sum;

    double elapsed_ns = t1 - t0;
    return (result_t){
        .name = "sequential",
        .ns_per_access = elapsed_ns / (double)accesses,
        .mib_per_second = ((double)accesses * sizeof(uint64_t)) /
                          (elapsed_ns / 1e9) / (1024.0 * 1024.0),
    };
}

static result_t measure_software_prefetch(const uint64_t *data,
                                          size_t n,
                                          size_t passes,
                                          size_t distance)
{
    uint64_t sum = 0;
    size_t accesses = n * passes;

    /*
     * Software prefetch asks the CPU to bring a future cache line closer before
     * it is needed. On simple sequential scans, hardware prefetching may
     * already do enough, so explicit prefetch can help, do nothing, or even add
     * overhead depending on the machine.
     */
    double t0 = now_ns();
    for (size_t pass = 0; pass < passes; ++pass) {
        for (size_t i = 0; i < n; ++i) {
            if (i + distance < n) {
                __builtin_prefetch(&data[i + distance], 0, 1);
            }
            sum += data[i];
        }
    }
    double t1 = now_ns();

    sum_sink = sum;

    double elapsed_ns = t1 - t0;
    return (result_t){
        .name = "software_prefetch",
        .ns_per_access = elapsed_ns / (double)accesses,
        .mib_per_second = ((double)accesses * sizeof(uint64_t)) /
                          (elapsed_ns / 1e9) / (1024.0 * 1024.0),
    };
}

static result_t measure_random_chase(const size_t *next,
                                     size_t start,
                                     size_t accesses)
{
    size_t idx = start;

    /*
     * Random dependent pointer chasing is the counterexample: the next address
     * is unknown until the current load finishes, so ordinary prefetchers have
     * little useful pattern to learn.
     */
    double t0 = now_ns();
    for (size_t i = 0; i < accesses; ++i) {
        idx = next[idx];
    }
    double t1 = now_ns();

    idx_sink = idx;

    double elapsed_ns = t1 - t0;
    return (result_t){
        .name = "random_chase",
        .ns_per_access = elapsed_ns / (double)accesses,
        .mib_per_second = ((double)accesses * sizeof(size_t)) /
                          (elapsed_ns / 1e9) / (1024.0 * 1024.0),
    };
}

int main(int argc, char **argv)
{
    srand(1);

    const size_t bytes = argc > 1 ? strtoull(argv[1], NULL, 10) :
                                    256 * 1024 * 1024UL;
    const size_t passes = argc > 2 ? strtoull(argv[2], NULL, 10) : 8;
    const size_t prefetch_distance = argc > 3 ? strtoull(argv[3], NULL, 10) : 64;
    const size_t n = bytes / sizeof(uint64_t);
    const size_t random_accesses = n * passes;

    uint64_t *data = NULL;
    size_t *next = NULL;
    size_t *perm = NULL;

    if (posix_memalign((void **)&data, 64, n * sizeof(uint64_t)) != 0) {
        perror("posix_memalign data");
        return 1;
    }

    if (posix_memalign((void **)&next, 64, n * sizeof(size_t)) != 0) {
        perror("posix_memalign next");
        free(data);
        return 1;
    }

    perm = malloc(n * sizeof(size_t));
    if (!perm) {
        perror("malloc perm");
        free(next);
        free(data);
        return 1;
    }

    for (size_t i = 0; i < n; ++i) {
        data[i] = (uint64_t)i;
        perm[i] = i;
    }

    shuffle(perm, n);
    for (size_t i = 0; i < n; ++i) {
        next[perm[i]] = perm[(i + 1) % n];
    }

    size_t start = perm[0];
    for (size_t i = 0; i < n; ++i) {
        start = next[start];
    }

    result_t sequential = measure_sequential(data, n, passes);
    result_t software_prefetch =
        measure_software_prefetch(data, n, passes, prefetch_distance);
    result_t random_chase = measure_random_chase(next, start, random_accesses);

    printf("working set:        %zu MiB\n", bytes / (1024 * 1024));
    printf("passes:             %zu\n", passes);
    printf("prefetch distance:  %zu elements (%zu bytes)\n\n",
           prefetch_distance,
           prefetch_distance * sizeof(uint64_t));

    printf("%18s %15s %15s %15s\n",
           "case",
           "ns/access",
           "MiB/s",
           "vs sequential");

    printf("%18s %15.3f %15.1f %15.2f\n",
           sequential.name,
           sequential.ns_per_access,
           sequential.mib_per_second,
           sequential.ns_per_access / sequential.ns_per_access);

    printf("%18s %15.3f %15.1f %15.2f\n",
           software_prefetch.name,
           software_prefetch.ns_per_access,
           software_prefetch.mib_per_second,
           software_prefetch.ns_per_access / sequential.ns_per_access);

    printf("%18s %15.3f %15.1f %15.2f\n",
           random_chase.name,
           random_chase.ns_per_access,
           random_chase.mib_per_second,
           random_chase.ns_per_access / sequential.ns_per_access);

    free(perm);
    free(next);
    free(data);
    return 0;
}

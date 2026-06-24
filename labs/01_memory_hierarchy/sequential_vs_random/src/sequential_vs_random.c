#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * The benchmark stores final results here so the compiler must keep the
 * measured loops. Without an observable result, optimization could erase the
 * work we are trying to time.
 */
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
    /* Fisher-Yates shuffle with a fixed seed from main(). */
    for (size_t i = n - 1; i > 0; --i) {
        size_t j = (size_t)rand() % (i + 1);
        size_t tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
    }
}

static double measure_sequential(const uint64_t *data, size_t n, size_t accesses)
{
    uint64_t sum = 0;
    size_t idx = 0;
    const size_t mask = n - 1;

    /*
     * Sequential access has a simple predictable address stream. Modern CPUs
     * can detect this pattern, prefetch upcoming cache lines, and overlap many
     * memory requests.
     */
    double t0 = now_ns();
    for (size_t i = 0; i < accesses; ++i) {
        sum += data[idx];
        idx = (idx + 1) & mask;
    }
    double t1 = now_ns();

    sum_sink = sum;
    return (t1 - t0) / (double)accesses;
}

static double measure_random_chase(const size_t *next, size_t start, size_t accesses)
{
    size_t idx = start;

    /*
     * Random pointer chasing is deliberately hostile to hardware prefetching.
     * The next address is unknown until the current load returns.
     */
    double t0 = now_ns();
    for (size_t i = 0; i < accesses; ++i) {
        idx = next[idx];
    }
    double t1 = now_ns();

    idx_sink = idx;
    return (t1 - t0) / (double)accesses;
}

int main(void)
{
    srand(1);

    const size_t min_bytes = 4 * 1024;
    const size_t max_bytes = 256 * 1024 * 1024;
    const size_t min_accesses = 10000000UL;

    printf("%12s %15s %15s %12s\n",
           "Working set",
           "seq ns/access",
           "rnd ns/access",
           "rnd/seq");

    for (size_t bytes = min_bytes; bytes <= max_bytes; bytes *= 2) {
        size_t n = bytes / sizeof(uint64_t);
        size_t accesses = n > min_accesses ? n : min_accesses;

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

        size_t idx = perm[0];
        for (size_t i = 0; i < n; ++i) {
            idx = next[idx];
        }

        double seq_ns = measure_sequential(data, n, accesses);
        double rnd_ns = measure_random_chase(next, idx, accesses);

        printf("%9zu KiB %15.3f %15.3f %12.2f\n",
               bytes / 1024,
               seq_ns,
               rnd_ns,
               rnd_ns / seq_ns);

        free(perm);
        free(next);
        free(data);
    }

    return 0;
}

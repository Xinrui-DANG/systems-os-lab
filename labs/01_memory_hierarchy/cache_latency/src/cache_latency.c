#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

/*
 * Keep the final pointer-chasing result visible to the compiler.
 * Without this, an optimizer could decide that the measured loop has no
 * externally observable effect and remove or simplify it.
 */
static volatile size_t sink = 0;

static double now_ns(void)
{
    struct timespec ts;
    /*
     * MONOTONIC_RAW is a steady hardware-backed clock on Linux. It avoids
     * wall-clock adjustments, which makes it suitable for short benchmarks.
     */
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return (double)ts.tv_sec * 1e9 + (double)ts.tv_nsec;
}

static void shuffle(size_t *a, size_t n)
{
    /* Fisher-Yates shuffle: produce a deterministic random permutation. */
    for (size_t i = n - 1; i > 0; --i) {
        size_t j = (size_t)rand() % (i + 1);
        size_t tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
    }
}

int main(void)
{
    /*
     * Fixed seed makes runs reproducible. The access pattern is random-like,
     * but the same binary on the same machine follows the same permutation.
     */
    srand(1);

    const size_t min_bytes = 4 * 1024;
    const size_t max_bytes = 512 * 1024 * 1024;
    const size_t steps = 10000000UL;

    printf("%12s %15s\n", "Working set", "ns/access");

    for (size_t bytes = min_bytes; bytes <= max_bytes; bytes *= 2) {
        size_t n = bytes / sizeof(size_t);

        size_t *next = NULL;
        /*
         * Align the array to a cache-line boundary. This does not remove all
         * alignment effects, but it gives each working-set size a cleaner
         * starting point.
         */
        if (posix_memalign((void **)&next, 64, n * sizeof(size_t)) != 0) {
            perror("posix_memalign");
            return 1;
        }

        size_t *perm = malloc(n * sizeof(size_t));
        if (!perm) {
            perror("malloc");
            free(next);
            return 1;
        }

        for (size_t i = 0; i < n; ++i) {
            perm[i] = i;
        }

        shuffle(perm, n);

        /*
         * Build one random cycle:
         *
         *     perm[0] -> perm[1] -> ... -> perm[n - 1] -> perm[0]
         *
         * The benchmark later stores the current position in idx and advances
         * with idx = next[idx]. Because the next address is only known after
         * the current load completes, the CPU cannot easily prefetch or
         * overlap many independent loads.
         */
        for (size_t i = 0; i < n; ++i) {
            next[perm[i]] = perm[(i + 1) % n];
        }

        size_t idx = perm[0];

        /* Warm up: touch the whole cycle once before timing. */
        for (size_t i = 0; i < n; ++i) {
            idx = next[idx];
        }

        double t0 = now_ns();

        /*
         * Timed dependent-load loop. The total elapsed time divided by steps
         * is an approximate ns/access latency for this working-set size.
         */
        for (size_t i = 0; i < steps; ++i) {
            idx = next[idx];
        }

        double t1 = now_ns();

        sink = idx;

        printf("%9zu KiB %15.3f\n", bytes / 1024, (t1 - t0) / steps);

        free(perm);
        free(next);
    }

    return 0;
}

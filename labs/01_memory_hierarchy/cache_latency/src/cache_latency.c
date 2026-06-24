#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

static volatile size_t sink = 0;

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

int main(void)
{
    srand(1);

    const size_t min_bytes = 4 * 1024;
    const size_t max_bytes = 512 * 1024 * 1024;
    const size_t steps = 10000000UL;

    printf("%12s %15s\n", "Working set", "ns/access");

    for (size_t bytes = min_bytes; bytes <= max_bytes; bytes *= 2) {
        size_t n = bytes / sizeof(size_t);

        size_t *next = NULL;
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

        for (size_t i = 0; i < n; ++i) {
            next[perm[i]] = perm[(i + 1) % n];
        }

        size_t idx = perm[0];

        for (size_t i = 0; i < n; ++i) {
            idx = next[idx];
        }

        double t0 = now_ns();

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

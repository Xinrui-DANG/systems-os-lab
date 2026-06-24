#define _GNU_SOURCE
#include <errno.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    const char *name;
    int warm_cpu;
    int measure_cpu;
    double ns_per_access;
} result_t;

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

static int pin_to_cpu(int cpu)
{
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu, &set);

    if (sched_setaffinity(0, sizeof(set), &set) != 0) {
        fprintf(stderr, "warning: could not pin to CPU %d: %s\n",
                cpu,
                strerror(errno));
        return -1;
    }

    return 0;
}

static size_t chase(const size_t *next, size_t idx, size_t steps)
{
    for (size_t i = 0; i < steps; ++i) {
        idx = next[idx];
    }
    return idx;
}

static result_t run_case(const char *name,
                         const size_t *next,
                         size_t start,
                         size_t working_set_items,
                         size_t timed_steps,
                         int warm_cpu,
                         int measure_cpu)
{
    size_t idx = start;

    /*
     * First touch the whole pointer cycle while pinned to warm_cpu. Then move
     * to measure_cpu and time the same dependent-load chain. If the two CPUs
     * do not share the same private caches or local L3 slice/cluster, the
     * measured latency can change.
     */
    (void)pin_to_cpu(warm_cpu);
    idx = chase(next, idx, working_set_items);

    (void)pin_to_cpu(measure_cpu);
    double t0 = now_ns();
    idx = chase(next, idx, timed_steps);
    double t1 = now_ns();

    sink = idx;

    return (result_t){
        .name = name,
        .warm_cpu = warm_cpu,
        .measure_cpu = measure_cpu,
        .ns_per_access = (t1 - t0) / (double)timed_steps,
    };
}

static int pick_allowed_cpus(int *first, int *second, int *last)
{
    cpu_set_t allowed;
    CPU_ZERO(&allowed);

    if (sched_getaffinity(0, sizeof(allowed), &allowed) != 0) {
        perror("sched_getaffinity");
        return -1;
    }

    int count = 0;
    *first = -1;
    *second = -1;
    *last = -1;

    for (int cpu = 0; cpu < CPU_SETSIZE; ++cpu) {
        if (!CPU_ISSET(cpu, &allowed)) {
            continue;
        }

        if (*first == -1) {
            *first = cpu;
        } else if (*second == -1) {
            *second = cpu;
        }

        *last = cpu;
        ++count;
    }

    if (count == 0) {
        fprintf(stderr, "no allowed CPUs found\n");
        return -1;
    }

    if (*second == -1) {
        *second = *first;
    }

    return count;
}

int main(int argc, char **argv)
{
    srand(1);

    int first_cpu = 0;
    int second_cpu = 0;
    int last_cpu = 0;
    int allowed_count = pick_allowed_cpus(&first_cpu, &second_cpu, &last_cpu);
    if (allowed_count < 0) {
        return 1;
    }

    const size_t bytes = argc > 1 ? strtoull(argv[1], NULL, 10) :
                                    16 * 1024 * 1024UL;
    const size_t timed_steps = argc > 2 ? strtoull(argv[2], NULL, 10) :
                                          10000000UL;
    const int warm_cpu = argc > 3 ? atoi(argv[3]) : first_cpu;
    const int near_cpu = argc > 4 ? atoi(argv[4]) : second_cpu;
    const int far_cpu = argc > 5 ? atoi(argv[5]) : last_cpu;
    const size_t n = bytes / sizeof(size_t);

    size_t *next = NULL;
    size_t *perm = NULL;

    if (posix_memalign((void **)&next, 64, n * sizeof(size_t)) != 0) {
        perror("posix_memalign next");
        return 1;
    }

    perm = malloc(n * sizeof(size_t));
    if (!perm) {
        perror("malloc perm");
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

    size_t start = perm[0];

    result_t same = run_case("same_cpu",
                             next,
                             start,
                             n,
                             timed_steps,
                             warm_cpu,
                             warm_cpu);
    result_t near = run_case("near_cpu",
                             next,
                             start,
                             n,
                             timed_steps,
                             warm_cpu,
                             near_cpu);
    result_t far = run_case("far_cpu",
                            next,
                            start,
                            n,
                            timed_steps,
                            warm_cpu,
                            far_cpu);

    printf("allowed CPUs:       %d\n", allowed_count);
    printf("working set:        %zu MiB\n", bytes / (1024 * 1024));
    printf("timed accesses:     %zu\n\n", timed_steps);
    printf("%12s %10s %12s %15s %15s\n",
           "case",
           "warm cpu",
           "measure cpu",
           "ns/access",
           "vs same_cpu");

    printf("%12s %10d %12d %15.3f %15.2f\n",
           same.name,
           same.warm_cpu,
           same.measure_cpu,
           same.ns_per_access,
           same.ns_per_access / same.ns_per_access);
    printf("%12s %10d %12d %15.3f %15.2f\n",
           near.name,
           near.warm_cpu,
           near.measure_cpu,
           near.ns_per_access,
           near.ns_per_access / same.ns_per_access);
    printf("%12s %10d %12d %15.3f %15.2f\n",
           far.name,
           far.warm_cpu,
           far.measure_cpu,
           far.ns_per_access,
           far.ns_per_access / same.ns_per_access);

    free(perm);
    free(next);
    return 0;
}

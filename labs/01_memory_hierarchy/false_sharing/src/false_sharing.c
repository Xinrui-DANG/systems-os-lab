#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum {
    CACHE_LINE_BYTES = 64,
    THREADS = 2
};

typedef struct {
    volatile uint64_t a;
    volatile uint64_t b;
} shared_counters_t;

typedef struct __attribute__((aligned(CACHE_LINE_BYTES))) {
    volatile uint64_t value;
    char padding[CACHE_LINE_BYTES - sizeof(uint64_t)];
} padded_counter_t;

typedef struct {
    padded_counter_t a;
    padded_counter_t b;
} padded_counters_t;

_Static_assert(sizeof(padded_counter_t) == CACHE_LINE_BYTES,
               "padded counter should occupy exactly one cache line");

typedef struct {
    int thread_id;
    int padded;
    size_t iterations;
    int cpu_id;
    void *counters;
} worker_args_t;

static double now_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return (double)ts.tv_sec * 1e9 + (double)ts.tv_nsec;
}

static void pin_to_cpu(int cpu_id)
{
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu_id, &set);

    /*
     * Pinning reduces scheduler noise and makes cross-core cache coherence
     * traffic easier to observe. If the requested CPU is not available, keep
     * running rather than failing the benchmark.
     */
    (void)pthread_setaffinity_np(pthread_self(), sizeof(set), &set);
}

static void *worker(void *arg)
{
    worker_args_t *args = arg;
    pin_to_cpu(args->cpu_id);

    if (args->padded) {
        padded_counters_t *counters = args->counters;
        volatile uint64_t *counter =
            args->thread_id == 0 ? &counters->a.value : &counters->b.value;

        for (size_t i = 0; i < args->iterations; ++i) {
            *counter += 1;
        }
    } else {
        shared_counters_t *counters = args->counters;
        volatile uint64_t *counter =
            args->thread_id == 0 ? &counters->a : &counters->b;

        for (size_t i = 0; i < args->iterations; ++i) {
            *counter += 1;
        }
    }

    return NULL;
}

static double run_case(int padded, size_t iterations, int cpu0, int cpu1)
{
    pthread_t threads[THREADS];
    worker_args_t args[THREADS];
    shared_counters_t shared = {0};
    padded_counters_t padded_counters = {0};
    void *counters = padded ? (void *)&padded_counters : (void *)&shared;

    double t0 = now_ns();

    for (int i = 0; i < THREADS; ++i) {
        args[i].thread_id = i;
        args[i].padded = padded;
        args[i].iterations = iterations;
        args[i].cpu_id = i == 0 ? cpu0 : cpu1;
        args[i].counters = counters;

        if (pthread_create(&threads[i], NULL, worker, &args[i]) != 0) {
            perror("pthread_create");
            exit(1);
        }
    }

    for (int i = 0; i < THREADS; ++i) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            exit(1);
        }
    }

    double t1 = now_ns();

    if (padded) {
        volatile uint64_t total = padded_counters.a.value + padded_counters.b.value;
        (void)total;
    } else {
        volatile uint64_t total = shared.a + shared.b;
        (void)total;
    }

    return (t1 - t0) / 1e9;
}

int main(int argc, char **argv)
{
    const size_t iterations = argc > 1 ? strtoull(argv[1], NULL, 10) : 200000000UL;
    const int cpu0 = argc > 2 ? atoi(argv[2]) : 0;
    const int cpu1 = argc > 3 ? atoi(argv[3]) : 1;
    const double total_writes = (double)iterations * (double)THREADS;

    printf("iterations/thread: %zu\n", iterations);
    printf("cpu placement:     %d, %d\n\n", cpu0, cpu1);
    printf("%16s %12s %18s %15s\n",
           "case",
           "seconds",
           "million writes/s",
           "ns/write");

    double false_s = run_case(0, iterations, cpu0, cpu1);
    double padded_s = run_case(1, iterations, cpu0, cpu1);

    printf("%16s %12.3f %18.1f %15.3f\n",
           "false sharing",
           false_s,
           total_writes / false_s / 1e6,
           false_s * 1e9 / total_writes);

    printf("%16s %12.3f %18.1f %15.3f\n",
           "padded",
           padded_s,
           total_writes / padded_s / 1e6,
           padded_s * 1e9 / total_writes);

    printf("\nspeedup from padding: %.2fx\n", false_s / padded_s);

    return 0;
}

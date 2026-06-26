#define _GNU_SOURCE

#include <errno.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static long long now_ns(void)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }

    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

static void busy_work(void)
{
    volatile unsigned long long x = 0;

    for (unsigned long long i = 0; i < 100000000ULL; i++) {
        x += i;
    }

    if (x == 0) {
        printf("unexpected\n");
    }
}

int main(int argc, char **argv)
{
    int target_cpu = 0;

    if (argc >= 2) {
        target_cpu = atoi(argv[1]);
    }

    printf("Process ID: %d\n", getpid());
    printf("Requested CPU: %d\n", target_cpu);

    int before_cpu = sched_getcpu();
    if (before_cpu < 0) {
        perror("sched_getcpu before");
    } else {
        printf("CPU before pinning: %d\n", before_cpu);
    }

    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(target_cpu, &set);

    if (sched_setaffinity(0, sizeof(set), &set) != 0) {
        fprintf(stderr, "sched_setaffinity failed: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    int after_cpu = sched_getcpu();
    if (after_cpu < 0) {
        perror("sched_getcpu after");
    } else {
        printf("CPU after pinning: %d\n", after_cpu);
    }

    long long start = now_ns();
    busy_work();
    long long end = now_ns();

    printf("Busy work time: %.3f ms\n", (double)(end - start) / 1000000.0);

    return EXIT_SUCCESS;
}

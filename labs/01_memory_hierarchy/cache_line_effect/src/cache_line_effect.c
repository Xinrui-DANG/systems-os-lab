#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum {
    CACHE_LINE_BYTES = 64
};

/*
 * Keep the load results visible to the compiler so the timed loop remains real
 * memory work instead of becoming dead code.
 */
static volatile uint64_t sink = 0;

static double now_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return (double)ts.tv_sec * 1e9 + (double)ts.tv_nsec;
}

static double useful_cache_line_percent(size_t stride)
{
    /*
     * The program reads one byte at each address. With stride=1 it eventually
     * uses all 64 bytes in a cache line. With stride=64 it uses one byte from
     * each fetched cache line. Larger strides still use only one byte from each
     * cache line they touch.
     */
    size_t useful_bytes = stride < CACHE_LINE_BYTES ? CACHE_LINE_BYTES / stride : 1;
    return 100.0 * (double)useful_bytes / (double)CACHE_LINE_BYTES;
}

static double measure_stride(const uint8_t *data,
                             size_t bytes,
                             size_t stride,
                             size_t min_accesses)
{
    uint64_t sum = 0;
    size_t accesses_per_pass = bytes / stride;
    size_t passes = (min_accesses + accesses_per_pass - 1) / accesses_per_pass;
    size_t accesses = passes * accesses_per_pass;

    /*
     * Stride controls spatial locality:
     *
     * stride=1   reads every byte in a fetched cache line.
     * stride=64  reads one byte per cache line.
     * stride=4096 reads one byte per page-sized gap.
     */
    double t0 = now_ns();
    for (size_t pass = 0; pass < passes; ++pass) {
        for (size_t i = 0; i < bytes; i += stride) {
            sum += data[i];
        }
    }
    double t1 = now_ns();

    sink = sum;
    return (t1 - t0) / (double)accesses;
}

int main(void)
{
    const size_t bytes = 256 * 1024 * 1024UL;
    const size_t min_accesses = 10000000UL;
    const size_t strides[] = {
        1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 4096
    };

    uint8_t *data = NULL;
    if (posix_memalign((void **)&data, CACHE_LINE_BYTES, bytes) != 0) {
        perror("posix_memalign");
        return 1;
    }

    for (size_t i = 0; i < bytes; ++i) {
        data[i] = (uint8_t)i;
    }

    printf("%10s %15s %18s %15s\n",
           "Stride",
           "ns/access",
           "line utilization",
           "accesses");

    for (size_t i = 0; i < sizeof(strides) / sizeof(strides[0]); ++i) {
        size_t stride = strides[i];
        size_t accesses_per_pass = bytes / stride;
        size_t passes = (min_accesses + accesses_per_pass - 1) / accesses_per_pass;
        size_t accesses = passes * accesses_per_pass;
        double ns = measure_stride(data, bytes, stride, min_accesses);

        printf("%7zu B %15.3f %17.2f%% %15zu\n",
               stride,
               ns,
               useful_cache_line_percent(stride),
               accesses);
    }

    free(data);
    return 0;
}

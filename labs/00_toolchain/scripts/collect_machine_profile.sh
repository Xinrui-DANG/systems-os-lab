#!/usr/bin/env bash
set -euo pipefail

OUT_DIR="results"
OUT_FILE="${OUT_DIR}/machine_profile_$(date +%F_%H-%M-%S).txt"

mkdir -p "$OUT_DIR"

{
    echo "===== Machine Profile ====="
    echo "Date: $(date --iso-8601=seconds)"
    echo

    echo "===== Kernel ====="
    uname -a
    echo

    echo "===== OS Release ====="
    if [ -f /etc/os-release ]; then
        cat /etc/os-release
    else
        echo "/etc/os-release not found"
    fi
    echo

    echo "===== Compiler ====="
    if command -v gcc >/dev/null 2>&1; then
        gcc --version | head -n 1
    else
        echo "gcc not found"
    fi

    if command -v clang >/dev/null 2>&1; then
        clang --version | head -n 1
    else
        echo "clang not found"
    fi

    if command -v make >/dev/null 2>&1; then
        make --version | head -n 1
    else
        echo "make not found"
    fi
    echo

    echo "===== CPU Topology ====="
    lscpu
    echo

    echo "===== CPU Caches ====="
    if lscpu --caches >/dev/null 2>&1; then
        lscpu --caches
    else
        echo "lscpu --caches not available"
    fi
    echo

    echo "===== /proc/cpuinfo Summary ====="
    grep -m 1 "model name" /proc/cpuinfo || true
    grep -m 1 "cpu cores" /proc/cpuinfo || true
    grep -m 1 "siblings" /proc/cpuinfo || true
    echo

    echo "===== Memory ====="
    grep -E "MemTotal|MemFree|MemAvailable|SwapTotal|SwapFree" /proc/meminfo
    echo

    echo "===== Cache sysfs for cpu0 ====="
    for c in /sys/devices/system/cpu/cpu0/cache/index*; do
        [ -d "$c" ] || continue
        echo "---- $c ----"
        echo -n "level: "; cat "$c/level" 2>/dev/null || true
        echo -n "type: "; cat "$c/type" 2>/dev/null || true
        echo -n "size: "; cat "$c/size" 2>/dev/null || true
        echo -n "coherency_line_size: "; cat "$c/coherency_line_size" 2>/dev/null || true
        echo -n "ways_of_associativity: "; cat "$c/ways_of_associativity" 2>/dev/null || true
        echo -n "number_of_sets: "; cat "$c/number_of_sets" 2>/dev/null || true
        echo -n "shared_cpu_list: "; cat "$c/shared_cpu_list" 2>/dev/null || true
        echo
    done

    echo "===== Tools ====="
    command -v taskset || true
    command -v perf || true
    command -v numactl || true

    if command -v perf >/dev/null 2>&1; then
        perf --version
    fi
} > "$OUT_FILE"

echo "Wrote machine profile to $OUT_FILE"

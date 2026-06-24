#!/usr/bin/env bash
set -euo pipefail

out_dir="${1:-results/machine_profiles}"
mkdir -p "$out_dir"

lscpu > "$out_dir/lscpu.txt"
cat /proc/cpuinfo > "$out_dir/cpuinfo.txt"
cat /proc/meminfo > "$out_dir/meminfo.txt"

if [ -d /sys/devices/system/cpu/cpu0/cache ]; then
    find /sys/devices/system/cpu/cpu0/cache -maxdepth 2 -type f -print -exec cat {} \; \
        > "$out_dir/cpu0_cache_sysfs.txt" 2>/dev/null || true
fi

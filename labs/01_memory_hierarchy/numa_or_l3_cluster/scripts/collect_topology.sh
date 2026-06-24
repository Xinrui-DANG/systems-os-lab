#!/usr/bin/env bash
set -euo pipefail

out_dir="${1:-results}"
mkdir -p "$out_dir"

{
    echo "# lscpu -e=CPU,CORE,SOCKET,NODE,CACHE"
    lscpu -e=CPU,CORE,SOCKET,NODE,CACHE
    echo
    echo "# lscpu"
    lscpu
    echo
    echo "# shared_cpu_list"
    find /sys/devices/system/cpu/cpu*/cache -maxdepth 2 \
        -type f -name shared_cpu_list -print -exec cat {} \; 2>/dev/null
} > "$out_dir/topology.txt"

echo "wrote $out_dir/topology.txt"

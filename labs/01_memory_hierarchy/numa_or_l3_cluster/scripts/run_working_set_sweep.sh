#!/usr/bin/env bash
set -euo pipefail

dir="$(dirname "$0")/.."
steps="${1:-10000000}"
warm="${2:-0}"
same_l3_core="${3:-1}"
other_l3_core="${4:-8}"

make -C "$dir"

for bytes in 1048576 4194304 16777216 67108864; do
    echo
    echo "# working set: $bytes bytes"
    "$dir/build/numa_or_l3_cluster" "$bytes" "$steps" "$warm" "$same_l3_core" "$other_l3_core"
done

#!/usr/bin/env bash
set -euo pipefail

dir="$(dirname "$0")/.."
bytes="${1:-16777216}"
steps="${2:-10000000}"
warm="${3:-0}"
smt_sibling="${4:-16}"
same_l3_core="${5:-1}"
other_l3_core="${6:-8}"

make -C "$dir"

echo "# same CPU, SMT sibling, same-L3 core"
"$dir/build/numa_or_l3_cluster" "$bytes" "$steps" "$warm" "$smt_sibling" "$same_l3_core"

echo
echo "# same CPU, same-L3 core, other-L3 core"
"$dir/build/numa_or_l3_cluster" "$bytes" "$steps" "$warm" "$same_l3_core" "$other_l3_core"

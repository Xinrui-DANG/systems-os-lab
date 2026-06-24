#!/usr/bin/env bash
set -euo pipefail

dir="$(dirname "$0")/.."
repeats="${1:-7}"
bytes="${2:-16777216}"
steps="${3:-10000000}"
warm="${4:-0}"
same_l3_core="${5:-1}"
other_l3_core="${6:-8}"

make -C "$dir" >/dev/null

tmp="$(mktemp)"
trap 'rm -f "$tmp"' EXIT

for i in $(seq 1 "$repeats"); do
    "$dir/build/numa_or_l3_cluster" "$bytes" "$steps" "$warm" "$same_l3_core" "$other_l3_core" |
        awk -v run="$i" '
            $1 == "same_cpu" { print run, "same_cpu", $4 }
            $1 == "near_cpu" { print run, "same_l3", $4 }
            $1 == "far_cpu" { print run, "other_l3", $4 }
        ' >> "$tmp"
done

echo "repeats:        $repeats"
echo "working set:    $((bytes / 1024 / 1024)) MiB"
echo "timed accesses: $steps"
echo "placement:      warm=$warm same_l3=$same_l3_core other_l3=$other_l3_core"
echo
printf "%10s %12s %12s %12s\n" "case" "min_ns" "median_ns" "max_ns"

awk '
    {
        values[$2] = values[$2] " " $3
    }
    END {
        order[1] = "same_cpu"
        order[2] = "same_l3"
        order[3] = "other_l3"

        for (i = 1; i <= 3; ++i) {
            name = order[i]
            split(values[name], a, " ")
            n = 0
            for (j in a) {
                if (a[j] != "") {
                    v[++n] = a[j] + 0
                }
            }
            for (x = 1; x <= n; ++x) {
                for (y = x + 1; y <= n; ++y) {
                    if (v[y] < v[x]) {
                        t = v[x]
                        v[x] = v[y]
                        v[y] = t
                    }
                }
            }
            median = v[int((n + 1) / 2)]
            printf "%10s %12.3f %12.3f %12.3f\n", name, v[1], median, v[n]
            delete v
        }
    }
' "$tmp"

echo
echo "raw runs:"
cat "$tmp"

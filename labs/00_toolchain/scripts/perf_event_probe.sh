#!/usr/bin/env bash
set -euo pipefail

TARGET="${1:-./build/cpu_affinity_check}"
OUT_DIR="results"
OUT_FILE="${OUT_DIR}/perf_probe_$(date +%F_%H-%M-%S).txt"

mkdir -p "$OUT_DIR"

{
    echo "===== perf probe ====="
    echo "Date: $(date --iso-8601=seconds)"
    echo

    if ! command -v perf >/dev/null 2>&1; then
        echo "perf not found"
        exit 0
    fi

    echo "===== perf version ====="
    perf --version
    echo

    echo "===== selected perf list entries ====="
    perf list 2>/dev/null | grep -m 80 -E "cycles|instructions|cache-misses|cache-references|context-switches|task-clock" || true
    echo

    echo "===== perf stat test ====="

    if [ ! -x "$TARGET" ]; then
        echo "target not executable: $TARGET"
        exit 0
    fi

    perf stat \
        -e task-clock,context-switches,cpu-migrations,cycles,instructions,cache-references,cache-misses \
        "$TARGET" 0 || echo "perf stat failed; check perf_event permissions or event support"
} > "$OUT_FILE" 2>&1

echo "Wrote perf probe to $OUT_FILE"

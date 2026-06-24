#!/usr/bin/env bash
set -euo pipefail

if [ "$#" -lt 1 ]; then
    echo "usage: $0 <command> [args...]" >&2
    exit 2
fi

"$@"

#!/usr/bin/env bash
set -euo pipefail

make -C "$(dirname "$0")/.."
"$(dirname "$0")/../build/false_sharing" "$@"

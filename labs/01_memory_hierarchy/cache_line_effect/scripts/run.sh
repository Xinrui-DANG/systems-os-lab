#!/usr/bin/env bash
set -euo pipefail

make -C "$(dirname "$0")/.."
"$(dirname "$0")/../build/cache_line_effect"

#!/usr/bin/env python3
"""Small placeholder for plotting CSV experiment results."""

import sys


def main() -> int:
    if len(sys.argv) < 2:
        print("usage: plot_csv.py <file.csv>", file=sys.stderr)
        return 2

    print(f"plotting is not implemented yet: {sys.argv[1]}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3
import argparse
from pathlib import Path


def metadata(path):
    values = {}
    for line in Path(path).read_text().splitlines():
        if "=" in line:
            key, value = line.split("=", 1)
            values[key] = value
    return values


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--stat", required=True)
    parser.add_argument("--metadata", required=True)
    parser.add_argument("--output", required=True)
    args = parser.parse_args()
    values = metadata(args.metadata)
    stat = Path(args.stat).read_text() if Path(args.stat).exists() else "perf output missing"
    lines = [
        "# Hardware-counter benchmark",
        "",
        "> Dockerized `perf stat` around the exact-decimal representation benchmark. No decoding work is included.",
        "",
        "## Run contract",
        "",
        f"- Encodes per repetition: `{values.get('iterations', '?')}`",
        f"- Repetitions: `{values.get('repetitions', '?')}`",
        f"- perf exit status: `{values.get('perf_exit_status', '?')}`",
        "- Events requested: cycles, instructions, cache references, cache misses, branches, and branch misses.",
        "",
        "## Raw perf output",
        "",
        "```text",
        stat.rstrip(),
        "```",
        "",
        "If events are unavailable under the container host, the output is retained explicitly rather than being presented as a valid counter measurement. Repeat this phase on production Linux hardware with the required perf permissions for authoritative microarchitectural comparisons.",
        "",
    ]
    Path(args.output).write_text("\n".join(lines))


if __name__ == "__main__":
    main()

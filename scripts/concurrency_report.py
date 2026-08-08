#!/usr/bin/env python3
import argparse
import csv
import glob
import statistics
from pathlib import Path


def fmt(value):
    return f"{value:,.2f}"


def metadata(path):
    values = {}
    for line in Path(path).read_text().splitlines():
        if "=" in line:
            key, value = line.split("=", 1)
            values[key] = value
    return values


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--glob", required=True)
    parser.add_argument("--metadata", required=True)
    parser.add_argument("--output", required=True)
    args = parser.parse_args()
    rows = []
    for path in sorted(glob.glob(args.glob)):
        with open(path, newline="") as handle:
            rows.extend(csv.DictReader(handle))
    groups = {}
    for row in rows:
        groups.setdefault((row["mode"], row["threads"]), []).append(row)
    meta = metadata(args.metadata)
    lines = [
        "# Concurrency and contention benchmark",
        "",
        "> Protobuf encoding only. Each row contains one million total encodes per repetition across the configured threads.",
        "",
        "## Run contract",
        "",
        f"- Configurations: `{meta.get('runs', '?')}`",
        f"- Total encodes per repetition: `{meta.get('iterations', '?')}`",
        f"- Repetitions: `{meta.get('repetitions', '?')}`",
        f"- Thread counts: `{meta.get('threads', '?')}`",
        "",
        "## Results",
        "",
        "| Mode | Threads | Reps | Bytes | Median ns/encode | Mean ns/encode | Mean M/s |",
        "|---|---:|---:|---:|---:|---:|---:|",
    ]
    for (mode, threads), values in sorted(groups.items(), key=lambda item: (item[0][0], int(item[0][1]))):
        samples = [float(row["ns_per_encode"]) for row in values]
        lines.append(f"| {mode} | {threads} | {len(values)} | {values[-1]['bytes']} | {fmt(statistics.median(samples))} | {fmt(statistics.mean(samples))} | {fmt(statistics.mean(1_000_000_000 / value / 1_000_000 for value in samples))} |")
    lines += [
        "",
        "## Mode definitions",
        "",
        "- `thread_local`: each worker has its own protobuf message and output buffer.",
        "- `shared_readonly`: workers serialize the same immutable message into independent buffers.",
        "- `shared_buffer_mutex`: workers serialize into one shared buffer under a mutex, exposing lock contention.",
        "- Thread creation and barrier setup are outside the timed encode loop.",
        "",
    ]
    Path(args.output).write_text("\n".join(lines))


if __name__ == "__main__":
    main()

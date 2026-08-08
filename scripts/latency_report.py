#!/usr/bin/env python3
import argparse
import csv
import statistics
from collections import defaultdict
from pathlib import Path


def fmt(value):
    return f"{value:,.2f}"


def meta(path):
    values = {}
    for line in Path(path).read_text().splitlines():
        if "=" in line:
            key, value = line.split("=", 1)
            values[key] = value
    return values


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--csv", required=True)
    parser.add_argument("--metadata", required=True)
    parser.add_argument("--output", required=True)
    args = parser.parse_args()
    rows = list(csv.DictReader(open(args.csv, newline="")))
    groups = defaultdict(list)
    for row in rows:
        groups[(row["test_case"], row["path"])].append(row)
    metadata = meta(args.metadata)
    lines = [
        "# Per-encode latency and allocation benchmark",
        "",
        "> Every percentile below is calculated from one million individually timed encodes per repetition. No decoding or network work is included.",
        "",
        "## Run contract",
        "",
        f"- Encodes per repetition: `{metadata.get('iterations', '?')}`",
        f"- Repetitions: `{metadata.get('repetitions', '?')}`",
        f"- Warmup encodes excluded: `{metadata.get('warmup_iterations', '?')}`",
        "- Instrumentation uses a timer around every encode, so these are instrumented latency measurements and should be compared within this report, not directly substituted for the aggregate throughput benchmark.",
        "",
        "## Results",
        "",
        "| Payload | Path | Reps | Bytes | Median p50 ns | p90 ns | p99 ns | p99.9 ns | p99.99 ns | Max ns | Mean allocations/rep | Mean allocated bytes/rep |",
        "|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|",
    ]
    for (test_case, path), values in sorted(groups.items()):
        def med(field):
            return statistics.median(float(row[field]) for row in values)
        lines.append(
            f"| {test_case} | {path} | {len(values)} | {values[-1]['bytes']} | {fmt(med('p50_ns'))} | {fmt(med('p90_ns'))} | {fmt(med('p99_ns'))} | {fmt(med('p99_9_ns'))} | {fmt(med('p99_99_ns'))} | {fmt(med('max_ns'))} | {fmt(statistics.mean(int(row['allocations']) for row in values))} | {fmt(statistics.mean(int(row['allocated_bytes']) for row in values))} |"
        )
    lines += [
        "",
        "## Allocation interpretation",
        "",
        "- `serialize_array_reuse` measures a caller-owned buffer reused across encodes.",
        "- `serialize_array_fresh_buffer` intentionally allocates a new output vector for each encode and exposes allocator cost.",
        "- `serialize_string_reserved` reuses a reserved string, while `serialize_array_arena_message` serializes a message allocated on a protobuf Arena.",
        "- Allocation counts include allocations observed by the benchmark process during the timed loop; they are not a decoding metric.",
        "",
    ]
    Path(args.output).write_text("\n".join(lines))


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
import argparse
import csv
import statistics
from collections import defaultdict
from pathlib import Path


def fmt(value):
    return f"{value:,.2f}"


def percentile(values, fraction):
    ordered = sorted(values)
    index = min(len(ordered) - 1, max(0, int((len(ordered) * fraction + 0.999999) - 1)))
    return ordered[index]


def metadata(path):
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
        groups[row["representation"]].append(row)
    meta = metadata(args.metadata)
    lines = [
        "# Exact decimal representation benchmark",
        "",
        "> Protobuf encoding only. No decoding, parsing, or network work is included.",
        "",
        "## Run contract",
        "",
        f"- Encodes per repetition: `{meta.get('iterations', '?')}`",
        f"- Repetitions: `{meta.get('repetitions', '?')}`",
        f"- Warmup encodes excluded: `{meta.get('warmup_iterations', '?')}`",
        f"- Integer/binary scale: `{meta.get('decimal_scale', '?')}` decimal places",
        "- The integer and binary representations use the same fixed-scale numeric values; the string representation preserves decimal text.",
        "",
        "## Results",
        "",
        "| Representation | Reps | Bytes | Median ns/encode | Mean ns/encode | p95 ns | p99 ns | p99.9 ns | p99.99 ns | Mean M/s |",
        "|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|",
    ]
    for representation, values in sorted(groups.items(), key=lambda item: statistics.median(float(row["ns_per_encode"]) for row in item[1])):
        samples = [float(row["ns_per_encode"]) for row in values]
        mean = statistics.mean(samples)
        lines.append(
            f"| {representation} | {len(values)} | {values[-1]['bytes']} | {fmt(statistics.median(samples))} | {fmt(mean)} | {fmt(percentile(samples, .95))} | {fmt(percentile(samples, .99))} | {fmt(percentile(samples, .999))} | {fmt(percentile(samples, .9999))} | {fmt(1_000_000_000 / mean / 1_000_000)} |"
        )
    lines += [
        "",
        "## Trade-off",
        "",
        "- Strings are self-describing and preserve exact text, but include decimal characters and protobuf length-delimited overhead.",
        "- Fixed-scale `sint64` is compact for values that fit the agreed scale and range; the scale must be part of the schema contract.",
        "- `fixed64` avoids varint-size variation but always spends eight coefficient bytes.",
        "- `bytes` with a 16-byte coefficient supports a wider exact range, at the cost of a larger fixed payload.",
        "",
    ]
    Path(args.output).write_text("\n".join(lines))


if __name__ == "__main__":
    main()

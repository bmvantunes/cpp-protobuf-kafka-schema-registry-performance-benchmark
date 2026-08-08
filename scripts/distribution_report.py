#!/usr/bin/env python3
import argparse
import csv
import statistics
from collections import defaultdict
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
    parser.add_argument("--csv", required=True)
    parser.add_argument("--metadata", required=True)
    parser.add_argument("--output", required=True)
    args = parser.parse_args()
    rows = list(csv.DictReader(open(args.csv, newline="")))
    groups = defaultdict(list)
    for row in rows:
        groups[row["distribution"]].append(row)
    meta = metadata(args.metadata)
    lines = [
        "# Realistic value distribution benchmark",
        "",
        "> Protobuf encoding only. No decoding or network work is included.",
        "",
        "## Run contract",
        "",
        f"- Encodes per repetition: `{meta.get('iterations', '?')}`",
        f"- Repetitions: `{meta.get('repetitions', '?')}`",
        f"- Warmup encodes excluded: `{meta.get('warmup_iterations', '?')}`",
        f"- Fixed corpus size: `{meta.get('corpus_size', '?')}` pre-populated messages",
        "- The corpus changes which values are serialized without measuring message construction or random-number generation inside the timed loop.",
        "",
        "## Results",
        "",
        "| Distribution | Reps | Mean bytes | Median ns/encode | Mean ns/encode | Mean M/s |",
        "|---|---:|---:|---:|---:|---:|",
    ]
    for distribution, values in sorted(groups.items(), key=lambda item: statistics.median(float(row["ns_per_encode"]) for row in item[1])):
        samples = [float(row["ns_per_encode"]) for row in values]
        lines.append(f"| {distribution} | {len(values)} | {fmt(statistics.mean(float(row['mean_bytes']) for row in values))} | {fmt(statistics.median(samples))} | {fmt(statistics.mean(samples))} | {fmt(statistics.mean(1_000_000_000 / value / 1_000_000 for value in samples))} |")
    lines += [
        "",
        "## Interpretation",
        "",
        "- Standard negative `int64` values use the ordinary protobuf int64 wire encoding and can expand to ten bytes each; compare them with small positive varints.",
        "- Large positive values expose the upper varint-width path without changing the field type.",
        "- Mixed values approximate branch and payload-size variation in a live feed.",
        "- Long and multibyte strings isolate length-delimited payload size and UTF-8 byte-count effects.",
        "",
    ]
    Path(args.output).write_text("\n".join(lines))


if __name__ == "__main__":
    main()

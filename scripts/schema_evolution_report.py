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
        groups[row["phase"]].append(row)
    meta = metadata(args.metadata)
    lines = [
        "# Schema Registry evolution benchmark",
        "",
        "> Control-plane schema evolution only. No decoding or per-message network path is included.",
        "",
        "## Run contract",
        "",
        f"- Repetitions: `{meta.get('repetitions', '?')}`",
        "- Each repetition registers v1, registers v2 under the same subject, then lists the subject versions.",
        "- v2 adds `venue` while preserving v1 fields and the same fully-qualified Protobuf message name.",
        "",
        "## Results",
        "",
        "| Phase | Reps | Status codes | Median ns | Mean ns | p95 ns | Response bytes |",
        "|---|---:|---|---:|---:|---:|---:|",
    ]
    for phase, values in sorted(groups.items()):
        samples = [float(row["elapsed_ns"]) for row in values]
        ordered = sorted(samples)
        p95 = ordered[min(len(ordered) - 1, int(len(ordered) * .95))]
        lines.append(f"| {phase} | {len(values)} | {', '.join(sorted(set(row['status'] for row in values)))} | {fmt(statistics.median(samples))} | {fmt(statistics.mean(samples))} | {fmt(p95)} | {statistics.median(int(row['bytes']) for row in values)} |")
    lines += [
        "",
        "## Interpretation",
        "",
        "- A successful v2 registration under the v1 subject demonstrates the tested Registry compatibility path for this additive schema change.",
        "- This does not replace a full compatibility-policy matrix; production rollout should also test breaking changes and the configured BACKWARD/FORWARD/FULL policy explicitly.",
        "",
    ]
    Path(args.output).write_text("\n".join(lines))


if __name__ == "__main__":
    main()

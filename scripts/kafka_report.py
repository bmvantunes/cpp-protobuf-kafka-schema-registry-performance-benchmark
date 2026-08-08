#!/usr/bin/env python3
import argparse
import csv
import glob
import statistics
from pathlib import Path


def fmt(value):
    return f"{value:,.2f}"


def percentile(values, fraction):
    ordered = sorted(values)
    if not ordered:
        return 0.0
    index = min(len(ordered) - 1, max(0, int((len(ordered) * fraction + 0.999999) - 1)))
    return ordered[index]


def read_metadata(path):
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
    if not rows:
        raise SystemExit("no Kafka producer CSV files found")

    groups = {}
    for row in rows:
        key = tuple(row[name] for name in ("mode", "acks", "compression", "linger_ms", "batch_num_messages"))
        groups.setdefault(key, []).append(row)

    metadata = read_metadata(args.metadata)
    lines = [
        "# Kafka producer benchmark",
        "",
        "> Encoding plus real librdkafka producer handoff. No decoding benchmark is included.",
        "",
        "## Run contract",
        "",
        f"- Configurations: `{metadata.get('runs', '?')}`",
        f"- Encodes/messages per repetition: `{metadata.get('iterations', '?')}`",
        f"- Repetitions per configuration: `{metadata.get('repetitions', '?')}`",
        "- `enqueue` covers serialization plus the librdkafka `produce()` handoff.",
        "- `flush` covers the remaining producer/broker delivery time for the configured acknowledgement mode.",
        "- `end_to_end` is enqueue plus flush and is the relevant result for this producer-path benchmark.",
        "",
        "## Results",
        "",
        "| Mode | Acks | Compression | Linger ms | Batch messages | Reps | Bytes | Median enqueue ns/msg | Median end-to-end ns/msg | p95 end-to-end ns/msg | p99 end-to-end ns/msg | Errors |",
        "|---|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|",
    ]
    for key, values in sorted(groups.items()):
        enqueue = [float(row["enqueue_elapsed_ns"]) / int(row["iterations"]) for row in values]
        total = [float(row["end_to_end_ns"]) / int(row["iterations"]) for row in values]
        errors = sum(int(row["delivery_errors"]) for row in values)
        lines.append(
            f"| {key[0]} | {key[1]} | {key[2]} | {key[3]} | {key[4]} | {len(values)} | {values[-1]['bytes']} | {fmt(statistics.median(enqueue))} | {fmt(statistics.median(total))} | {fmt(percentile(total, 0.95))} | {fmt(percentile(total, 0.99))} | {errors} |"
        )

    lines += [
        "",
        "## Interpretation",
        "",
        "- Compare `copy` and `owned` carefully: the owned path transfers an allocated payload to librdkafka, while the copy path reuses a caller-owned buffer and asks librdkafka to copy it.",
        "- `acks=0` measures producer handoff with no broker acknowledgement guarantee; it must not be treated as durable delivery.",
        "- Compression and batching can reduce wire bytes at the cost of producer CPU and latency. The correct choice depends on the latency budget and broker/network constraints.",
        "- This report measures producer handoff and flush behavior, not consumer throughput or decoding.",
        "",
        "## Raw data",
        "",
        "Every configuration has one CSV file with all repetitions. The raw rows include enqueue, flush, end-to-end elapsed time, delivery errors, and configuration fields.",
        "",
    ]
    Path(args.output).write_text("\n".join(lines))


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
import argparse
import csv
import statistics
from pathlib import Path


def number(row, key):
    return float(row[key])


def fmt(value):
    return f"{value:,.2f}"


def median(rows, key):
    return statistics.median(number(row, key) for row in rows)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--csv", required=True)
    parser.add_argument("--metadata", required=True)
    parser.add_argument("--output", required=True)
    args = parser.parse_args()

    rows = list(csv.DictReader(Path(args.csv).open(newline="")))
    groups = {}
    for row in rows:
        groups.setdefault((row["mode"], row["test_case"]), []).append(row)
    metadata = {}
    for line in Path(args.metadata).read_text().splitlines():
        if "=" in line:
            key, value = line.split("=", 1)
            metadata[key] = value

    lines = [
        "# Asynchronous hot-path pipeline benchmark",
        "",
        "> Producer-side measurements cover the non-blocking event handoff. Protobuf and pipe formatting measurements cover worker-side work. No decoding, broker, or network work is included.",
        "",
        "## Run contract",
        "",
        f"- Iterations per row: `{metadata.get('iterations', '?')}`",
        f"- Measured repetitions: `{metadata.get('repetitions', '?')}`",
        f"- Warmup iterations: `{metadata.get('warmup_iterations', '?')}`",
        f"- Queue capacity: `{metadata.get('queue_capacity', '?')}` pointer slots",
        f"- Handoff model: `{metadata.get('handoff_model', '?')}`",
        "",
        "The queue is preallocated larger than one measured repetition so the producer never waits for the worker during the baseline handoff test. A dropped count is still reported and must be zero for a valid throughput row.",
        "",
        "## Results",
        "",
        "| Mode | Test case | Completed | Dropped | Payload + header bytes | Hot handoff p50 ns | Hot handoff p99 ns | Worker p50 ns | Worker p99 ns | Worker wall M/s |",
        "|---|---|---:|---:|---:|---:|---:|---:|---:|---:|",
    ]
    for (mode, test_case), group in sorted(groups.items()):
        completed = int(median(group, "completed"))
        dropped = int(median(group, "dropped"))
        worker_wall = median(group, "worker_wall_ns")
        worker_mps = (completed / worker_wall * 1_000) if worker_wall else 0
        lines.append(
            f"| {mode} | {test_case} | {completed:,} | {dropped:,} | {group[0]['payload_bytes']} | "
            f"{fmt(median(group, 'hot_p50_ns'))} | {fmt(median(group, 'hot_p99_ns'))} | "
            f"{fmt(median(group, 'worker_p50_ns'))} | {fmt(median(group, 'worker_p99_ns'))} | {fmt(worker_mps)} |"
        )

    lines += [
        "",
        "## Mode definitions",
        "",
        "- `sync_both`: control path; the producer performs Protobuf framing and pipe formatting itself.",
        "- `async_handoff`: producer performs only the non-blocking ring handoff; the worker drains the queue without serialization work.",
        "- `async_protobuf`: producer hands off; the worker populates the generated message and serializes the six-byte Confluent-framed payload.",
        "- `async_pipe`: producer hands off; the worker formats the pipe-delimited line into a reusable buffer.",
        "- `async_both`: producer hands off; the worker performs both Protobuf serialization and pipe formatting.",
        "",
        "## Interpretation",
        "",
        "The number to protect on the trading thread is hot handoff p50/p99, not worker encoding time. The worker rows answer whether the encoder can keep up with the event rate. `async_handoff` isolates the queue cost; `async_both` is the relevant design when one worker creates both Kafka bytes and human-readable log lines.",
        "",
        "The benchmark uses an immutable event-pool pointer handoff to model transferring ownership from a pool without hot-thread allocation or variable-size copying. A production implementation that copies a fixed-size event into the ring should be benchmarked as a separate queue variant because its cost depends on the exact event layout and string storage strategy.",
        "",
        "A non-zero dropped count means the configured worker/queue combination did not sustain the attempted event rate. It is not a successful 1M-encoding result and must be investigated rather than averaged away.",
        "",
    ]
    Path(args.output).write_text("\n".join(lines))


if __name__ == "__main__":
    main()

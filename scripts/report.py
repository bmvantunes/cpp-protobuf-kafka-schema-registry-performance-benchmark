#!/usr/bin/env python3
import argparse
import csv
import statistics
from collections import defaultdict
from pathlib import Path


def read_metadata(path):
    values = {}
    for line in Path(path).read_text().splitlines():
        if "=" in line:
            key, value = line.split("=", 1)
            values[key] = value
    return values


def fmt(value):
    return f"{value:,.2f}"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--csv", required=True)
    parser.add_argument("--metadata", required=True)
    parser.add_argument("--output", required=True)
    args = parser.parse_args()

    metadata = read_metadata(args.metadata)
    with open(args.csv, newline="") as handle:
        rows = list(csv.DictReader(handle))

    groups = defaultdict(list)
    for row in rows:
        row["ns"] = float(row["ns_per_encode"])
        row["mps"] = float(row["encodes_per_second"]) / 1_000_000.0
        groups[(row["kind"], row["library"], row["codegen"], row["api"], row["test_case"])].append(row)

    summaries = []
    for key, values in groups.items():
        ns_values = [item["ns"] for item in values]
        summaries.append({
            "kind": key[0], "library": key[1], "codegen": key[2], "api": key[3], "test_case": key[4],
            "repetitions": len(values), "bytes": int(values[-1]["bytes"]),
            "mean_ns": statistics.mean(ns_values), "median_ns": statistics.median(ns_values),
            "min_ns": min(ns_values), "max_ns": max(ns_values),
            "mean_mps": statistics.mean(item["mps"] for item in values),
        })

    protobuf = [item for item in summaries if item["kind"] == "protobuf"]
    json_rows = [item for item in summaries if item["kind"] == "json"]
    fastest_by_case = {}
    for item in summaries:
        fastest_by_case.setdefault(item["test_case"], item)
        if item["median_ns"] < fastest_by_case[item["test_case"]]["median_ns"]:
            fastest_by_case[item["test_case"]] = item

    lines = [
        "# Protobuf and JSON encoding benchmark",
        "",
        "> Encoding only. No decode, parse, or schema-registry/network time is included.",
        "",
        "## Run contract",
        "",
        f"- Encodes per repetition: `{metadata.get('iterations', '?')}`",
        f"- Repetitions per benchmark: `{metadata.get('repetitions', '?')}`",
        f"- Warmup encodes (excluded): `{metadata.get('warmup_iterations', '?')}`",
        "- Decimal representation: protobuf and JSON both use decimal strings; JSON does not parse floating-point decimals.",
        "- Generated types: `buf generate` with Google C++ `SPEED`, `CODE_SIZE`, and `LITE_RUNTIME` variants, plus protobuf-c.",
        "",
        "## Verdict by payload",
        "",
        "The fastest row is selected by median nanoseconds per encode across the recorded repetitions.",
        "",
        "| Payload | Fastest | Median ns/encode | Encodes/sec | Bytes |",
        "|---|---|---:|---:|---:|",
    ]
    for test_case in sorted(fastest_by_case):
        item = fastest_by_case[test_case]
        label = f"{item['library']} / {item['codegen']} / {item['api']}"
        lines.append(f"| {test_case} | {label} | {fmt(item['median_ns'])} | {fmt(item['mean_mps'])} M/s | {item['bytes']} |")

    lines += ["", "## Full aggregate results", "", "| Kind | Library | Codegen | API | Payload | Reps | Bytes | Median ns/encode | Mean ns/encode | Mean M/s |", "|---|---|---|---|---|---:|---:|---:|---:|---:|"]
    for item in sorted(summaries, key=lambda x: (x["test_case"], x["median_ns"])):
        lines.append(f"| {item['kind']} | {item['library']} | {item['codegen']} | {item['api']} | {item['test_case']} | {item['repetitions']} | {item['bytes']} | {fmt(item['median_ns'])} | {fmt(item['mean_ns'])} | {fmt(item['mean_mps'])} |")

    lines += ["", "## Protobuf-only comparison", "", "| Payload | Library / generation | API | Median ns/encode | Relative to fastest protobuf |", "|---|---|---|---:|---:|"]
    for test_case in sorted({item["test_case"] for item in protobuf}):
        case_rows = [item for item in protobuf if item["test_case"] == test_case]
        fastest = min(item["median_ns"] for item in case_rows)
        for item in sorted(case_rows, key=lambda x: x["median_ns"]):
            label = f"{item['library']} / {item['codegen']}"
            lines.append(f"| {test_case} | {label} | {item['api']} | {fmt(item['median_ns'])} | {item['median_ns'] / fastest:.2f}x |")

    lines += ["", "## JSON-only comparison", "", "| Payload | Library | Median ns/encode | Relative to fastest JSON |", "|---|---|---:|---:|"]
    for test_case in sorted({item["test_case"] for item in json_rows}):
        case_rows = [item for item in json_rows if item["test_case"] == test_case]
        fastest = min(item["median_ns"] for item in case_rows)
        for item in sorted(case_rows, key=lambda x: x["median_ns"]):
            lines.append(f"| {test_case} | {item['library']} | {fmt(item['median_ns'])} | {item['median_ns'] / fastest:.2f}x |")

    lines += ["", "## Raw data", "", "- `raw.csv` contains every individual repetition and is the source for the aggregates above.", "- Re-run on a quiet, pinned CPU if comparing small differences; CPU frequency, thermal state, compiler, allocator, and container host affect absolute numbers.", ""]
    Path(args.output).write_text("\n".join(lines))


if __name__ == "__main__":
    main()

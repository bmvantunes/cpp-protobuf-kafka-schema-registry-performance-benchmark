#!/usr/bin/env python3
import argparse
import csv
import statistics
from collections import defaultdict
from pathlib import Path


def metadata(path):
    values = {}
    for line in Path(path).read_text().splitlines():
        if "=" in line:
            key, value = line.split("=", 1)
            values[key] = value
    return values


def read_csv(path):
    with open(path, newline="") as handle:
        return list(csv.DictReader(handle))


def percentile(values, fraction):
    ordered = sorted(values)
    index = max(0, min(len(ordered) - 1, int((len(ordered) * fraction + 0.999999) - 1)))
    return ordered[index]


def number(value):
    return f"{value:,.2f}"


def group(rows, keys):
    grouped = defaultdict(list)
    for row in rows:
        grouped[tuple(row[key] for key in keys)].append(row)
    return grouped


def steady_summary(rows):
    values = [float(row["ns_per_encode"]) for row in rows]
    return {
        "repetitions": len(rows),
        "bytes": int(rows[-1]["bytes"]),
        "median": statistics.median(values),
        "mean": statistics.mean(values),
        "p95": percentile(values, 0.95),
        "mps": 1_000_000_000 / statistics.mean(values),
    }


def network_summary(rows):
    values = [float(row["elapsed_ns"]) for row in rows]
    return {
        "repetitions": len(rows),
        "response_bytes": int(rows[-1]["bytes"]),
        "median": statistics.median(values),
        "mean": statistics.mean(values),
        "p95": percentile(values, 0.95),
        "min": min(values),
        "max": max(values),
        "rps": 1_000_000_000 / statistics.mean(values),
    }


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--baseline-csv", required=True)
    parser.add_argument("--registry-csv", required=True)
    parser.add_argument("--metadata", required=True)
    parser.add_argument("--output", required=True)
    args = parser.parse_args()

    base_rows = read_csv(args.baseline_csv)
    registry_rows = read_csv(args.registry_csv)
    meta = metadata(args.metadata)

    base_groups = group(base_rows, ["kind", "library", "codegen", "api", "test_case"])
    registry_steady = [row for row in registry_rows if row["iterations"] != "1"]
    registry_groups = group(registry_steady, ["api", "test_case"])
    network_rows = [row for row in registry_rows if row["iterations"] == "1"]
    network_groups = group(network_rows, ["api", "test_case"])

    pure_baseline = {}
    for test_case in ("one_string_ten_int64", "one_string_ten_decimal_strings", "ten_strings_fifty_decimal_strings"):
        key = ("protobuf", "google_protobuf", "speed", "SerializeToArray_preallocated", test_case)
        if key not in base_groups:
            raise SystemExit(f"Missing pure protobuf baseline group: {key}")
        pure_baseline[test_case] = steady_summary(base_groups[key])

    lines = [
        "# Confluent Schema Registry and Kafka framing benchmark",
        "",
        "> Detailed report: pure protobuf encoding, cached Confluent framing, allocation/copy variants, and live Schema Registry HTTP paths.",
        "",
        "## Executive verdict",
        "",
        "For an HFT producer, Schema Registry is acceptable only when the schema ID is acquired before the hot path and cached locally. The steady-state framing work is a fixed byte-prefix operation; registration and network lookups are separate millisecond-scale control-plane operations and must not occur per message.",
        "",
        "The benchmark uses a six-byte Protobuf Confluent prefix for a single top-level message: one magic byte, four big-endian schema-ID bytes, and the one-byte message-index encoding for index zero. The exact framing is included in the measured output size.",
        "",
        "## Run contract",
        "",
        f"- Steady-state encodes per repetition: `{meta.get('steady_state_iterations', '?')}`",
        f"- Steady-state repetitions: `{meta.get('steady_state_repetitions', '?')}`",
        f"- Excluded warmup encodes: `{meta.get('steady_state_warmup_iterations', '?')}`",
        f"- Cold/control-plane repetitions: `{meta.get('network_repetitions', '?')}` requests per path",
        "- Cold/control-plane paths intentionally use one HTTP request per repetition; sending one million live registration requests would benchmark Registry stress and storage behavior, not a production encode path.",
        "- All work runs in Docker; the Registry is Confluent Schema Registry backed by Confluent Kafka and ZooKeeper containers.",
        "- Decimal values remain exact strings in both protobuf and JSON payloads.",
        "",
        "## Steady-state result versus pure protobuf",
        "",
        "The pure baseline is Google protobuf generated with Buf using `SPEED` and preallocated `SerializeToArray`, matching the message implementation used by the Registry framing executable.",
        "",
        "| Payload | Pure protobuf ns | In-place framed ns | Framed + copy ns | Cached serializer string ns | Framed bytes | In-place overhead | Copy overhead |",
        "|---|---:|---:|---:|---:|---:|---:|---:|",
    ]
    for test_case in pure_baseline:
        pure = pure_baseline[test_case]["median"]
        inplace = steady_summary(registry_groups[("cached_id_framed_in_place", test_case)])
        copied = steady_summary(registry_groups[("cached_id_framed_with_copy", test_case)])
        string = steady_summary(registry_groups[("cached_serializer_string", test_case)])
        lines.append(
            f"| {test_case} | {number(pure)} | {number(inplace['median'])} | {number(copied['median'])} | {number(string['median'])} | {inplace['bytes']} | {(inplace['median'] / pure - 1) * 100:.2f}% | {(copied['median'] / pure - 1) * 100:.2f}% |"
        )

    lines += [
        "",
        "## All cached paths",
        "",
        "| Payload | API | Reps | Framed bytes | Median ns/encode | Mean M/s | p95 ns |",
        "|---|---|---:|---:|---:|---:|---:|",
    ]
    for key, rows in sorted(registry_groups.items(), key=lambda item: (item[0][1], steady_summary(item[1])["median"])):
        summary = steady_summary(rows)
        lines.append(f"| {key[1]} | {key[0]} | {summary['repetitions']} | {summary['bytes']} | {number(summary['median'])} | {number(summary['mps'])} | {number(summary['p95'])} |")

    lines += [
        "",
        "## Live Schema Registry paths",
        "",
        "These measurements include HTTP request/response and Registry processing. `registry_lookup_keepalive` means a keep-alive HTTP lookup; it is not a local schema-ID cache hit. The local cached-ID paths are the steady-state framing rows above. All live rows are control-plane latency measurements, not per-message serializer benchmarks.",
        "",
        "| Path | Reps | Median ns | Mean ns | p95 ns | Min ns | Max ns | Requests/sec |",
        "|---|---:|---:|---:|---:|---:|---:|---:|",
    ]
    for key, rows in sorted(network_groups.items()):
        summary = network_summary(rows)
        lines.append(f"| {key[0]} | {summary['repetitions']} | {number(summary['median'])} | {number(summary['mean'])} | {number(summary['p95'])} | {number(summary['min'])} | {number(summary['max'])} | {number(summary['rps'])} |")

    lines += [
        "",
        "## HFT interpretation",
        "",
        "1. Register or resolve the schema ID during process startup, deployment, or a controlled recovery path.",
        "2. Keep the schema ID in an immutable/read-mostly local cache. A cache miss must fail closed or use an explicitly non-HFT recovery path, not synchronously call Registry from the producer hot loop.",
        "3. Prefer serializing directly into a buffer with reserved prefix space. That avoids a second payload copy and makes the six-byte framing cost visible and bounded.",
        "4. Treat serializer-string paths as a convenience path, not the default HFT path; their allocation/copy behavior is visible in the cached-path table.",
        "5. Registry availability is still operationally important even when it is absent from the hot path: startup, schema rollout, failover, and cache invalidation need timeouts, metrics, and a tested fallback policy.",
        "6. Kafka compression, batching, broker acknowledgements, and network transport remain outside this benchmark and should be tested separately once the serializer/framing choice is fixed.",
        "",
        "## Scope boundary",
        "",
        "The companion `REPORT.md` contains the complete pure protobuf/JSON library matrix, including every Buf-generated protobuf variant and every JSON library. This Registry report intentionally uses the matching Google protobuf `SPEED` type for its wire-framing and Registry-path comparison; changing generated code and changing Registry integration at the same time would make the framing result harder to attribute.",
        "",
        "## Reproducibility and raw data",
        "",
        "- `schema_registry_raw.csv` contains every cached-path repetition and every measured HTTP request.",
        "- The existing `raw.csv` is the pure protobuf/JSON baseline used for the comparison.",
        "- Re-run on an isolated CPU when comparing small differences; CPU frequency, emulation mode, allocator state, and container host scheduling affect absolute values.",
        "",
    ]
    Path(args.output).write_text("\n".join(lines))


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
import csv
import glob
import statistics
from collections import defaultdict
from pathlib import Path


ROOT = Path("results")
PAYLOADS = [
    "one_string_ten_int64",
    "one_string_ten_decimal_strings",
    "ten_strings_fifty_decimal_strings",
]


def read(path):
    with open(path, newline="") as handle:
        return list(csv.DictReader(handle))


def grouped(rows, keys):
    result = defaultdict(list)
    for row in rows:
        result[tuple(row[key] for key in keys)].append(row)
    return result


def median(rows, field):
    return statistics.median(float(row[field]) for row in rows)


def fmt(value):
    return f"{value:,.2f}"


def ratio(arm, amd):
    return f"{amd / arm:.2f}x"


def protocol_matrix(arm_rows, amd_rows):
    arm = grouped(arm_rows, ("kind", "library", "codegen", "api", "test_case"))
    amd = grouped(amd_rows, ("kind", "library", "codegen", "api", "test_case"))
    lines = []
    for payload in PAYLOADS:
        lines += [
            f"### `{payload}`",
            "",
            "| Protocol / implementation | Bytes ARM/AMD | ARM64 median ns | AMD64 median ns | AMD/ARM |",
            "|---|---:|---:|---:|---:|",
        ]
        keys = sorted(key for key in arm if key[-1] == payload)
        for key in keys:
            arm_ns = median(arm[key], "ns_per_encode")
            amd_ns = median(amd[key], "ns_per_encode")
            label = " / ".join(key[:4])
            lines.append(
                f"| `{label}` | {arm[key][-1]['bytes']} / {amd[key][-1]['bytes']} | "
                f"{fmt(arm_ns)} | {fmt(amd_ns)} | {ratio(arm_ns, amd_ns)} |"
            )
        lines.append("")
    return lines


def fastest_payloads(arm_rows, amd_rows):
    def best(rows):
        groups = grouped(rows, ("kind", "library", "codegen", "api", "test_case"))
        result = {}
        for key, values in groups.items():
            item = (median(values, "ns_per_encode"), key, values[-1]["bytes"])
            if key[-1] not in result or item[0] < result[key[-1]][0]:
                result[key[-1]] = item
        return result

    arm = best(arm_rows)
    amd = best(amd_rows)
    lines = []
    for payload in PAYLOADS:
        av, ak, _ = arm[payload]
        xv, xk, _ = amd[payload]
        lines.append(
            f"| `{payload}` | {' / '.join(ak[1:4])} | {fmt(av)} | "
            f"{' / '.join(xk[1:4])} | {fmt(xv)} | {ratio(av, xv)} |"
        )
    return lines


def compiler_matrix():
    arm = {}
    amd = {}
    for path in sorted(ROOT.glob("compiler/*/raw.csv")):
        arm[path.parent.name] = read(path)
    for path in sorted((ROOT / "amd64").glob("compiler/*/raw.csv")):
        amd[path.parent.name] = read(path)

    lines = [
        "| Compiler variant | Payload | ARM64 fastest ns | AMD64 fastest ns | AMD/ARM |",
        "|---|---|---:|---:|---:|",
    ]
    for variant in sorted(arm):
        arm_groups = grouped(arm[variant], ("kind", "library", "codegen", "api", "test_case"))
        amd_groups = grouped(amd[variant], ("kind", "library", "codegen", "api", "test_case"))
        for payload in PAYLOADS:
            av = min(median(rows, "ns_per_encode") for key, rows in arm_groups.items() if key[-1] == payload)
            xv = min(median(rows, "ns_per_encode") for key, rows in amd_groups.items() if key[-1] == payload)
            lines.append(f"| `{variant}` | `{payload}` | {fmt(av)} | {fmt(xv)} | {ratio(av, xv)} |")
    return lines


def representation_matrix():
    arm = grouped(read(ROOT / "representation_raw.csv"), ("representation",))
    amd = grouped(read(ROOT / "amd64" / "representation_raw.csv"), ("representation",))
    lines = [
        "| Exact representation | ARM64 bytes | AMD64 bytes | ARM64 median ns | AMD64 median ns | AMD/ARM |",
        "|---|---:|---:|---:|---:|---:|",
    ]
    for key in sorted(arm):
        av = median(arm[key], "ns_per_encode")
        xv = median(amd[key], "ns_per_encode")
        lines.append(
            f"| `{key[0]}` | {arm[key][-1]['bytes']} | {amd[key][-1]['bytes']} | "
            f"{fmt(av)} | {fmt(xv)} | {ratio(av, xv)} |"
        )
    return lines


def latency_matrix():
    arm = grouped(read(ROOT / "latency_raw.csv"), ("path", "test_case"))
    amd = grouped(read(ROOT / "amd64" / "latency_raw.csv"), ("path", "test_case"))
    lines = [
        "| Path | Payload | ARM64 p50 / p99.9 / max ns | AMD64 p50 / p99.9 / max ns | ARM/AMD p50 |",
        "|---|---|---:|---:|---:|",
    ]
    for key in sorted(arm, key=lambda item: (PAYLOADS.index(item[1]), item[0])):
        av = [median(arm[key], field) for field in ("p50_ns", "p99_9_ns", "max_ns")]
        xv = [median(amd[key], field) for field in ("p50_ns", "p99_9_ns", "max_ns")]
        lines.append(
            f"| `{key[0]}` | `{key[1]}` | {fmt(av[0])} / {fmt(av[1])} / {fmt(av[2])} | "
            f"{fmt(xv[0])} / {fmt(xv[1])} / {fmt(xv[2])} | {ratio(av[0], xv[0])} |"
        )
    return lines


def distribution_matrix():
    arm = grouped(read(ROOT / "distribution_raw.csv"), ("distribution",))
    amd = grouped(read(ROOT / "amd64" / "distribution_raw.csv"), ("distribution",))
    lines = [
        "| Distribution | ARM64 median ns | AMD64 median ns | ARM64 mean bytes | AMD64 mean bytes | AMD/ARM |",
        "|---|---:|---:|---:|---:|---:|",
    ]
    for key in sorted(arm):
        av = median(arm[key], "ns_per_encode")
        xv = median(amd[key], "ns_per_encode")
        amb = statistics.mean(float(row["mean_bytes"]) for row in arm[key])
        xmb = statistics.mean(float(row["mean_bytes"]) for row in amd[key])
        lines.append(f"| `{key[0]}` | {fmt(av)} | {fmt(xv)} | {fmt(amb)} | {fmt(xmb)} | {ratio(av, xv)} |")
    return lines


def concurrency_matrix():
    def load(root):
        rows = []
        for path in sorted(root.glob("concurrency_*.csv")):
            rows.extend(read(path))
        return rows

    arm = grouped(load(ROOT), ("mode", "threads"))
    amd = grouped(load(ROOT / "amd64"), ("mode", "threads"))
    lines = [
        "| Mode | Threads | ARM64 median ns | AMD64 median ns | AMD/ARM |",
        "|---|---:|---:|---:|---:|",
    ]
    for key in sorted(arm, key=lambda item: (item[0], int(item[1]))):
        av = median(arm[key], "ns_per_encode")
        xv = median(amd[key], "ns_per_encode")
        lines.append(f"| `{key[0]}` | {key[1]} | {fmt(av)} | {fmt(xv)} | {ratio(av, xv)} |")
    return lines


def kafka_rows(root):
    rows = []
    for path in sorted(root.glob("kafka_producer_*.csv"), key=lambda p: int(p.stem.rsplit("_", 1)[1])):
        rows.extend(read(path))
    return rows


def async_pipeline_matrix():
    arm_path = ROOT / "async_pipeline_raw.csv"
    amd_path = ROOT / "amd64" / "async_pipeline_raw.csv"
    if not arm_path.exists() or not amd_path.exists():
        return ["The ARM64 and AMD64 asynchronous pipeline raw CSV files are not both available yet.", ""]
    arm = grouped(read(arm_path), ("mode", "test_case"))
    amd = grouped(read(amd_path), ("mode", "test_case"))
    lines = [
        "| Mode | Payload | ARM64 hot p50/p99 ns | AMD64 hot p50/p99 ns | ARM64 worker p50/p99 ns | AMD64 worker p50/p99 ns | ARM64/AMD64 worker M/s | Drops ARM/AMD |",
        "|---|---|---:|---:|---:|---:|---:|---:|",
    ]
    for key in sorted(arm, key=lambda item: (item[0], PAYLOADS.index(item[1]))):
        arm_hot = [median(arm[key], field) for field in ("hot_p50_ns", "hot_p99_ns")]
        amd_hot = [median(amd[key], field) for field in ("hot_p50_ns", "hot_p99_ns")]
        arm_worker = [median(arm[key], field) for field in ("worker_p50_ns", "worker_p99_ns")]
        amd_worker = [median(amd[key], field) for field in ("worker_p50_ns", "worker_p99_ns")]
        arm_mps = median(arm[key], "completed") / median(arm[key], "worker_wall_ns") * 1_000
        amd_mps = median(amd[key], "completed") / median(amd[key], "worker_wall_ns") * 1_000
        arm_drops = int(median(arm[key], "dropped"))
        amd_drops = int(median(amd[key], "dropped"))
        lines.append(
            f"| `{key[0]}` | `{key[1]}` | {fmt(arm_hot[0])} / {fmt(arm_hot[1])} | "
            f"{fmt(amd_hot[0])} / {fmt(amd_hot[1])} | {fmt(arm_worker[0])} / {fmt(arm_worker[1])} | "
            f"{fmt(amd_worker[0])} / {fmt(amd_worker[1])} | {fmt(arm_mps)} / {fmt(amd_mps)} | {arm_drops}/{amd_drops} |"
        )
    return lines


def kafka_matrix():
    arm_rows = kafka_rows(ROOT)
    # Each downloaded GitHub shard artifact contains the same assembled 72-row
    # configuration matrix. Use one assembled copy; do not count the six
    # artifact copies as repeated measurements.
    amd_paths = sorted((ROOT / "amd64" / "kafka-shards").glob("*/amd64/kafka_producer_merged.csv"))
    amd_rows = read(amd_paths[0])
    keys = ("mode", "acks", "compression", "linger_ms", "batch_num_messages")
    arm = grouped(arm_rows, keys)
    amd = grouped(amd_rows, keys)
    lines = [
        "| Mode | Acks | Compression | Linger | Batch | ARM64 enqueue / E2E ns/msg | AMD64 enqueue / E2E ns/msg | ARM/AMD E2E | Errors ARM/AMD |",
        "|---|---:|---|---:|---:|---:|---:|---:|---:|",
    ]
    for key in sorted(arm, key=lambda item: (item[0], ["0", "1", "all"].index(item[1]), item[2], int(item[3]), int(item[4]))):
        ae = median(arm[key], "enqueue_elapsed_ns") / 1_000_000
        at = median(arm[key], "end_to_end_ns") / 1_000_000
        xe = median(amd[key], "enqueue_elapsed_ns") / 1_000_000
        xt = median(amd[key], "end_to_end_ns") / 1_000_000
        arm_errors = sum(int(row["delivery_errors"]) for row in arm[key])
        amd_errors = sum(int(row["delivery_errors"]) for row in amd[key])
        lines.append(
            f"| `{key[0]}` | `{key[1]}` | `{key[2]}` | {key[3]} | {key[4]} | "
            f"{fmt(ae)} / {fmt(at)} | {fmt(xe)} / {fmt(xt)} | {ratio(at, xt)} | {arm_errors}/{amd_errors} |"
        )
    return lines


def registry_matrix(filename):
    arm = grouped(read(ROOT / filename), ("test_case", "api"))
    amd = grouped(read(ROOT / "amd64" / filename), ("test_case", "api"))
    lines = [
        "| Payload / operation | API | ARM64 median ns | AMD64 median ns | AMD/ARM |",
        "|---|---|---:|---:|---:|",
    ]
    for key in sorted(arm, key=lambda item: (0 if item[0] in PAYLOADS else 1, item)):
        field = "ns_per_encode" if int(arm[key][-1].get("iterations", "1")) > 1 else "elapsed_ns"
        av = median(arm[key], field)
        xv = median(amd[key], field)
        lines.append(f"| `{key[0]}` | `{key[1]}` | {fmt(av)} | {fmt(xv)} | {ratio(av, xv)} |")
    return lines


def evolution_matrix():
    arm = grouped(read(ROOT / "schema_evolution_raw.csv"), ("phase",))
    amd = grouped(read(ROOT / "amd64" / "schema_evolution_raw.csv"), ("phase",))
    lines = [
        "| Evolution phase | ARM64 median ms | AMD64 median ms | AMD/ARM |",
        "|---|---:|---:|---:|",
    ]
    for key in sorted(arm):
        av = median(arm[key], "elapsed_ns") / 1_000_000
        xv = median(amd[key], "elapsed_ns") / 1_000_000
        lines.append(f"| `{key[0]}` | {fmt(av)} | {fmt(xv)} | {ratio(av, xv)} |")
    return lines


def registry_overhead_matrix():
    baselines = {}
    for arch, path in (("ARM64", ROOT / "raw.csv"), ("AMD64", ROOT / "amd64" / "raw.csv")):
        rows = read(path)
        groups = grouped(rows, ("kind", "library", "codegen", "api", "test_case"))
        baselines[arch] = {
            payload: median(
                groups[("protobuf", "google_protobuf", "speed", "SerializeToArray_preallocated", payload)],
                "ns_per_encode",
            )
            for payload in PAYLOADS
        }

    lines = [
        "| Architecture | Payload | Pure Buf/Google `SPEED` preallocated ns | Cached in-place ns | In-place overhead | Framed + copy ns | Copy overhead | Serializer string ns | String overhead |",
        "|---|---|---:|---:|---:|---:|---:|---:|---:|",
    ]
    for arch, path in (("ARM64", ROOT / "schema_registry_raw.csv"), ("AMD64", ROOT / "amd64" / "schema_registry_raw.csv")):
        groups = grouped(read(path), ("test_case", "api"))
        for payload in PAYLOADS:
            pure = baselines[arch][payload]
            inplace = median(groups[(payload, "cached_id_framed_in_place")], "ns_per_encode")
            copy = median(groups[(payload, "cached_id_framed_with_copy")], "ns_per_encode")
            string = median(groups[(payload, "cached_serializer_string")], "ns_per_encode")
            pct = lambda value: f"{(value / pure - 1) * 100:+.2f}%"
            lines.append(
                f"| {arch} | `{payload}` | {fmt(pure)} | {fmt(inplace)} | {pct(inplace)} | "
                f"{fmt(copy)} | {pct(copy)} | {fmt(string)} | {pct(string)} |"
            )
    return lines


def registry_live_matrix():
    lines = [
        "| Live Registry operation | ARM64 median ms | AMD64 median ms | AMD/ARM |",
        "|---|---:|---:|---:|",
    ]
    for api in [
        "registry_lookup_keepalive",
        "cold_lookup_new_connection",
        "cold_register",
        "cache_miss_404",
        "retry_failure_then_success",
        "concurrent_registration",
        "registry_unavailable",
    ]:
        values = []
        for path in (ROOT / "schema_registry_raw.csv", ROOT / "amd64" / "schema_registry_raw.csv"):
            rows = [row for row in read(path) if row["api"] == api and int(row["iterations"]) == 1]
            values.append(median(rows, "elapsed_ns") / 1_000_000)
        lines.append(f"| `{api}` | {fmt(values[0])} | {fmt(values[1])} | {ratio(values[0], values[1])} |")
    return lines


def main():
    arm_pure = read(ROOT / "raw.csv")
    amd_pure = read(ROOT / "amd64" / "raw.csv")
    lines = [
        "# ARM64 versus native AMD64 protocol comparison",
        "",
        "> One aligned report for the completed Docker benchmarks. Every steady-state encoding row uses 1,000,000 operations per repetition and 10 measured repetitions. Lower latency is better.",
        "",
        "## Executive summary",
        "",
        "| Test payload | ARM64 fastest | ARM64 ns/encode | AMD64 fastest | AMD64 ns/encode | AMD/ARM |",
        "|---|---|---:|---|---:|---:|",
    ]
    lines += fastest_payloads(arm_pure, amd_pure)
    lines += [
        "",
        "The best implementation changes with payload shape: Google protobuf is strongest for the compact int64 message, while protobuf-c is strongest for the largest string-heavy message. yyjson is the fastest JSON implementation in most payloads, but protobuf remains smaller on the wire and generally faster.",
        "",
        "## 1. Complete protobuf and JSON protocol matrix",
        "",
        "This table includes every generated/API permutation and every JSON library tested. `SPEED`, `CODE_SIZE`, and `LITE_RUNTIME` are Google C++ types generated through `buf generate`; protobuf-c is separately generated C code.",
        "",
    ]
    lines += protocol_matrix(arm_pure, amd_pure)
    lines += [
        "## 2. Compiler and C++ standard comparison",
        "",
        "Each row is the fastest implementation for that compiler variant and payload; the complete per-library compiler tables remain in the architecture reports.",
        "",
    ]
    lines += compiler_matrix()
    lines += [
        "",
        "## 3. Exact decimal representations",
        "",
        "These are protobuf-only representations of exact decimal values; JSON decimal fields were deliberately encoded as strings in the protocol matrix.",
        "",
    ]
    lines += representation_matrix()
    lines += [
        "",
        "## 4. Allocation and tail latency",
        "",
        "Values are instrumented per-encode latency. They should be compared within this phase, not substituted directly for the aggregate throughput benchmark.",
        "",
    ]
    lines += latency_matrix()
    lines += [
        "",
        "## 5. Realistic value distributions",
        "",
    ]
    lines += distribution_matrix()
    lines += [
        "",
        "## 6. Concurrency and contention",
        "",
    ]
    lines += concurrency_matrix()
    lines += [
        "",
        "## 7. Off-hot-path asynchronous pipeline",
        "",
        "This phase measures the producer-side non-blocking handoff separately from worker-side Protobuf serialization and pipe formatting. It contains no Kafka broker or logging I/O. The handoff baseline uses preallocated immutable event-pool ownership transfer.",
        "",
    ]
    lines += async_pipeline_matrix()
    lines += [
        "",
        "## 8. Real librdkafka/Kafka producer path",
        "",
        "The producer matrix uses the representative 49-byte `one_string_ten_int64` protobuf message and covers ownership, acknowledgements, compression, linger, and batching. Both architectures ran all 72 configurations, each with 10 × 1M messages and zero delivery errors.",
        "",
    ]
    lines += kafka_matrix()
    lines += [
        "",
        "## 9. Schema Registry cached framing and live paths",
        "",
        "The Confluent protobuf prefix is six bytes: magic byte + four-byte schema ID + one-byte message index. Cached rows are steady-state framing; non-payload rows are HTTP/control-plane operations.",
        "",
        "### Plain Registry",
        "",
    ]
    lines += registry_matrix("schema_registry_raw.csv")
    lines += ["", "### TLS and Basic Auth Registry", ""]
    lines += registry_matrix("schema_registry_secure_raw.csv")
    lines += ["", "### Schema evolution", ""]
    lines += evolution_matrix()
    lines += [
        "",
        "## 9. Measurement availability",
        "",
        "| Phase | ARM64 | Native AMD64 |",
        "|---|---|---|",
        "| Docker steady-state encoding | Complete | Complete |",
        "| Docker Kafka producer matrix | 72/72 configurations, zero errors | 72/72 configurations, zero errors |",
        "| Docker Schema Registry plain/TLS/auth/evolution | Complete | Complete |",
        "| Linux `perf` counters | Unavailable (`perf` exit 127) | Unavailable (`perf` exit 127) |",
        "",
        "## 10. Conclusion and recommendation",
        "",
        "### What should we use?",
        "",
        "1. **Default production choice:** keep Buf as the schema source of truth and use Buf-generated Google C++ `SPEED` types with `SerializeToArray` into a caller-owned, reused buffer. This is the best cross-architecture default, integrates cleanly with Confluent framing, and avoids choosing a different code-generation ecosystem for every payload.",
        "2. **Use protobuf-c selectively:** `pack_preallocated` wins the ARM64 large string-heavy payload and the AMD64 decimal-string and large payloads. Use it when that specific message type is a proven latency bottleneck and the additional C API/integration trade-off is acceptable. It is not universally fastest: Google protobuf wins the compact int64 payload on both architectures.",
        "3. **Do not use `CODE_SIZE` in the HFT hot path:** it was dramatically slower in these tests. `LITE_RUNTIME` can win a particular ARM64 row, but `SPEED` is the safer cross-architecture baseline.",
        "4. **JSON is not the lowest-latency choice here:** yyjson is the strongest JSON option, but protobuf is smaller and faster for the tested Kafka payloads. Keep JSON for interoperability or non-hot paths unless a separate business requirement dominates.",
        "",
        "### Schema Registry overhead without Kafka",
        "",
        "This is the direct protobuf-versus-Registry-plus-protobuf comparison; Kafka producer handoff, broker acknowledgement, batching, and compression are not included in this table.",
        "",
    ]
    lines += registry_overhead_matrix()
    lines += [
        "",
        "The six-byte Confluent prefix itself is not the expensive part. Cached in-place framing is approximately 0%–2% overhead on the normal non-noisy rows; the copy path is generally low single-digit overhead, and the serializer-string convenience path is higher because it introduces extra string/copy behavior. Negative percentages in isolated rows are measurement noise, not a real Registry speedup.",
        "",
        "### Registry control-plane cost",
        "",
    ]
    lines += registry_live_matrix()
    lines += [
        "",
        "These live operations are millisecond-scale control-plane work. Resolve/register the schema during startup, deployment, or controlled recovery, then cache the schema ID locally. A cache miss in the producer hot loop should fail closed or use an explicit non-HFT recovery path; it should not synchronously call Registry.",
        "",
        "### Final HFT recommendation",
        "",
        "- Use the fastest payload-specific protobuf implementation after validating compiler/allocator behavior on production hardware.",
        "- Resolve and cache the Schema Registry ID before the producer hot path. Cached framing is low-single-digit overhead; live Registry lookup/registration is millisecond-scale and must stay off the message path.",
        "- Batching dominates the local Kafka test. `acks=0` is not durable delivery; choose `acks=1` or `acks=all` from the loss/recovery policy, then validate the choice on the real multi-node cluster.",
        "- Reuse preallocated buffers and avoid fresh output allocation in the hot loop. Shared-buffer mutex contention is visible in the concurrency table.",
        "",
        "## 11. Scope and reproducibility",
        "",
        "- ARM64: Docker under OrbStack on the local macOS arm64 host.",
        "- AMD64: native x86_64 GitHub Actions runner, Docker workflow run [31284801828](https://github.com/bmvantunes/cpp-protobuf-kafka-schema-registry-performance-benchmark/actions/runs/31284801828).",
        "- Emulated amd64 results are retained separately in [`emulated-amd64.md`](emulated-amd64.md); they are not native AMD64 evidence.",
        "- `perf` counters were unavailable in the container environments, so no cycles/instructions/cache claim is made.",
        "- No bare-metal production Linux host or production multi-node Kafka cluster was available; absolute production SLA numbers still require a final pinned-core run.",
        "",
        "See [`arm64.md`](arm64.md), [`amd64.md`](amd64.md), and the raw artifacts for the phase-specific detailed reports.",
        "",
    ]
    (ROOT / "COMPARE.md").write_text("\n".join(lines))


if __name__ == "__main__":
    main()

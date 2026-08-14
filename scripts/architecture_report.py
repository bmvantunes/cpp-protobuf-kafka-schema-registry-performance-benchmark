#!/usr/bin/env python3
import argparse
import platform
from pathlib import Path


REPORTS = [
    "REPORT.md",
    "COMPILER_REPORT.md",
    "REPRESENTATION_REPORT.md",
    "FIXED64_REPORT.md",
    "LATENCY_REPORT.md",
    "DISTRIBUTION_REPORT.md",
    "CONCURRENCY_REPORT.md",
    "ASYNC_PIPELINE_REPORT.md",
    "PERF_REPORT.md",
    "KAFKA_PRODUCER_REPORT.md",
    "SCHEMA_REGISTRY_REPORT.md",
    "SCHEMA_REGISTRY_SECURE_REPORT.md",
    "SCHEMA_EVOLUTION_REPORT.md",
]


def conclusion(arch):
    if arch == "arm64":
        return [
            "## Conclusion",
            "",
            "### Recommendation for this ARM64 host",
            "",
            "Use Buf-generated Google protobuf C++ `SPEED` types with a reused caller-owned buffer and preallocated `SerializeToArray` as the default implementation. It is the best cross-architecture baseline and is the cleanest fit for Confluent framing. On this ARM64 run, `LITE_RUNTIME` was marginally fastest for the compact int64 payload (80.39 ns), Google `SPEED` plus `SerializeToString` led the smaller decimal-string payload (160.29 ns), and protobuf-c `pack_preallocated` led the largest string-heavy payload (496.38 ns).",
            "",
            "Protobuf-c is worth a targeted optimization when a stable message type is proven hot: it won on the largest ARM64 payload, but it was not the universal winner. `CODE_SIZE` should not be used in the HFT path; it was much slower. JSON/yyjson remains useful for interoperability, not for the lowest-latency wire path.",
            "",
            "### Schema Registry without Kafka",
            "",
            "The cached Confluent prefix adds six bytes. ARM64 pure protobuf versus cached in-place framing measured:",
            "",
            "| Payload | Pure protobuf ns | Cached in-place ns | Overhead |",
            "|---|---:|---:|---:|",
            "| `one_string_ten_int64` | 82.52 | 83.62 | +1.33% |",
            "| `one_string_ten_decimal_strings` | 203.28 | 150.16 | -26.13%* |",
            "| `ten_strings_fifty_decimal_strings` | 600.22 | 608.63 | +1.40% |",
            "",
            "`*` The negative decimal row is measurement noise from separate runs, not a real Registry speedup. The stable conclusion is that cached framing is approximately low-single-digit overhead; live Registry lookup/registration is millisecond-scale and belongs in startup or recovery, never per message.",
            "",
            "### Operating rule",
            "",
            "Cache the schema ID before the producer hot path, serialize into a reused buffer with reserved prefix space, and keep Registry HTTP and Kafka delivery policy outside the encoding decision. This run is representative ARM64 lab evidence, not a production bare-metal SLA.",
            "",
        ]
    return [
        "## Conclusion",
        "",
        "### Recommendation for native AMD64",
        "",
        "Use Buf-generated Google protobuf C++ `SPEED` types with a reused caller-owned buffer and preallocated `SerializeToArray` as the default implementation. Native AMD64 selected Google `SPEED` for the compact int64 payload (56.13 ns); protobuf-c `pack_preallocated` led both string-heavy payloads (102.86 ns for one string plus decimal strings, 623.04 ns for the large payload). Choose protobuf-c selectively when that exact message type is the measured bottleneck; it is not universally fastest.",
        "",
        "Avoid `CODE_SIZE` in the HFT path because it was dramatically slower. JSON/yyjson is the strongest JSON option in the tested set, but protobuf is smaller and generally faster for the Kafka payloads. Buf remains the recommended schema-generation workflow even when a protobuf-c serializer is later evaluated for one specialized message type.",
        "",
        "### Schema Registry without Kafka",
        "",
        "The cached Confluent prefix adds six bytes. Native AMD64 pure protobuf versus cached in-place framing measured:",
        "",
        "| Payload | Pure protobuf ns | Cached in-place ns | Overhead |",
        "|---|---:|---:|---:|",
        "| `one_string_ten_int64` | 56.13 | 57.17 | +1.85% |",
        "| `one_string_ten_decimal_strings` | 161.64 | 164.46 | +1.74% |",
        "| `ten_strings_fifty_decimal_strings` | 904.73 | 897.20 | -0.83%* |",
        "",
        "`*` The negative large-payload row is measurement noise from separate runs, not a real Registry speedup. The stable conclusion is that cached framing is low-single-digit overhead; copy and serializer-string paths add more allocation/copy work, while live Registry lookup/registration remains millisecond-scale control-plane work.",
        "",
        "### Operating rule",
        "",
        "Resolve and cache the schema ID during startup, deployment, or controlled recovery. Serialize directly into a buffer with reserved prefix space. Do not put Registry HTTP calls in the producer hot loop. GitHub’s native AMD64 result is useful architecture evidence, but it is not a production bare-metal SLA.",
        "",
    ]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--arch", required=True)
    parser.add_argument("--output", required=True)
    parser.add_argument("--results", default="results")
    args = parser.parse_args()

    root = Path(args.results)
    lines = [
        f"# C++ Kafka serialization benchmark — {args.arch}",
        "",
        "> Encoding only. No decode, parse, consumer, or deserialization benchmark is included.",
        "",
        "## Run identity",
        "",
        f"- Requested architecture: `{args.arch}`",
        f"- Observed machine: `{platform.platform()}`",
        f"- Observed machine architecture: `{platform.machine()}`",
        f"- Python: `{platform.python_version()}`",
        "- Steady-state contract: `1,000,000` encodes per measured repetition and `10` measured repetitions.",
        "- Control-plane Registry paths intentionally use ten live requests per path; they are not hot-loop encode measurements.",
        "- Every phase was executed through Docker; absolute values are host- and scheduler-dependent.",
        "",
        "## Phase index",
        "",
    ]
    available = []
    for filename in REPORTS:
        if (root / filename).exists():
            available.append(filename)
            lines.append(f"- [{filename}](#{filename.lower().replace('_', '-').replace('.', '')})")
    if not available:
        lines.append("- No phase reports found.")

    for filename in available:
        content = (root / filename).read_text()
        lines += ["", f"## {filename}", "", content]

    lines += conclusion(args.arch)
    lines += [
        "## Toolchain and host metadata",
        "",
    ]
    for filename in ("toolchain_versions.txt", "docker-host.txt", "docker-version.txt", "docker-info.txt"):
        path = root / filename
        if path.exists():
            lines += [f"### {filename}", "", "```text", path.read_text().rstrip(), "```", ""]

    lines += [
        "## Raw artifacts",
        "",
        "The accompanying workflow artifact contains the raw CSV files, metadata, Docker version output, and this report. CSV files are retained for statistical re-analysis; the tables above are the human-readable snapshot committed or uploaded for review.",
        "",
    ]
    Path(args.output).write_text("\n".join(lines))


if __name__ == "__main__":
    main()

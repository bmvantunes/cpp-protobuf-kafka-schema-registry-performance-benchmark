#!/usr/bin/env python3
import argparse
import platform
from pathlib import Path


REPORTS = [
    "REPORT.md",
    "REPRESENTATION_REPORT.md",
    "LATENCY_REPORT.md",
    "DISTRIBUTION_REPORT.md",
    "CONCURRENCY_REPORT.md",
    "PERF_REPORT.md",
    "KAFKA_PRODUCER_REPORT.md",
    "SCHEMA_REGISTRY_REPORT.md",
    "SCHEMA_REGISTRY_SECURE_REPORT.md",
    "SCHEMA_EVOLUTION_REPORT.md",
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

    lines += [
        "",
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

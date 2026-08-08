#!/usr/bin/env python3
import argparse
import csv
import statistics
from pathlib import Path


def metadata(path):
    result = {}
    for line in path.read_text().splitlines():
        if "=" in line:
            key, value = line.split("=", 1)
            result[key] = value
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", required=True)
    parser.add_argument("--output", required=True)
    args = parser.parse_args()
    root = Path(args.root)
    variants = []
    summaries = []

    for raw in sorted(root.glob("*/raw.csv")):
        variant_dir = raw.parent
        info = metadata(variant_dir / "metadata.txt")
        variants.append(info)
        with raw.open(newline="") as handle:
            rows = list(csv.DictReader(handle))
        groups = {}
        for row in rows:
            key = (row["kind"], row["library"], row["codegen"], row["api"], row["test_case"])
            groups.setdefault(key, []).append(float(row["ns_per_encode"]))
        for key, values in groups.items():
            summaries.append({
                "variant": info.get("variant", variant_dir.name),
                "compiler": info.get("compiler_name", "unknown"),
                "standard": info.get("cxx_standard", "unknown"),
                "kind": key[0], "library": key[1], "codegen": key[2], "api": key[3],
                "test_case": key[4], "reps": len(values),
                "median": statistics.median(values), "mean": statistics.mean(values),
            })

    if not variants:
        raise SystemExit("no compiler result directories found")

    lines = [
        "# Compiler and C++ standard comparison",
        "",
        "> Encoding only. No decoding, parsing, Schema Registry, or Kafka network work is included.",
        "",
        "## Contract",
        "",
        "- Every successful variant uses 1,000,000 encodes per repetition and 10 measured repetitions.",
        "- All variants use the same Docker image, Buf-generated sources, `-O3 -march=native -DNDEBUG`, and pre-populated messages.",
        "- C++23 is the established baseline; C++26 is included where the compiler accepts the standard mode.",
        "",
        "## Variants",
        "",
        "| Variant | Compiler | Standard | Architecture | Repetitions |",
        "|---|---|---:|---|---:|",
    ]
    for info in sorted(variants, key=lambda x: x.get("variant", "")):
        lines.append(f"| {info.get('variant','?')} | {info.get('compiler_name','?')} ({info.get('compiler_version','?')}) | C++{info.get('cxx_standard','?')} | {info.get('architecture','?')} | {info.get('repetitions','?')} |")

    lines += ["", "## Fastest row per payload and compiler", "", "| Variant | Payload | Fastest implementation | Median ns/encode | Mean M/s |", "|---|---|---|---:|---:|"]
    for variant in sorted({row["variant"] for row in summaries}):
        for case in sorted({row["test_case"] for row in summaries if row["variant"] == variant}):
            candidates = [row for row in summaries if row["variant"] == variant and row["test_case"] == case]
            best = min(candidates, key=lambda row: row["median"])
            mps = 1000.0 / best["mean"]
            label = f"{best['kind']} / {best['library']} / {best['codegen']} / {best['api']}"
            lines.append(f"| {variant} | {case} | {label} | {best['median']:,.2f} | {mps:,.2f} |")

    lines += ["", "## Full aggregate results", "", "| Variant | Kind | Library | Codegen | API | Payload | Reps | Median ns/encode | Mean ns/encode |", "|---|---|---|---|---|---|---:|---:|---:|"]
    for row in sorted(summaries, key=lambda row: (row["test_case"], row["variant"], row["median"])):
        lines.append(f"| {row['variant']} | {row['kind']} | {row['library']} | {row['codegen']} | {row['api']} | {row['test_case']} | {row['reps']} | {row['median']:,.2f} | {row['mean']:,.2f} |")

    lines += ["", "## Interpretation", "", "- Compare compiler rows only within the same host architecture and Docker host; compiler and standard effects are smaller than CPU, frequency, allocator, and scheduler variance in many rows.", "- A successful C++26 row proves the selected compiler accepted that language mode; it does not imply all generated dependencies have adopted every C++26 feature.", "- Raw per-repetition data remains in `results/compiler/<variant>/raw.csv`.", ""]
    Path(args.output).write_text("\n".join(lines))


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
import argparse
import csv
import statistics
from pathlib import Path


def median(rows, field):
    return statistics.median(float(row[field]) for row in rows)


def fmt(value):
    return f"{value:,.2f}"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--csv", required=True)
    parser.add_argument("--metadata", required=True)
    parser.add_argument("--output", required=True)
    args = parser.parse_args()
    rows = list(csv.DictReader(Path(args.csv).open(newline="")))
    groups = {}
    for row in rows:
        groups.setdefault((row["test_case"], row["representation"]), []).append(row)
    lines = [
        "# `int64` versus `fixed64` encoding benchmark",
        "",
        "> Identical positive values, identical field count, preallocated `SerializeToArray`, encoding only. No decoding or Kafka work is included.",
        "",
        "## Run contract",
        "",
        "- Every row: `1,000,000` encodes per repetition × `10` measured repetitions.",
        "- The only changed variable is the generated field wire type: signed `int64` versus unsigned `fixed64`.",
        "- The `fixed64` schema is valid for non-negative values. Use `sfixed64` if signed semantics are required.",
        "",
        "## Results",
        "",
        "| Value range | Representation | Bytes | Median ns/encode | Mean ns/encode | Mean M/s | Fixed64 speed versus int64 |",
        "|---|---|---:|---:|---:|---:|---:|",
    ]
    for test_case in sorted({key[0] for key in groups}):
        int_rows = groups[(test_case, "int64")]
        fixed_rows = groups[(test_case, "fixed64")]
        int_ns = median(int_rows, "ns_per_encode")
        for representation, values in (("int64", int_rows), ("fixed64", fixed_rows)):
            ns = median(values, "ns_per_encode")
            lines.append(f"| {test_case} | {representation} | {values[0]['bytes']} | {fmt(ns)} | {fmt(statistics.mean(float(row['ns_per_encode']) for row in values))} | {fmt(1_000_000_000 / ns / 1_000_000)} | {fmt(int_ns / ns)}x |")
    lines += [
        "",
        "## Verdict",
        "",
        "`fixed64` always uses eight payload bytes per numeric field, so its wire size is stable. `int64` uses a variable-length varint: small positive values are smaller, while large positive values approach the fixed-width size. The benchmark result, not intuition, determines whether the fixed-width serializer wins on the target CPU and generated code.",
        "",
        "Do not choose `fixed64` solely because it avoids varint branching. It can be faster for large values, but it can also produce larger Kafka records. For prices, quantities, and other non-negative fixed-scale values, compare the exact production distribution and include broker/network bandwidth in the final decision.",
        "",
    ]
    Path(args.output).write_text("\n".join(lines))


if __name__ == "__main__":
    main()

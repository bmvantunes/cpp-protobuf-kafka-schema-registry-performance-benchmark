#!/usr/bin/env python3
import argparse
import csv
from pathlib import Path


EXPECTED_ITERATIONS = 1_000_000
EXPECTED_REPETITIONS = 10


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", required=True)
    parser.add_argument("--csv", required=True)
    parser.add_argument("--metadata", required=True)
    args = parser.parse_args()

    root = Path(args.root)
    files = sorted(root.rglob("kafka_producer_*.csv"))
    if not files:
        raise SystemExit(f"no Kafka producer CSV files found under {root}")

    rows = []
    fieldnames = None
    for path in files:
        with path.open(newline="") as handle:
            reader = csv.DictReader(handle)
            if fieldnames is None:
                fieldnames = reader.fieldnames
            elif reader.fieldnames != fieldnames:
                raise SystemExit(f"CSV header mismatch in {path}")
            file_rows = list(reader)
            if len(file_rows) != EXPECTED_REPETITIONS:
                raise SystemExit(
                    f"{path}: expected {EXPECTED_REPETITIONS} rows, "
                    f"found {len(file_rows)}"
                )
            repetitions = []
            for row in file_rows:
                if int(row["iterations"]) != EXPECTED_ITERATIONS:
                    raise SystemExit(
                        f"{path}: expected {EXPECTED_ITERATIONS} iterations, "
                        f"found {row['iterations']}"
                    )
                repetitions.append(int(row["repetition"]))
            if repetitions != list(range(1, EXPECTED_REPETITIONS + 1)):
                raise SystemExit(
                    f"{path}: expected repetitions 1..{EXPECTED_REPETITIONS}, "
                    f"found {repetitions}"
                )
            rows.extend(file_rows)

    with Path(args.csv).open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)

    metadata = {
        "runs": str(len(files)),
        "iterations": "1000000",
        "repetitions": "10",
        "shards": str(len({path.parent for path in files})),
        "encoding_plus_kafka_produce": "true",
        "decode_benchmark": "false",
    }
    Path(args.metadata).write_text("".join(f"{key}={value}\n" for key, value in metadata.items()))


if __name__ == "__main__":
    main()

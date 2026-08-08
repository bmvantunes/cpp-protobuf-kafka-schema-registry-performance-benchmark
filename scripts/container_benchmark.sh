#!/usr/bin/env bash
set -euo pipefail

cd /work

: "${ITERATIONS:=1000000}"
: "${REPETITIONS:=10}"
: "${WARMUP_ITERATIONS:=10000}"
: "${RESULTS_DIR:=/work/results}"

if [[ "${ITERATIONS}" -lt 1000000 || "${REPETITIONS}" -lt 10 ]]; then
  echo "Refusing an under-sized run: ITERATIONS >= 1000000 and REPETITIONS >= 10 are required." >&2
  exit 2
fi

rm -rf generated build
mkdir -p "${RESULTS_DIR}"

echo "Generating C++ and protobuf-c types with buf ${BUF_VERSION:-pinned-image-version}..."
buf generate proto --template buf.gen.google_speed.yaml
buf generate proto --template buf.gen.google_codesize.yaml
buf generate proto --template buf.gen.google_lite.yaml
buf generate proto --template buf.gen.protobuf_c.yaml

echo "Building release benchmark..."
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

echo "Running ${ITERATIONS} encodes x ${REPETITIONS} repetitions per benchmark..."
build/encode_benchmark \
  --iterations "${ITERATIONS}" \
  --repetitions "${REPETITIONS}" \
  --warmup-iterations "${WARMUP_ITERATIONS}" \
  --csv "${RESULTS_DIR}/raw.csv" \
  --metadata "${RESULTS_DIR}/metadata.txt"

python3 scripts/report.py \
  --csv "${RESULTS_DIR}/raw.csv" \
  --metadata "${RESULTS_DIR}/metadata.txt" \
  --output "${RESULTS_DIR}/REPORT.md"

echo "Report written to ${RESULTS_DIR}/REPORT.md"

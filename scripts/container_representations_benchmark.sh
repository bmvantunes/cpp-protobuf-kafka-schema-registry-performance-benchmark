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
buf generate proto --template buf.gen.google_speed.yaml
buf generate proto --template buf.gen.google_codesize.yaml
buf generate proto --template buf.gen.google_lite.yaml
buf generate proto --template buf.gen.protobuf_c.yaml
buf generate proto --template buf.gen.representations.yaml
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel --target representation_benchmark
build/representation_benchmark \
  --iterations "${ITERATIONS}" \
  --repetitions "${REPETITIONS}" \
  --warmup-iterations "${WARMUP_ITERATIONS}" \
  --csv "${RESULTS_DIR}/representation_raw.csv" \
  --metadata "${RESULTS_DIR}/representation_metadata.txt"
python3 scripts/representation_report.py \
  --csv "${RESULTS_DIR}/representation_raw.csv" \
  --metadata "${RESULTS_DIR}/representation_metadata.txt" \
  --output "${RESULTS_DIR}/REPRESENTATION_REPORT.md"
echo "Representation report written to ${RESULTS_DIR}/REPRESENTATION_REPORT.md"

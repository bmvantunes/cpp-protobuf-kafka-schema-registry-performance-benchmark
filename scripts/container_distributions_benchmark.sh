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
for t in buf.gen.google_speed.yaml buf.gen.google_codesize.yaml buf.gen.google_lite.yaml buf.gen.protobuf_c.yaml buf.gen.representations.yaml; do
  buf generate proto --template "$t"
done
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel --target distribution_benchmark
build/distribution_benchmark --iterations "${ITERATIONS}" --repetitions "${REPETITIONS}" --warmup-iterations "${WARMUP_ITERATIONS}" \
  --csv "${RESULTS_DIR}/distribution_raw.csv" --metadata "${RESULTS_DIR}/distribution_metadata.txt"
python3 scripts/distribution_report.py --csv "${RESULTS_DIR}/distribution_raw.csv" --metadata "${RESULTS_DIR}/distribution_metadata.txt" --output "${RESULTS_DIR}/DISTRIBUTION_REPORT.md"
echo "Distribution report written to ${RESULTS_DIR}/DISTRIBUTION_REPORT.md"

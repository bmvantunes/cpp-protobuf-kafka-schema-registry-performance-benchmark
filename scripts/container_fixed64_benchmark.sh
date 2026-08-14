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
for template in buf.gen.google_speed.yaml buf.gen.google_codesize.yaml buf.gen.google_lite.yaml buf.gen.protobuf_c.yaml buf.gen.representations.yaml; do
  buf generate proto --template "${template}"
done
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel --target fixed64_benchmark
rm -f "${RESULTS_DIR}/fixed64_raw.csv" "${RESULTS_DIR}/fixed64_metadata.txt"
build/fixed64_benchmark --iterations "${ITERATIONS}" --repetitions "${REPETITIONS}" \
  --warmup-iterations "${WARMUP_ITERATIONS}" --csv "${RESULTS_DIR}/fixed64_raw.csv" \
  --metadata "${RESULTS_DIR}/fixed64_metadata.txt"
python3 scripts/fixed64_report.py --csv "${RESULTS_DIR}/fixed64_raw.csv" \
  --metadata "${RESULTS_DIR}/fixed64_metadata.txt" --output "${RESULTS_DIR}/FIXED64_REPORT.md"
echo "fixed64 report written to ${RESULTS_DIR}/FIXED64_REPORT.md"

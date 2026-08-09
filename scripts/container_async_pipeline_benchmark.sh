#!/usr/bin/env bash
set -euo pipefail
cd /work
: "${ITERATIONS:=1000000}"
: "${REPETITIONS:=10}"
: "${WARMUP_ITERATIONS:=10000}"
: "${MODES:=sync_both,async_handoff,async_protobuf,async_pipe,async_both}"
: "${RESULTS_DIR:=/work/results}"
if [[ "${ITERATIONS}" -lt 1000000 || "${REPETITIONS}" -lt 10 ]]; then
  echo "Refusing an under-sized run: ITERATIONS >= 1000000 and REPETITIONS >= 10 are required." >&2
  exit 2
fi
rm -rf generated build
mkdir -p "${RESULTS_DIR}"
buf generate proto --template buf.gen.google_speed.yaml
for template in buf.gen.google_codesize.yaml buf.gen.google_lite.yaml buf.gen.protobuf_c.yaml buf.gen.representations.yaml; do
  buf generate proto --template "${template}"
done
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel --target async_pipeline_benchmark
rm -f "${RESULTS_DIR}/async_pipeline_raw.csv" "${RESULTS_DIR}/async_pipeline_metadata.txt"
build/async_pipeline_benchmark \
  --iterations "${ITERATIONS}" \
  --repetitions "${REPETITIONS}" \
  --warmup-iterations "${WARMUP_ITERATIONS}" \
  --modes "${MODES}" \
  --csv "${RESULTS_DIR}/async_pipeline_raw.csv" \
  --metadata "${RESULTS_DIR}/async_pipeline_metadata.txt"
python3 scripts/async_pipeline_report.py \
  --csv "${RESULTS_DIR}/async_pipeline_raw.csv" \
  --metadata "${RESULTS_DIR}/async_pipeline_metadata.txt" \
  --output "${RESULTS_DIR}/ASYNC_PIPELINE_REPORT.md"
echo "Async pipeline report written to ${RESULTS_DIR}/ASYNC_PIPELINE_REPORT.md"

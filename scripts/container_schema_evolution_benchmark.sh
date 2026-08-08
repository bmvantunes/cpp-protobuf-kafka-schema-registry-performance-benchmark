#!/usr/bin/env bash
set -euo pipefail
cd /work
: "${REPETITIONS:=10}"
: "${REGISTRY_URL:=http://schema-registry:8081}"
: "${RESULTS_DIR:=/work/results}"
if [[ "${REPETITIONS}" -lt 10 ]]; then
  echo "REPETITIONS must be >= 10" >&2
  exit 2
fi
rm -rf generated build
for t in buf.gen.google_speed.yaml buf.gen.google_codesize.yaml buf.gen.google_lite.yaml buf.gen.protobuf_c.yaml buf.gen.representations.yaml; do
  buf generate proto --template "$t"
done
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel --target schema_evolution_benchmark
build/schema_evolution_benchmark --repetitions "${REPETITIONS}" --registry-url "${REGISTRY_URL}" \
  --csv "${RESULTS_DIR}/schema_evolution_raw.csv" --metadata "${RESULTS_DIR}/schema_evolution_metadata.txt"
python3 scripts/schema_evolution_report.py --csv "${RESULTS_DIR}/schema_evolution_raw.csv" --metadata "${RESULTS_DIR}/schema_evolution_metadata.txt" \
  --output "${RESULTS_DIR}/SCHEMA_EVOLUTION_REPORT.md"
echo "Schema evolution report written to ${RESULTS_DIR}/SCHEMA_EVOLUTION_REPORT.md"

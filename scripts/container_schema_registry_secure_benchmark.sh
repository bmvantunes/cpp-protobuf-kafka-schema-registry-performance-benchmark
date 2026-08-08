#!/usr/bin/env bash
set -euo pipefail

cd /work

: "${ITERATIONS:=1000000}"
: "${REPETITIONS:=10}"
: "${WARMUP_ITERATIONS:=10000}"
: "${NETWORK_REPETITIONS:=10}"
: "${REGISTRY_URL:=https://schema-registry-secure:8082}"
: "${RESULTS_DIR:=/work/results}"
: "${USERNAME:=benchmark}"
: "${PASSWORD:=benchmark-secret}"

if [[ "${ITERATIONS}" -lt 1000000 || "${REPETITIONS}" -lt 10 || "${NETWORK_REPETITIONS}" -lt 10 ]]; then
  echo "Refusing an under-sized run: steady-state iterations/repetitions and network repetitions must be >= 1000000/10/10." >&2
  exit 2
fi

rm -rf generated build
mkdir -p "${RESULTS_DIR}"

echo "Generating types with buf..."
buf generate proto --template buf.gen.google_speed.yaml
buf generate proto --template buf.gen.google_codesize.yaml
buf generate proto --template buf.gen.google_lite.yaml
buf generate proto --template buf.gen.protobuf_c.yaml
buf generate proto --template buf.gen.representations.yaml

echo "Building secure Schema Registry benchmark..."
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel --target schema_registry_benchmark

build/schema_registry_benchmark \
  --iterations "${ITERATIONS}" \
  --repetitions "${REPETITIONS}" \
  --warmup-iterations "${WARMUP_ITERATIONS}" \
  --network-repetitions "${NETWORK_REPETITIONS}" \
  --registry-url "${REGISTRY_URL}" \
  --failure-url "${REGISTRY_URL%:*}:65530" \
  --username "${USERNAME}" \
  --password "${PASSWORD}" \
  --insecure-tls \
  --csv "${RESULTS_DIR}/schema_registry_secure_raw.csv" \
  --metadata "${RESULTS_DIR}/schema_registry_secure_metadata.txt"

python3 scripts/schema_registry_report.py \
  --baseline-csv "${RESULTS_DIR}/raw.csv" \
  --registry-csv "${RESULTS_DIR}/schema_registry_secure_raw.csv" \
  --metadata "${RESULTS_DIR}/schema_registry_secure_metadata.txt" \
  --output "${RESULTS_DIR}/SCHEMA_REGISTRY_SECURE_REPORT.md"

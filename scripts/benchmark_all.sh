#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE_NAME="${IMAGE_NAME:-protobuf-encoding-benchmark:all-latest}"
RESULTS_DIR="${RESULTS_DIR:-${ROOT_DIR}/results}"
ARCH="${ARCH:-$(uname -m)}"
REPORT_NAME="${REPORT_NAME:-${ARCH}}"

mkdir -p "${RESULTS_DIR}"
docker build --progress=plain -t "${IMAGE_NAME}" "${ROOT_DIR}"

export IMAGE_NAME SKIP_IMAGE_BUILD=1 RESULTS_DIR

run_phase() {
  echo "=== $1 ==="
  shift
  "$@"
}

run_phase "pure protobuf and JSON" "${ROOT_DIR}/scripts/benchmark.sh"
run_phase "compiler and C++ standard matrix" "${ROOT_DIR}/scripts/benchmark_compilers.sh"
run_phase "exact decimal representations" "${ROOT_DIR}/scripts/benchmark_representations.sh"
run_phase "int64 versus fixed64 wire encoding" "${ROOT_DIR}/scripts/benchmark_fixed64.sh"
run_phase "latency and allocations" "${ROOT_DIR}/scripts/benchmark_latency.sh"
run_phase "value distributions" "${ROOT_DIR}/scripts/benchmark_distributions.sh"
run_phase "concurrency and contention" "${ROOT_DIR}/scripts/benchmark_concurrency.sh"
run_phase "off-hot-path asynchronous encoding and logging pipeline" "${ROOT_DIR}/scripts/benchmark_async_pipeline.sh"
run_phase "hardware counters" "${ROOT_DIR}/scripts/benchmark_perf.sh"
run_phase "plain Schema Registry" "${ROOT_DIR}/scripts/benchmark_schema_registry.sh"
run_phase "secure Schema Registry" "${ROOT_DIR}/scripts/benchmark_schema_registry_secure.sh"
run_phase "schema evolution" "${ROOT_DIR}/scripts/benchmark_schema_evolution.sh"

if [[ "${RUN_KAFKA:-1}" == "1" ]]; then
  run_phase "real Kafka/librdkafka producer matrix" "${ROOT_DIR}/scripts/benchmark_kafka.sh"
fi

uname -a > "${RESULTS_DIR}/docker-host.txt"
docker version > "${RESULTS_DIR}/docker-version.txt"
docker info > "${RESULTS_DIR}/docker-info.txt"
python3 "${ROOT_DIR}/scripts/architecture_report.py" \
  --arch "${ARCH}" \
  --results "${RESULTS_DIR}" \
  --output "${RESULTS_DIR}/${REPORT_NAME}.md"

echo "Complete report: ${RESULTS_DIR}/${REPORT_NAME}.md"

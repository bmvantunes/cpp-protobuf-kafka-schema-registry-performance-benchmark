#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE_NAME="${IMAGE_NAME:-protobuf-encoding-benchmark:local}"
RESULTS_DIR="${RESULTS_DIR:-${ROOT_DIR}/results}"
COMPOSE_PROJECT="${COMPOSE_PROJECT:-protobuf-kafka-producer-benchmark}"
NETWORK_NAME="${COMPOSE_PROJECT}_default"

mkdir -p "${RESULTS_DIR}"

echo "Building benchmark image ${IMAGE_NAME}..."
docker build --progress=plain -t "${IMAGE_NAME}" "${ROOT_DIR}"

cleanup() {
  docker compose -p "${COMPOSE_PROJECT}" -f "${ROOT_DIR}/docker-compose.schema-registry.yml" down -v --remove-orphans >/dev/null 2>&1 || true
}
trap cleanup EXIT

echo "Starting Kafka and Schema Registry..."
docker compose -p "${COMPOSE_PROJECT}" -f "${ROOT_DIR}/docker-compose.schema-registry.yml" up -d

for attempt in $(seq 1 90); do
  if curl --fail --silent "http://localhost:18081/subjects" >/dev/null; then
    break
  fi
  if [[ "${attempt}" -eq 90 ]]; then
    docker compose -p "${COMPOSE_PROJECT}" -f "${ROOT_DIR}/docker-compose.schema-registry.yml" logs
    exit 1
  fi
  sleep 2
done

echo "Running Kafka producer benchmark on ${NETWORK_NAME}..."
docker run --rm \
  --network "${NETWORK_NAME}" \
  --cpuset-cpus="${CPUSET_CPUS:-0}" \
  -e ITERATIONS="${ITERATIONS:-1000000}" \
  -e REPETITIONS="${REPETITIONS:-10}" \
  -e WARMUP_ITERATIONS="${WARMUP_ITERATIONS:-10000}" \
  -e BROKER="${BROKER:-kafka:29092}" \
  -e TOPIC_PREFIX="${TOPIC_PREFIX:-protobuf_encoding_kafka}" \
  -e MODES="${MODES:-copy,owned}" \
  -e ACKS="${ACKS:-0,1,all}" \
  -e COMPRESSIONS="${COMPRESSIONS:-none,lz4,zstd}" \
  -e LINGER_MS="${LINGER_MS:-0,5}" \
  -e BATCH_NUM_MESSAGES="${BATCH_NUM_MESSAGES:-1,100}" \
  -e RESULTS_DIR=/work/results \
  -v "${RESULTS_DIR}:/work/results" \
  --entrypoint /work/scripts/container_kafka_benchmark.sh \
  "${IMAGE_NAME}"

echo "Done: ${RESULTS_DIR}/KAFKA_PRODUCER_REPORT.md"

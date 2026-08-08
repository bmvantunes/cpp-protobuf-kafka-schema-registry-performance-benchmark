#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE_NAME="${IMAGE_NAME:-protobuf-encoding-benchmark:local}"
RESULTS_DIR="${RESULTS_DIR:-${ROOT_DIR}/results}"
COMPOSE_PROJECT="${COMPOSE_PROJECT:-protobuf-schema-registry-secure-benchmark}"
COMPOSE_FILE="${ROOT_DIR}/docker-compose.schema-registry-secure.yml"
NETWORK_NAME="${COMPOSE_PROJECT}_default"

mkdir -p "${RESULTS_DIR}"

if [[ "${SKIP_IMAGE_BUILD:-0}" != "1" ]]; then
  echo "Building benchmark image ${IMAGE_NAME}..."
  docker build --progress=plain -t "${IMAGE_NAME}" "${ROOT_DIR}"
fi

cleanup() {
  docker compose -p "${COMPOSE_PROJECT}" -f "${COMPOSE_FILE}" down -v --remove-orphans >/dev/null 2>&1 || true
}
trap cleanup EXIT

echo "Starting secure Confluent Kafka and Schema Registry..."
docker compose -p "${COMPOSE_PROJECT}" -f "${COMPOSE_FILE}" up -d

for attempt in $(seq 1 180); do
  if curl --http1.1 --insecure --fail --silent --resolve schema-registry-secure:18082:127.0.0.1 \
      --user "${SCHEMA_REGISTRY_USERNAME:-benchmark}:${SCHEMA_REGISTRY_PASSWORD:-benchmark-secret}" \
      "https://schema-registry-secure:18082/subjects" >/dev/null; then
    break
  fi
  if [[ "${attempt}" -eq 180 ]]; then
    echo "Secure Schema Registry did not become ready." >&2
    docker compose -p "${COMPOSE_PROJECT}" -f "${COMPOSE_FILE}" logs
    exit 1
  fi
  sleep 2
done

docker run --rm \
  --network "${NETWORK_NAME}" \
  --cpuset-cpus="${CPUSET_CPUS:-0}" \
  -e ITERATIONS="${ITERATIONS:-1000000}" \
  -e REPETITIONS="${REPETITIONS:-10}" \
  -e WARMUP_ITERATIONS="${WARMUP_ITERATIONS:-10000}" \
  -e NETWORK_REPETITIONS="${NETWORK_REPETITIONS:-10}" \
  -e REGISTRY_URL="${REGISTRY_URL:-https://schema-registry-secure:8082}" \
  -e USERNAME="${SCHEMA_REGISTRY_USERNAME:-benchmark}" \
  -e PASSWORD="${SCHEMA_REGISTRY_PASSWORD:-benchmark-secret}" \
  -e RESULTS_DIR=/work/results \
  -v "${RESULTS_DIR}:/work/results" \
  --entrypoint /work/scripts/container_schema_registry_secure_benchmark.sh \
  "${IMAGE_NAME}"

echo "Done: ${RESULTS_DIR}/SCHEMA_REGISTRY_SECURE_REPORT.md"

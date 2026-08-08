#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE_NAME="${IMAGE_NAME:-protobuf-encoding-benchmark:local}"
RESULTS_DIR="${RESULTS_DIR:-${ROOT_DIR}/results}"
COMPOSE_PROJECT="${COMPOSE_PROJECT:-protobuf-schema-evolution-benchmark}"
NETWORK_NAME="${COMPOSE_PROJECT}_default"
mkdir -p "${RESULTS_DIR}"
if [[ "${SKIP_IMAGE_BUILD:-0}" != "1" ]]; then
  docker build --progress=plain -t "${IMAGE_NAME}" "${ROOT_DIR}"
fi
cleanup() { docker compose -p "${COMPOSE_PROJECT}" -f "${ROOT_DIR}/docker-compose.schema-registry.yml" down -v --remove-orphans >/dev/null 2>&1 || true; }
trap cleanup EXIT
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
docker run --rm --network "${NETWORK_NAME}" --cpuset-cpus="${CPUSET_CPUS:-0}" \
  -e REPETITIONS="${REPETITIONS:-10}" -e REGISTRY_URL="${REGISTRY_URL:-http://schema-registry:8081}" -e RESULTS_DIR=/work/results \
  -v "${RESULTS_DIR}:/work/results" --entrypoint /work/scripts/container_schema_evolution_benchmark.sh "${IMAGE_NAME}"
echo "Done: ${RESULTS_DIR}/SCHEMA_EVOLUTION_REPORT.md"

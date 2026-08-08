#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE_NAME="${IMAGE_NAME:-protobuf-encoding-benchmark:local}"
RESULTS_DIR="${RESULTS_DIR:-${ROOT_DIR}/results}"
mkdir -p "${RESULTS_DIR}"
if [[ "${SKIP_IMAGE_BUILD:-0}" != "1" ]]; then
  docker build --progress=plain -t "${IMAGE_NAME}" "${ROOT_DIR}"
fi
docker run --rm --network=none --cpuset-cpus="${CPUSET_CPUS:-0}" \
  -e ITERATIONS="${ITERATIONS:-1000000}" \
  -e REPETITIONS="${REPETITIONS:-10}" \
  -e WARMUP_ITERATIONS="${WARMUP_ITERATIONS:-10000}" \
  -e RESULTS_DIR=/work/results -v "${RESULTS_DIR}:/work/results" \
  --entrypoint /work/scripts/container_latency_benchmark.sh "${IMAGE_NAME}"
echo "Done: ${RESULTS_DIR}/LATENCY_REPORT.md"

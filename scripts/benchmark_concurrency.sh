#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE_NAME="${IMAGE_NAME:-protobuf-encoding-benchmark:local}"
RESULTS_DIR="${RESULTS_DIR:-${ROOT_DIR}/results}"
THREADS="${THREADS:-1,2,4,8}"
MODES="${MODES:-thread_local,shared_readonly,shared_buffer_mutex}"
mkdir -p "${RESULTS_DIR}"
docker build --progress=plain -t "${IMAGE_NAME}" "${ROOT_DIR}"
docker run --rm --network=none --cpuset-cpus="${CPUSET_CPUS:-0-7}" \
  -e ITERATIONS="${ITERATIONS:-1000000}" -e REPETITIONS="${REPETITIONS:-10}" -e WARMUP_ITERATIONS="${WARMUP_ITERATIONS:-10000}" \
  -e THREADS="${THREADS}" -e MODES="${MODES}" -e RESULTS_DIR=/work/results -v "${RESULTS_DIR}:/work/results" \
  --entrypoint /work/scripts/container_concurrency_benchmark.sh "${IMAGE_NAME}"
echo "Done: ${RESULTS_DIR}/CONCURRENCY_REPORT.md"

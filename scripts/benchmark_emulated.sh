#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PLATFORM="${PLATFORM:-linux/amd64}"
IMAGE_NAME="${IMAGE_NAME:-protobuf-encoding-benchmark:emulated-amd64}"
RESULTS_DIR="${RESULTS_DIR:-${ROOT_DIR}/results/emulated-amd64}"

rm -rf "${RESULTS_DIR}"
mkdir -p "${RESULTS_DIR}"

uname -a > "${RESULTS_DIR}/docker-host.txt"
docker version > "${RESULTS_DIR}/docker-version.txt"
docker info > "${RESULTS_DIR}/docker-info.txt"

docker build --platform "${PLATFORM}" --progress=plain -t "${IMAGE_NAME}" "${ROOT_DIR}"
docker run --rm --platform "${PLATFORM}" \
  --network=none \
  --cpuset-cpus="${CPUSET_CPUS:-0}" \
  -e ITERATIONS="${ITERATIONS:-1000000}" \
  -e REPETITIONS="${REPETITIONS:-10}" \
  -e WARMUP_ITERATIONS="${WARMUP_ITERATIONS:-10000}" \
  -e RESULTS_DIR=/work/results \
  -v "${RESULTS_DIR}:/work/results" \
  --entrypoint /work/scripts/container_benchmark.sh \
  "${IMAGE_NAME}"

python3 "${ROOT_DIR}/scripts/architecture_report.py" \
  --arch "${PLATFORM}-emulated" \
  --results "${RESULTS_DIR}" \
  --output "${ROOT_DIR}/results/emulated-amd64.md"

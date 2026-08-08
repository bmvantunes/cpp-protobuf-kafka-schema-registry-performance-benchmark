#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE_NAME="${IMAGE_NAME:-protobuf-encoding-benchmark:local}"
RESULTS_DIR="${COMPILER_RESULTS_DIR:-${ROOT_DIR}/results/compiler}"

mkdir -p "${RESULTS_DIR}"

if [[ "${SKIP_IMAGE_BUILD:-0}" != "1" ]]; then
  docker build --progress=plain -t "${IMAGE_NAME}" "${ROOT_DIR}"
fi

rm -rf "${RESULTS_DIR}"
mkdir -p "${RESULTS_DIR}"

failures=0
for definition in \
  "gcc-cxx23|gcc|g++|23|GCC 15 C++23" \
  "clang-cxx23|clang-22|clang++-22|23|Clang 22.1.2 C++23" \
  "gcc-cxx26|gcc|g++|26|GCC 15 C++26" \
  "clang-cxx26|clang-22|clang++-22|26|Clang 22.1.2 C++26"; do
  IFS='|' read -r variant cc cxx standard name <<< "${definition}"
  echo "Running compiler variant ${name} (${variant})..."
  run_status=0
  docker run --rm \
    --network=none \
    --cpuset-cpus="${CPUSET_CPUS:-0}" \
    -e ITERATIONS="${ITERATIONS:-1000000}" \
    -e REPETITIONS="${REPETITIONS:-10}" \
    -e WARMUP_ITERATIONS="${WARMUP_ITERATIONS:-10000}" \
    -e RESULTS_DIR=/work/results/compiler \
    -e VARIANT="${variant}" \
    -e CC="${cc}" \
    -e CXX="${cxx}" \
    -e CXX_STANDARD="${standard}" \
    -e COMPILER_NAME="${name}" \
    -v "${ROOT_DIR}/results:/work/results" \
    --entrypoint /work/scripts/container_compiler_benchmark.sh \
    "${IMAGE_NAME}" || run_status=$?
  if [[ "${run_status}" -ne 0 ]]; then
    printf 'status=failed\nvariant=%s\ncompiler_name=%s\n' "${variant}" "${name}" > "${RESULTS_DIR}/${variant}.failed"
    failures=$((failures + 1))
  fi
done

python3 "${ROOT_DIR}/scripts/compiler_report.py" \
  --root "${RESULTS_DIR}" \
  --output "${ROOT_DIR}/results/COMPILER_REPORT.md"

if [[ "${failures}" -ne 0 ]]; then
  echo "${failures} compiler variant(s) failed; see results/compiler/*.failed" >&2
  exit 1
fi

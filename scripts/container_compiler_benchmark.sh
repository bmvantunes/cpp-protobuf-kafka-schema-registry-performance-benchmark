#!/usr/bin/env bash
set -euo pipefail

cd /work

: "${ITERATIONS:=1000000}"
: "${REPETITIONS:=10}"
: "${WARMUP_ITERATIONS:=10000}"
: "${RESULTS_DIR:=/work/results}"
: "${VARIANT:=unknown}"
: "${CC:=gcc}"
: "${CXX:=g++}"
: "${CXX_STANDARD:=23}"
: "${COMPILER_NAME:=${CXX}}"

if [[ "${ITERATIONS}" -lt 1000000 || "${REPETITIONS}" -lt 10 ]]; then
  echo "Refusing an under-sized run: ITERATIONS >= 1000000 and REPETITIONS >= 10 are required." >&2
  exit 2
fi

OUT_DIR="${RESULTS_DIR}/${VARIANT}"
rm -rf generated build "${OUT_DIR}"
mkdir -p "${OUT_DIR}"

buf generate proto --template buf.gen.google_speed.yaml
buf generate proto --template buf.gen.google_codesize.yaml
buf generate proto --template buf.gen.google_lite.yaml
buf generate proto --template buf.gen.protobuf_c.yaml
buf generate proto --template buf.gen.representations.yaml

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER="${CC}" \
  -DCMAKE_CXX_COMPILER="${CXX}" \
  -DCMAKE_CXX_STANDARD="${CXX_STANDARD}"
cmake --build build --parallel --target encode_benchmark

{
  printf 'variant=%s\n' "${VARIANT}"
  printf 'compiler_name=%s\n' "${COMPILER_NAME}"
  printf 'cc=%s\n' "${CC}"
  printf 'cxx=%s\n' "${CXX}"
  printf 'compiler_version=%s\n' "$("${CXX}" --version | head -1)"
  printf 'cxx_standard=%s\n' "${CXX_STANDARD}"
  printf 'architecture=%s\n' "$(uname -m)"
  printf 'iterations=%s\n' "${ITERATIONS}"
  printf 'repetitions=%s\n' "${REPETITIONS}"
  printf 'warmup_iterations=%s\n' "${WARMUP_ITERATIONS}"
  printf 'compiler_flags=-O3 -march=native -DNDEBUG\n'
} > "${OUT_DIR}/metadata.txt"

build/encode_benchmark \
  --iterations "${ITERATIONS}" \
  --repetitions "${REPETITIONS}" \
  --warmup-iterations "${WARMUP_ITERATIONS}" \
  --csv "${OUT_DIR}/raw.csv" \
  --metadata "${OUT_DIR}/run_metadata.txt"

python3 scripts/report.py \
  --csv "${OUT_DIR}/raw.csv" \
  --metadata "${OUT_DIR}/run_metadata.txt" \
  --output "${OUT_DIR}/REPORT.md"

cat "${OUT_DIR}/metadata.txt" >> "${OUT_DIR}/run_metadata.txt"

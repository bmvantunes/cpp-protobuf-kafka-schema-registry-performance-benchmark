#!/usr/bin/env bash
set -euo pipefail

cd /work

: "${ITERATIONS:=1000000}"
: "${REPETITIONS:=10}"
: "${WARMUP_ITERATIONS:=10000}"
: "${RESULTS_DIR:=/work/results}"

if [[ "${ITERATIONS}" -lt 1000000 || "${REPETITIONS}" -lt 10 ]]; then
  echo "Refusing an under-sized run: ITERATIONS >= 1000000 and REPETITIONS >= 10 are required." >&2
  exit 2
fi

rm -rf generated build
mkdir -p "${RESULTS_DIR}"

package_version() {
  dpkg-query -W -f='${Version}' "$1" 2>/dev/null || printf 'not-installed'
}

{
  printf 'architecture=%s\n' "$(uname -m)"
  printf 'kernel=%s\n' "$(uname -r)"
  printf 'compiler=%s\n' "$(c++ --version | head -1)"
  printf 'cmake=%s\n' "$(cmake --version | head -1)"
  printf 'buf=%s\n' "$(buf --version)"
  printf 'protoc=%s\n' "$(protoc --version)"
  printf 'python=%s\n' "$(python3 --version)"
  printf 'protobuf_package=%s\n' "$(package_version libprotobuf-dev)"
  printf 'protobuf_c_package=%s\n' "$(package_version libprotobuf-c-dev)"
  printf 'boost_json_package=%s\n' "$(package_version libboost-json1.83-dev)"
  printf 'jsoncpp_package=%s\n' "$(package_version libjsoncpp-dev)"
  printf 'rapidjson_package=%s\n' "$(package_version rapidjson-dev)"
  printf 'librdkafka=%s\n' "$(pkg-config --modversion rdkafka 2>/dev/null || printf 'source-build-version-2.8.0')"
  printf 'yyjson=0.12.0\n'
  printf 'nlohmann_json=3.12.0\n'
} > "${RESULTS_DIR}/toolchain_versions.txt"

echo "Generating C++ and protobuf-c types with buf ${BUF_VERSION:-pinned-image-version}..."
buf generate proto --template buf.gen.google_speed.yaml
buf generate proto --template buf.gen.google_codesize.yaml
buf generate proto --template buf.gen.google_lite.yaml
buf generate proto --template buf.gen.protobuf_c.yaml
buf generate proto --template buf.gen.representations.yaml

echo "Building release benchmark..."
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

echo "Running ${ITERATIONS} encodes x ${REPETITIONS} repetitions per benchmark..."
build/encode_benchmark \
  --iterations "${ITERATIONS}" \
  --repetitions "${REPETITIONS}" \
  --warmup-iterations "${WARMUP_ITERATIONS}" \
  --csv "${RESULTS_DIR}/raw.csv" \
  --metadata "${RESULTS_DIR}/metadata.txt"

python3 scripts/report.py \
  --csv "${RESULTS_DIR}/raw.csv" \
  --metadata "${RESULTS_DIR}/metadata.txt" \
  --output "${RESULTS_DIR}/REPORT.md"

echo "Report written to ${RESULTS_DIR}/REPORT.md"

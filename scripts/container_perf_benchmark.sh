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
for t in buf.gen.google_speed.yaml buf.gen.google_codesize.yaml buf.gen.google_lite.yaml buf.gen.protobuf_c.yaml buf.gen.representations.yaml; do
  buf generate proto --template "$t"
done
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel --target representation_benchmark
set +e
perf stat -x, -e cycles,instructions,cache-references,cache-misses,branches,branch-misses \
  -o "${RESULTS_DIR}/perf_stat.txt" \
  build/representation_benchmark --iterations "${ITERATIONS}" --repetitions "${REPETITIONS}" --warmup-iterations "${WARMUP_ITERATIONS}" \
  --csv "${RESULTS_DIR}/perf_representation_raw.csv" --metadata "${RESULTS_DIR}/perf_representation_metadata.txt"
status=$?
set -e
printf 'perf_exit_status=%s\niterations=%s\nrepetitions=%s\n' "${status}" "${ITERATIONS}" "${REPETITIONS}" > "${RESULTS_DIR}/perf_metadata.txt"
python3 scripts/perf_report.py --stat "${RESULTS_DIR}/perf_stat.txt" --metadata "${RESULTS_DIR}/perf_metadata.txt" --output "${RESULTS_DIR}/PERF_REPORT.md"
if [[ "${status}" -ne 0 ]]; then
  echo "perf did not complete successfully; see PERF_REPORT.md" >&2
fi

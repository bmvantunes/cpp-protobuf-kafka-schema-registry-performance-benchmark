#!/usr/bin/env bash
set -euo pipefail
cd /work
: "${ITERATIONS:=1000000}"
: "${REPETITIONS:=10}"
: "${WARMUP_ITERATIONS:=10000}"
: "${THREADS:=1,2,4,8}"
: "${MODES:=thread_local,shared_readonly,shared_buffer_mutex}"
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
cmake --build build --parallel --target concurrency_benchmark
rm -f "${RESULTS_DIR}"/concurrency_*.csv "${RESULTS_DIR}"/concurrency_metadata.txt
IFS=',' read -r -a thread_values <<< "${THREADS}"
IFS=',' read -r -a mode_values <<< "${MODES}"
run_index=0
for mode in "${mode_values[@]}"; do
  for threads in "${thread_values[@]}"; do
    run_index=$((run_index + 1))
    build/concurrency_benchmark --iterations "${ITERATIONS}" --repetitions "${REPETITIONS}" --warmup-iterations "${WARMUP_ITERATIONS}" \
      --threads "${threads}" --mode "${mode}" --csv "${RESULTS_DIR}/concurrency_${run_index}.csv" \
      --metadata "${RESULTS_DIR}/concurrency_${run_index}.metadata.txt"
  done
done
printf 'runs=%s\niterations=%s\nrepetitions=%s\nthreads=%s\nmodes=%s\n' "${run_index}" "${ITERATIONS}" "${REPETITIONS}" "${THREADS}" "${MODES}" > "${RESULTS_DIR}/concurrency_metadata.txt"
python3 scripts/concurrency_report.py --glob "${RESULTS_DIR}/concurrency_*.csv" --metadata "${RESULTS_DIR}/concurrency_metadata.txt" --output "${RESULTS_DIR}/CONCURRENCY_REPORT.md"
echo "Concurrency report written to ${RESULTS_DIR}/CONCURRENCY_REPORT.md"

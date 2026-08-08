#!/usr/bin/env bash
set -euo pipefail

cd /work

: "${ITERATIONS:=1000000}"
: "${REPETITIONS:=10}"
: "${WARMUP_ITERATIONS:=10000}"
: "${BROKER:=kafka:29092}"
: "${TOPIC_PREFIX:=protobuf_encoding_kafka}"
: "${MODES:=copy,owned}"
: "${ACKS:=0,1,all}"
: "${COMPRESSIONS:=none,lz4,zstd}"
: "${LINGER_MS:=0,5}"
: "${BATCH_NUM_MESSAGES:=1,100}"
: "${RESULTS_DIR:=/work/results}"

if [[ "${ITERATIONS}" -lt 1000000 || "${REPETITIONS}" -lt 10 ]]; then
  echo "Refusing an under-sized run: ITERATIONS >= 1000000 and REPETITIONS >= 10 are required." >&2
  exit 2
fi

rm -rf generated build
mkdir -p "${RESULTS_DIR}"

echo "Generating Google protobuf types with buf..."
buf generate proto --template buf.gen.google_speed.yaml
buf generate proto --template buf.gen.google_codesize.yaml
buf generate proto --template buf.gen.google_lite.yaml
buf generate proto --template buf.gen.protobuf_c.yaml
buf generate proto --template buf.gen.representations.yaml

echo "Building Kafka producer benchmark..."
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel --target kafka_producer_benchmark

IFS=',' read -r -a mode_values <<< "${MODES}"
IFS=',' read -r -a ack_values <<< "${ACKS}"
IFS=',' read -r -a compression_values <<< "${COMPRESSIONS}"
IFS=',' read -r -a linger_values <<< "${LINGER_MS}"
IFS=',' read -r -a batch_values <<< "${BATCH_NUM_MESSAGES}"

rm -f "${RESULTS_DIR}"/kafka_producer_*.csv "${RESULTS_DIR}"/kafka_producer_*.metadata.txt "${RESULTS_DIR}/kafka_producer_metadata.txt"
run_index=0
for mode in "${mode_values[@]}"; do
  for acks in "${ack_values[@]}"; do
    for compression in "${compression_values[@]}"; do
      for linger in "${linger_values[@]}"; do
        for batch in "${batch_values[@]}"; do
          run_index=$((run_index + 1))
          topic="${TOPIC_PREFIX}_${run_index}_${mode}_${acks}_${compression}_${linger}_${batch}"
          output="${RESULTS_DIR}/kafka_producer_${run_index}.csv"
          metadata="${RESULTS_DIR}/kafka_producer_${run_index}.metadata.txt"
          echo "Run ${run_index}: mode=${mode} acks=${acks} compression=${compression} linger_ms=${linger} batch_num_messages=${batch}"
          build/kafka_producer_benchmark \
            --iterations "${ITERATIONS}" \
            --repetitions "${REPETITIONS}" \
            --warmup-iterations "${WARMUP_ITERATIONS}" \
            --broker "${BROKER}" \
            --topic "${topic}" \
            --mode "${mode}" \
            --acks "${acks}" \
            --compression "${compression}" \
            --linger-ms "${linger}" \
            --batch-num-messages "${batch}" \
            --csv "${output}" \
            --metadata "${metadata}"
        done
      done
    done
  done
done

printf 'runs=%s\niterations=%s\nrepetitions=%s\n' "${run_index}" "${ITERATIONS}" "${REPETITIONS}" > "${RESULTS_DIR}/kafka_producer_metadata.txt"
python3 scripts/kafka_report.py \
  --glob "${RESULTS_DIR}/kafka_producer_*.csv" \
  --metadata "${RESULTS_DIR}/kafka_producer_metadata.txt" \
  --output "${RESULTS_DIR}/KAFKA_PRODUCER_REPORT.md"

echo "Kafka producer report written to ${RESULTS_DIR}/KAFKA_PRODUCER_REPORT.md"

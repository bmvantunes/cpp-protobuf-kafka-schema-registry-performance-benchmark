# C++ Kafka serialization benchmark — arm64

> Encoding only. No decode, parse, consumer, or deserialization benchmark is included.

## Run identity

- Requested architecture: `arm64`
- Observed machine: `macOS-26.6-arm64-arm-64bit`
- Observed machine architecture: `arm64`
- Python: `3.9.6`
- Steady-state contract: `1,000,000` encodes per measured repetition and `10` measured repetitions.
- Control-plane Registry paths intentionally use ten live requests per path; they are not hot-loop encode measurements.
- Every phase was executed through Docker; absolute values are host- and scheduler-dependent.

## Phase index

- [REPORT.md](#reportmd)
- [REPRESENTATION_REPORT.md](#representation-reportmd)
- [LATENCY_REPORT.md](#latency-reportmd)
- [DISTRIBUTION_REPORT.md](#distribution-reportmd)
- [CONCURRENCY_REPORT.md](#concurrency-reportmd)
- [PERF_REPORT.md](#perf-reportmd)
- [KAFKA_PRODUCER_REPORT.md](#kafka-producer-reportmd)
- [SCHEMA_REGISTRY_REPORT.md](#schema-registry-reportmd)
- [SCHEMA_REGISTRY_SECURE_REPORT.md](#schema-registry-secure-reportmd)
- [SCHEMA_EVOLUTION_REPORT.md](#schema-evolution-reportmd)

## REPORT.md

# Protobuf and JSON encoding benchmark

> Encoding only. No decode, parse, or schema-registry/network time is included.

## Run contract

- Encodes per repetition: `1000000`
- Repetitions per benchmark: `10`
- Warmup encodes (excluded): `10000`
- Decimal representation: protobuf and JSON both use decimal strings; JSON does not parse floating-point decimals.
- Generated types: `buf generate` with Google C++ `SPEED`, `CODE_SIZE`, and `LITE_RUNTIME` variants, plus protobuf-c.

## Verdict by payload

The fastest row is selected by median nanoseconds per encode across the recorded repetitions.

| Payload | Fastest | Median ns/encode | Encodes/sec | Bytes |
|---|---|---:|---:|---:|
| one_string_ten_decimal_strings | protobuf_c / c_generated / pack_preallocated | 98.94 | 9.69 M/s | 213 |
| one_string_ten_int64 | google_protobuf / lite_runtime / SerializeToArray_preallocated | 77.26 | 12.88 M/s | 49 |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated / pack_preallocated | 520.67 | 1.94 M/s | 1193 |

## Full aggregate results

| Kind | Library | Codegen | API | Payload | Reps | Bytes | Median ns/encode | Mean ns/encode | Mean M/s |
|---|---|---|---|---|---:|---:|---:|---:|---:|
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 213 | 98.94 | 104.25 | 9.69 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 148.72 | 149.95 | 6.67 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 150.86 | 152.84 | 6.55 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 154.46 | 156.68 | 6.39 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 156.00 | 161.19 | 6.23 |
| json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 414 | 223.44 | 226.13 | 4.43 |
| json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 364 | 311.93 | 316.13 | 3.17 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 1,069.04 | 1,098.56 | 0.91 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 1,077.77 | 1,079.32 | 0.93 |
| json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 364 | 1,150.09 | 1,153.73 | 0.87 |
| json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 364 | 1,193.51 | 1,193.45 | 0.84 |
| json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 364 | 2,270.36 | 2,295.65 | 0.44 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 77.26 | 77.68 | 12.88 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 82.47 | 82.84 | 12.07 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 49 | 84.30 | 84.83 | 11.81 |
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 49 | 85.70 | 89.38 | 11.32 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 49 | 87.25 | 87.69 | 11.41 |
| json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 210 | 87.91 | 90.03 | 11.13 |
| json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 210 | 274.17 | 275.53 | 3.63 |
| json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 210 | 377.03 | 380.79 | 2.63 |
| json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 210 | 675.18 | 675.90 | 1.48 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 851.99 | 871.54 | 1.15 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 49 | 864.27 | 894.08 | 1.12 |
| json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 210 | 1,750.40 | 1,768.68 | 0.57 |
| protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 520.67 | 516.92 | 1.94 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 606.25 | 611.41 | 1.64 |
| protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 612.99 | 616.04 | 1.62 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 658.03 | 657.98 | 1.52 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 662.41 | 672.29 | 1.49 |
| json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 2229 | 752.18 | 757.06 | 1.32 |
| json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1979 | 1,479.74 | 1,489.01 | 0.67 |
| protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 5,516.89 | 5,554.96 | 0.18 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 5,795.95 | 5,763.61 | 0.17 |
| json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 1979 | 5,985.25 | 6,163.27 | 0.16 |
| json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 1979 | 6,529.96 | 6,567.27 | 0.15 |
| json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 1979 | 11,261.72 | 11,275.36 | 0.09 |

## Protobuf-only comparison

| Payload | Library / generation | API | Median ns/encode | Relative to fastest protobuf |
|---|---|---|---:|---:|
| one_string_ten_decimal_strings | protobuf_c / c_generated | pack_preallocated | 98.94 | 1.00x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 148.72 | 1.50x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 150.86 | 1.52x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToString | 154.46 | 1.56x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 156.00 | 1.58x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 1,069.04 | 10.81x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToString | 1,077.77 | 10.89x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToArray_preallocated | 77.26 | 1.00x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToArray_preallocated | 82.47 | 1.07x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToString | 84.30 | 1.09x |
| one_string_ten_int64 | protobuf_c / c_generated | pack_preallocated | 85.70 | 1.11x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToString | 87.25 | 1.13x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToArray_preallocated | 851.99 | 11.03x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToString | 864.27 | 11.19x |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated | pack_preallocated | 520.67 | 1.00x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 606.25 | 1.16x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToString | 612.99 | 1.18x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 658.03 | 1.26x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 662.41 | 1.27x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToString | 5,516.89 | 10.60x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 5,795.95 | 11.13x |

## JSON-only comparison

| Payload | Library | Median ns/encode | Relative to fastest JSON |
|---|---|---:|---:|
| one_string_ten_decimal_strings | yyjson | 223.44 | 1.00x |
| one_string_ten_decimal_strings | boost_json | 311.93 | 1.40x |
| one_string_ten_decimal_strings | rapidjson | 1,150.09 | 5.15x |
| one_string_ten_decimal_strings | nlohmann_json | 1,193.51 | 5.34x |
| one_string_ten_decimal_strings | jsoncpp | 2,270.36 | 10.16x |
| one_string_ten_int64 | yyjson | 87.91 | 1.00x |
| one_string_ten_int64 | boost_json | 274.17 | 3.12x |
| one_string_ten_int64 | rapidjson | 377.03 | 4.29x |
| one_string_ten_int64 | nlohmann_json | 675.18 | 7.68x |
| one_string_ten_int64 | jsoncpp | 1,750.40 | 19.91x |
| ten_strings_fifty_decimal_strings | yyjson | 752.18 | 1.00x |
| ten_strings_fifty_decimal_strings | boost_json | 1,479.74 | 1.97x |
| ten_strings_fifty_decimal_strings | nlohmann_json | 5,985.25 | 7.96x |
| ten_strings_fifty_decimal_strings | rapidjson | 6,529.96 | 8.68x |
| ten_strings_fifty_decimal_strings | jsoncpp | 11,261.72 | 14.97x |

## Raw data

- `raw.csv` contains every individual repetition and is the source for the aggregates above.
- Re-run on a quiet, pinned CPU if comparing small differences; CPU frequency, thermal state, compiler, allocator, and container host affect absolute numbers.


## REPRESENTATION_REPORT.md

# Exact decimal representation benchmark

> Protobuf encoding only. No decoding, parsing, or network work is included.

## Run contract

- Encodes per repetition: `1000000`
- Repetitions: `10`
- Warmup encodes excluded: `10000`
- Integer/binary scale: `8` decimal places
- The integer and binary representations use the same fixed-scale numeric values; the string representation preserves decimal text.

## Results

| Representation | Reps | Bytes | Median ns/encode | Mean ns/encode | p95 ns | p99 ns | p99.9 ns | p99.99 ns | Mean M/s |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| fixed64_scale_8 | 10 | 99 | 58.22 | 58.59 | 59.97 | 59.97 | 59.97 | 59.97 | 17.07 |
| bytes_128bit_coefficient_scale_8 | 10 | 189 | 77.27 | 78.65 | 89.30 | 89.30 | 89.30 | 89.30 | 12.71 |
| scaled_sint64_scale_8 | 10 | 79 | 81.67 | 84.13 | 93.16 | 93.16 | 93.16 | 93.16 | 11.89 |
| decimal_string | 10 | 169 | 154.34 | 154.46 | 155.61 | 155.61 | 155.61 | 155.61 | 6.47 |

## Trade-off

- Strings are self-describing and preserve exact text, but include decimal characters and protobuf length-delimited overhead.
- Fixed-scale `sint64` is compact for values that fit the agreed scale and range; the scale must be part of the schema contract.
- `fixed64` avoids varint-size variation but always spends eight coefficient bytes.
- `bytes` with a 16-byte coefficient supports a wider exact range, at the cost of a larger fixed payload.


## LATENCY_REPORT.md

# Per-encode latency and allocation benchmark

> Every percentile below is calculated from one million individually timed encodes per repetition. No decoding or network work is included.

## Run contract

- Encodes per repetition: `1000000`
- Repetitions: `10`
- Warmup encodes excluded: `10000`
- Instrumentation uses a timer around every encode, so these are instrumented latency measurements and should be compared within this report, not directly substituted for the aggregate throughput benchmark.

## Results

| Payload | Path | Reps | Bytes | Median p50 ns | p90 ns | p99 ns | p99.9 ns | p99.99 ns | Max ns | Mean allocations/rep | Mean allocated bytes/rep |
|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| one_string_ten_decimal_strings | serialize_array_arena_message | 10 | 213 | 167.00 | 208.00 | 209.00 | 250.50 | 6,896.00 | 88,502.00 | 0.00 | 0.00 |
| one_string_ten_decimal_strings | serialize_array_fresh_buffer | 10 | 213 | 208.00 | 209.00 | 229.50 | 250.00 | 5,896.00 | 56,918.00 | 1,000,000.00 | 213,000,000.00 |
| one_string_ten_decimal_strings | serialize_array_reuse | 10 | 213 | 167.00 | 187.50 | 209.00 | 209.00 | 5,354.50 | 33,730.00 | 0.00 | 0.00 |
| one_string_ten_decimal_strings | serialize_string_reserved | 10 | 213 | 167.00 | 208.00 | 209.00 | 209.00 | 4,145.50 | 36,355.50 | 0.00 | 0.00 |
| one_string_ten_int64 | serialize_array_arena_message | 10 | 49 | 125.00 | 125.00 | 125.00 | 126.00 | 2,958.00 | 25,250.50 | 0.00 | 0.00 |
| one_string_ten_int64 | serialize_array_fresh_buffer | 10 | 49 | 125.00 | 166.00 | 167.00 | 167.00 | 2,958.50 | 40,230.00 | 1,000,000.00 | 49,000,000.00 |
| one_string_ten_int64 | serialize_array_reuse | 10 | 49 | 125.00 | 125.00 | 125.00 | 126.00 | 2,958.00 | 30,834.00 | 0.00 | 0.00 |
| one_string_ten_int64 | serialize_string_reserved | 10 | 49 | 125.00 | 125.00 | 125.00 | 167.00 | 3,000.00 | 31,688.00 | 0.00 | 0.00 |
| ten_strings_fifty_decimal_strings | serialize_array_arena_message | 10 | 1193 | 625.00 | 625.00 | 708.00 | 2,729.00 | 12,917.00 | 119,190.50 | 0.00 | 0.00 |
| ten_strings_fifty_decimal_strings | serialize_array_fresh_buffer | 10 | 1193 | 667.00 | 708.00 | 750.00 | 3,416.50 | 20,563.00 | 771,019.00 | 1,000,000.00 | 1,193,000,000.00 |
| ten_strings_fifty_decimal_strings | serialize_array_reuse | 10 | 1193 | 625.00 | 625.00 | 667.00 | 2,417.00 | 13,417.50 | 103,482.00 | 0.00 | 0.00 |
| ten_strings_fifty_decimal_strings | serialize_string_reserved | 10 | 1193 | 666.50 | 667.00 | 1,604.50 | 5,229.50 | 51,835.00 | 4,158,168.00 | 0.00 | 0.00 |

## Allocation interpretation

- `serialize_array_reuse` measures a caller-owned buffer reused across encodes.
- `serialize_array_fresh_buffer` intentionally allocates a new output vector for each encode and exposes allocator cost.
- `serialize_string_reserved` reuses a reserved string, while `serialize_array_arena_message` serializes a message allocated on a protobuf Arena.
- Allocation counts include allocations observed by the benchmark process during the timed loop; they are not a decoding metric.


## DISTRIBUTION_REPORT.md

# Realistic value distribution benchmark

> Protobuf encoding only. No decoding or network work is included.

## Run contract

- Encodes per repetition: `1000000`
- Repetitions: `10`
- Warmup encodes excluded: `10000`
- Fixed corpus size: `1024` pre-populated messages
- The corpus changes which values are serialized without measuring message construction or random-number generation inside the timed loop.

## Results

| Distribution | Reps | Mean bytes | Median ns/encode | Mean ns/encode | Mean M/s |
|---|---:|---:|---:|---:|---:|
| int64_small_varints | 10 | 29.00 | 78.77 | 78.80 | 12.69 |
| string_short_ascii | 10 | 49.00 | 94.75 | 97.80 | 10.30 |
| string_multibyte_utf8 | 10 | 67.92 | 104.08 | 106.36 | 9.42 |
| string_long_ascii_256 | 10 | 299.00 | 104.67 | 105.83 | 9.46 |
| int64_mixed_distribution | 10 | 76.50 | 104.68 | 105.82 | 9.46 |
| int64_large_varints | 10 | 109.00 | 114.04 | 117.23 | 8.57 |
| int64_negative_standard_varint | 10 | 119.00 | 118.49 | 121.34 | 8.26 |

## Interpretation

- Standard negative `int64` values use the ordinary protobuf int64 wire encoding and can expand to ten bytes each; compare them with small positive varints.
- Large positive values expose the upper varint-width path without changing the field type.
- Mixed values approximate branch and payload-size variation in a live feed.
- Long and multibyte strings isolate length-delimited payload size and UTF-8 byte-count effects.


## CONCURRENCY_REPORT.md

# Concurrency and contention benchmark

> Protobuf encoding only. Each row contains one million total encodes per repetition across the configured threads.

## Run contract

- Configurations: `12`
- Total encodes per repetition: `1000000`
- Repetitions: `10`
- Thread counts: `1,2,4,8`

## Results

| Mode | Threads | Reps | Bytes | Median ns/encode | Mean ns/encode | Mean M/s |
|---|---:|---:|---:|---:|---:|---:|
| shared_buffer_mutex | 1 | 10 | 49 | 101.04 | 103.01 | 9.73 |
| shared_buffer_mutex | 2 | 10 | 49 | 101.76 | 105.78 | 9.51 |
| shared_buffer_mutex | 4 | 10 | 49 | 101.65 | 103.77 | 9.65 |
| shared_buffer_mutex | 8 | 10 | 49 | 102.62 | 103.07 | 9.70 |
| shared_readonly | 1 | 10 | 49 | 88.16 | 90.26 | 11.13 |
| shared_readonly | 2 | 10 | 49 | 85.93 | 86.61 | 11.55 |
| shared_readonly | 4 | 10 | 49 | 86.61 | 86.86 | 11.52 |
| shared_readonly | 8 | 10 | 49 | 84.00 | 84.48 | 11.84 |
| thread_local | 1 | 10 | 49 | 100.44 | 109.51 | 9.44 |
| thread_local | 2 | 10 | 49 | 99.34 | 103.87 | 9.78 |
| thread_local | 4 | 10 | 49 | 97.29 | 101.93 | 10.05 |
| thread_local | 8 | 10 | 49 | 94.16 | 99.16 | 10.28 |

## Mode definitions

- `thread_local`: each worker has its own protobuf message and output buffer.
- `shared_readonly`: workers serialize the same immutable message into independent buffers.
- `shared_buffer_mutex`: workers serialize into one shared buffer under a mutex, exposing lock contention.
- Thread creation and barrier setup are outside the timed encode loop.


## PERF_REPORT.md

# Hardware-counter benchmark

> Dockerized `perf stat` around the exact-decimal representation benchmark. No decoding work is included.

## Run contract

- Encodes per repetition: `1000000`
- Repetitions: `10`
- perf exit status: `127`
- Events requested: cycles, instructions, cache references, cache misses, branches, and branch misses.

## Raw perf output

```text
perf output missing
```

If events are unavailable under OrbStack, the output is retained explicitly rather than being presented as a valid counter measurement. Repeat this phase on production Linux hardware with the required perf permissions for authoritative microarchitectural comparisons.


## KAFKA_PRODUCER_REPORT.md

# Kafka producer benchmark

> Encoding plus real librdkafka producer handoff. No decoding benchmark is included.

## Run contract

- Configurations: `5`
- Encodes/messages per repetition: `1000000`
- Repetitions per configuration: `10`
- `enqueue` covers serialization plus the librdkafka `produce()` handoff.
- `flush` covers the remaining producer/broker delivery time for the configured acknowledgement mode.
- `end_to_end` is enqueue plus flush and is the relevant result for this producer-path benchmark.

## Results

| Mode | Acks | Compression | Linger ms | Batch messages | Reps | Bytes | Median enqueue ns/msg | Median end-to-end ns/msg | p95 end-to-end ns/msg | p99 end-to-end ns/msg | Errors |
|---|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| copy | 0 | lz4 | 0 | 1 | 10 | 49 | 46,172.77 | 50,711.85 | 93,854.69 | 93,854.69 | 0 |
| copy | 0 | none | 0 | 1 | 10 | 49 | 40,755.03 | 45,293.98 | 72,086.57 | 72,086.57 | 0 |
| copy | 0 | zstd | 0 | 1 | 10 | 49 | 47,813.85 | 52,674.56 | 113,391.02 | 113,391.02 | 0 |
| copy | 1 | none | 5 | 100 | 10 | 49 | 594.65 | 666.56 | 2,896.48 | 2,896.48 | 0 |
| owned | 1 | none | 5 | 100 | 10 | 49 | 670.03 | 746.25 | 4,049.69 | 4,049.69 | 0 |

## Interpretation

- Compare `copy` and `owned` carefully: the owned path transfers an allocated payload to librdkafka, while the copy path reuses a caller-owned buffer and asks librdkafka to copy it.
- `acks=0` measures producer handoff with no broker acknowledgement guarantee; it must not be treated as durable delivery.
- Compression and batching can reduce wire bytes at the cost of producer CPU and latency. The correct choice depends on the latency budget and broker/network constraints.
- This report measures producer handoff and flush behavior, not consumer throughput or decoding.

## Raw data

Every configuration has one CSV file with all repetitions. The raw rows include enqueue, flush, end-to-end elapsed time, delivery errors, and configuration fields.


## SCHEMA_REGISTRY_REPORT.md

# Confluent Schema Registry and Kafka framing benchmark

> Detailed report: pure protobuf encoding, cached Confluent framing, allocation/copy variants, and live Schema Registry HTTP paths.

## Executive verdict

For an HFT producer, Schema Registry is acceptable only when the schema ID is acquired before the hot path and cached locally. The steady-state framing work is a fixed byte-prefix operation; registration and network lookups are separate millisecond-scale control-plane operations and must not occur per message.

The benchmark uses a six-byte Protobuf Confluent prefix for a single top-level message: one magic byte, four big-endian schema-ID bytes, and the one-byte message-index encoding for index zero. The exact framing is included in the measured output size.

## Run contract

- Steady-state encodes per repetition: `1000000`
- Steady-state repetitions: `10`
- Excluded warmup encodes: `10000`
- Cold/control-plane repetitions: `10` requests per path
- Cold/control-plane paths intentionally use one HTTP request per repetition; sending one million live registration requests would benchmark Registry stress and storage behavior, not a production encode path.
- All work runs in Docker; the Registry is Confluent Schema Registry backed by Confluent Kafka in single-node KRaft mode.
- Decimal values remain exact strings in both protobuf and JSON payloads.

## Steady-state result versus pure protobuf

The pure baseline is Google protobuf generated with Buf using `SPEED` and preallocated `SerializeToArray`, matching the message implementation used by the Registry framing executable.

| Payload | Pure protobuf ns | In-place framed ns | Framed + copy ns | Cached serializer string ns | Framed bytes | In-place overhead | Copy overhead |
|---|---:|---:|---:|---:|---:|---:|---:|
| one_string_ten_int64 | 82.47 | 83.41 | 85.56 | 90.82 | 55 | 1.15% | 3.75% |
| one_string_ten_decimal_strings | 148.72 | 147.71 | 151.41 | 156.85 | 219 | -0.68% | 1.81% |
| ten_strings_fifty_decimal_strings | 606.25 | 597.42 | 713.14 | 738.36 | 1199 | -1.46% | 17.63% |

## All cached paths

| Payload | API | Reps | Framed bytes | Median ns/encode | Mean M/s | p95 ns |
|---|---|---:|---:|---:|---:|---:|
| one_string_ten_decimal_strings | cached_id_framed_in_place | 10 | 219 | 147.71 | 6,767,525.35 | 148.55 |
| one_string_ten_decimal_strings | cached_id_framed_with_copy | 10 | 219 | 151.41 | 6,483,116.67 | 170.68 |
| one_string_ten_decimal_strings | cached_serializer_string | 10 | 219 | 156.85 | 6,291,349.90 | 164.75 |
| one_string_ten_int64 | cached_id_framed_in_place | 10 | 55 | 83.41 | 11,858,457.45 | 87.67 |
| one_string_ten_int64 | cached_id_framed_with_copy | 10 | 55 | 85.56 | 11,681,656.18 | 86.66 |
| one_string_ten_int64 | cached_serializer_string | 10 | 55 | 90.82 | 10,993,311.67 | 91.97 |
| ten_strings_fifty_decimal_strings | cached_id_framed_in_place | 10 | 1199 | 597.42 | 1,655,487.16 | 657.37 |
| ten_strings_fifty_decimal_strings | cached_id_framed_with_copy | 10 | 1199 | 713.14 | 1,167,175.75 | 2,046.86 |
| ten_strings_fifty_decimal_strings | cached_serializer_string | 10 | 1199 | 738.36 | 1,360,382.82 | 837.38 |

## Live Schema Registry paths

These measurements include HTTP request/response and Registry processing. `registry_lookup_keepalive` means a keep-alive HTTP lookup; it is not a local schema-ID cache hit. The local cached-ID paths are the steady-state framing rows above. All live rows are control-plane latency measurements, not per-message serializer benchmarks.

| Path | Reps | Status/result | Median ns | Mean ns | p95 ns | Min ns | Max ns | Requests/sec |
|---|---:|---|---:|---:|---:|---:|---:|---:|
| cache_miss_404 | 10 | 404 | 1,940,320.00 | 2,671,896.60 | 8,158,831.00 | 1,712,335.00 | 8,158,831.00 | 374.27 |
| cold_lookup_new_connection | 10 | 1 | 2,569,377.50 | 2,605,415.90 | 3,270,499.00 | 2,139,721.00 | 3,270,499.00 | 383.82 |
| cold_register | 10 | 1 | 15,134,361.50 | 15,345,171.40 | 17,450,067.00 | 13,666,637.00 | 17,450,067.00 | 65.17 |
| concurrent_registration | 10 | 200 | 86,461,144.50 | 88,113,427.70 | 144,041,267.00 | 36,821,014.00 | 144,041,267.00 | 11.35 |
| registry_lookup_keepalive | 10 | 1 | 2,654,525.50 | 3,506,442.80 | 11,001,362.00 | 1,638,208.00 | 11,001,362.00 | 285.19 |
| registry_unavailable | 10 | 0 | 203,380.00 | 240,097.80 | 603,974.00 | 158,296.00 | 603,974.00 | 4,164.97 |
| retry_failure_then_success | 10 | 200 | 2,204,785.00 | 2,482,200.40 | 4,500,239.00 | 1,938,633.00 | 4,500,239.00 | 402.87 |

## HFT interpretation

1. Register or resolve the schema ID during process startup, deployment, or a controlled recovery path.
2. Keep the schema ID in an immutable/read-mostly local cache. A cache miss must fail closed or use an explicitly non-HFT recovery path, not synchronously call Registry from the producer hot loop.
3. Prefer serializing directly into a buffer with reserved prefix space. That avoids a second payload copy and makes the six-byte framing cost visible and bounded.
4. Treat serializer-string paths as a convenience path, not the default HFT path; their allocation/copy behavior is visible in the cached-path table.
5. Registry availability is still operationally important even when it is absent from the hot path: startup, schema rollout, failover, and cache invalidation need timeouts, metrics, and a tested fallback policy.
6. Kafka compression, batching, broker acknowledgements, and network transport remain outside this benchmark and should be tested separately once the serializer/framing choice is fixed.

## Scope boundary

The companion `REPORT.md` contains the complete pure protobuf/JSON library matrix, including every Buf-generated protobuf variant and every JSON library. This Registry report intentionally uses the matching Google protobuf `SPEED` type for its wire-framing and Registry-path comparison; changing generated code and changing Registry integration at the same time would make the framing result harder to attribute.

## Reproducibility and raw data

- `schema_registry_raw.csv` contains every cached-path repetition and every measured HTTP request.
- The existing `raw.csv` is the pure protobuf/JSON baseline used for the comparison.
- Re-run on an isolated CPU when comparing small differences; CPU frequency, emulation mode, allocator state, and container host scheduling affect absolute values.


## SCHEMA_REGISTRY_SECURE_REPORT.md

# Confluent Schema Registry and Kafka framing benchmark

> Detailed report: pure protobuf encoding, cached Confluent framing, allocation/copy variants, and live Schema Registry HTTP paths.

## Executive verdict

For an HFT producer, Schema Registry is acceptable only when the schema ID is acquired before the hot path and cached locally. The steady-state framing work is a fixed byte-prefix operation; registration and network lookups are separate millisecond-scale control-plane operations and must not occur per message.

The benchmark uses a six-byte Protobuf Confluent prefix for a single top-level message: one magic byte, four big-endian schema-ID bytes, and the one-byte message-index encoding for index zero. The exact framing is included in the measured output size.

## Run contract

- Steady-state encodes per repetition: `1000000`
- Steady-state repetitions: `10`
- Excluded warmup encodes: `10000`
- Cold/control-plane repetitions: `10` requests per path
- Cold/control-plane paths intentionally use one HTTP request per repetition; sending one million live registration requests would benchmark Registry stress and storage behavior, not a production encode path.
- All work runs in Docker; the Registry is Confluent Schema Registry backed by Confluent Kafka in single-node KRaft mode.
- Decimal values remain exact strings in both protobuf and JSON payloads.

## Steady-state result versus pure protobuf

The pure baseline is Google protobuf generated with Buf using `SPEED` and preallocated `SerializeToArray`, matching the message implementation used by the Registry framing executable.

| Payload | Pure protobuf ns | In-place framed ns | Framed + copy ns | Cached serializer string ns | Framed bytes | In-place overhead | Copy overhead |
|---|---:|---:|---:|---:|---:|---:|---:|
| one_string_ten_int64 | 82.47 | 82.81 | 85.60 | 91.97 | 55 | 0.42% | 3.81% |
| one_string_ten_decimal_strings | 148.72 | 150.11 | 154.01 | 157.25 | 219 | 0.93% | 3.55% |
| ten_strings_fifty_decimal_strings | 606.25 | 598.85 | 615.76 | 639.88 | 1199 | -1.22% | 1.57% |

## All cached paths

| Payload | API | Reps | Framed bytes | Median ns/encode | Mean M/s | p95 ns |
|---|---|---:|---:|---:|---:|---:|
| one_string_ten_decimal_strings | cached_id_framed_in_place | 10 | 219 | 150.11 | 6,416,706.02 | 185.72 |
| one_string_ten_decimal_strings | cached_id_framed_with_copy | 10 | 219 | 154.01 | 6,403,819.75 | 170.09 |
| one_string_ten_decimal_strings | cached_serializer_string | 10 | 219 | 157.25 | 6,354,330.16 | 158.35 |
| one_string_ten_int64 | cached_id_framed_in_place | 10 | 55 | 82.81 | 11,930,810.84 | 91.78 |
| one_string_ten_int64 | cached_id_framed_with_copy | 10 | 55 | 85.60 | 11,608,483.02 | 89.08 |
| one_string_ten_int64 | cached_serializer_string | 10 | 55 | 91.97 | 10,740,896.28 | 99.91 |
| ten_strings_fifty_decimal_strings | cached_id_framed_in_place | 10 | 1199 | 598.85 | 1,661,444.47 | 628.00 |
| ten_strings_fifty_decimal_strings | cached_id_framed_with_copy | 10 | 1199 | 615.76 | 1,613,364.86 | 652.91 |
| ten_strings_fifty_decimal_strings | cached_serializer_string | 10 | 1199 | 639.88 | 1,559,939.75 | 671.69 |

## Live Schema Registry paths

These measurements include HTTP request/response and Registry processing. `registry_lookup_keepalive` means a keep-alive HTTP lookup; it is not a local schema-ID cache hit. The local cached-ID paths are the steady-state framing rows above. All live rows are control-plane latency measurements, not per-message serializer benchmarks.

| Path | Reps | Status/result | Median ns | Mean ns | p95 ns | Min ns | Max ns | Requests/sec |
|---|---:|---|---:|---:|---:|---:|---:|---:|
| cache_miss_404 | 10 | 404 | 8,605,383.00 | 9,385,837.20 | 14,284,949.00 | 7,043,637.00 | 14,284,949.00 | 106.54 |
| cold_lookup_new_connection | 10 | 1 | 9,630,900.50 | 10,152,786.60 | 14,165,695.00 | 8,293,726.00 | 14,165,695.00 | 98.50 |
| cold_register | 10 | 1 | 17,407,461.00 | 17,537,070.20 | 24,527,372.00 | 14,606,836.00 | 24,527,372.00 | 57.02 |
| concurrent_registration | 10 | 200 | 103,171,448.00 | 100,147,777.50 | 163,825,730.00 | 40,981,152.00 | 163,825,730.00 | 9.99 |
| registry_lookup_keepalive | 10 | 1 | 3,858,081.00 | 6,444,232.00 | 28,703,278.00 | 2,654,933.00 | 28,703,278.00 | 155.18 |
| registry_unavailable | 10 | 0 | 165,214.50 | 286,914.90 | 1,031,372.00 | 134,589.00 | 1,031,372.00 | 3,485.35 |
| retry_failure_then_success | 10 | 200 | 7,746,330.00 | 7,720,654.30 | 9,870,868.00 | 6,567,078.00 | 9,870,868.00 | 129.52 |

## HFT interpretation

1. Register or resolve the schema ID during process startup, deployment, or a controlled recovery path.
2. Keep the schema ID in an immutable/read-mostly local cache. A cache miss must fail closed or use an explicitly non-HFT recovery path, not synchronously call Registry from the producer hot loop.
3. Prefer serializing directly into a buffer with reserved prefix space. That avoids a second payload copy and makes the six-byte framing cost visible and bounded.
4. Treat serializer-string paths as a convenience path, not the default HFT path; their allocation/copy behavior is visible in the cached-path table.
5. Registry availability is still operationally important even when it is absent from the hot path: startup, schema rollout, failover, and cache invalidation need timeouts, metrics, and a tested fallback policy.
6. Kafka compression, batching, broker acknowledgements, and network transport remain outside this benchmark and should be tested separately once the serializer/framing choice is fixed.

## Scope boundary

The companion `REPORT.md` contains the complete pure protobuf/JSON library matrix, including every Buf-generated protobuf variant and every JSON library. This Registry report intentionally uses the matching Google protobuf `SPEED` type for its wire-framing and Registry-path comparison; changing generated code and changing Registry integration at the same time would make the framing result harder to attribute.

## Reproducibility and raw data

- `schema_registry_raw.csv` contains every cached-path repetition and every measured HTTP request.
- The existing `raw.csv` is the pure protobuf/JSON baseline used for the comparison.
- Re-run on an isolated CPU when comparing small differences; CPU frequency, emulation mode, allocator state, and container host scheduling affect absolute values.


## SCHEMA_EVOLUTION_REPORT.md

# Schema Registry evolution benchmark

> Control-plane schema evolution only. No decoding or per-message network path is included.

## Run contract

- Repetitions: `10`
- Each repetition registers v1, registers v2 under the same subject, then lists the subject versions.
- v2 adds `venue` while preserving v1 fields and the same fully-qualified Protobuf message name.

## Results

| Phase | Reps | Status codes | Median ns | Mean ns | p95 ns | Response bytes |
|---|---:|---|---:|---:|---:|---:|
| lookup_versions | 10 | 200 | 4,377,601.00 | 6,922,328.70 | 30,456,010.00 | 5.0 |
| register_v1 | 10 | 200 | 26,430,026.50 | 45,235,710.50 | 227,148,755.00 | 2400.0 |
| register_v2 | 10 | 200 | 25,443,927.00 | 37,029,063.10 | 133,458,973.00 | 2422.0 |

## Interpretation

- A successful v2 registration under the v1 subject demonstrates the tested Registry compatibility path for this additive schema change.
- This does not replace a full compatibility-policy matrix; production rollout should also test breaking changes and the configured BACKWARD/FORWARD/FULL policy explicitly.


## Toolchain and host metadata

### toolchain_versions.txt

```text
architecture=aarch64
kernel=7.0.11-orbstack-00360-gc9bc4d96ac70
compiler=c++ (Ubuntu 15.2.0-16ubuntu1) 15.2.0
cmake=cmake version 4.4.2
buf=1.72.0
protoc=libprotoc 35.0
python=Python 3.14.4
protobuf_package=not-installed
protobuf_c_package=1.5.1-1ubuntu2
boost_json_package=1.83.0-5ubuntu5
jsoncpp_package=1.9.6-5
rapidjson_package=1.1.0+dfsg2-7.6ubuntu1
librdkafka=2.8.0
yyjson=0.12.0
nlohmann_json=3.12.0
```

### docker-host.txt

```text
Darwin Brunos-Mac-mini.local 25.6.0 Darwin Kernel Version 25.6.0: Sat Jul 11 15:24:35 PDT 2026; root:xnu-12377.161.13~4/RELEASE_ARM64_T8103 arm64
```

### docker-version.txt

```text
Client:
 Version:           29.4.0
 API version:       1.54
 Go version:        go1.26.3
 Git commit:        9d7ad9f
 Built:             Thu Jun  4 11:19:31 2026
 OS/Arch:           darwin/arm64
 Context:           orbstack

Server: Docker Engine - Community
 Engine:
  Version:          29.4.0
  API version:      1.54 (minimum version 1.40)
  Go version:       go1.26.1
  Git commit:       daa0cb7f
  Built:            Tue Apr  7 08:35:43 2026
  OS/Arch:          linux/arm64
  Experimental:     true
 containerd:
  Version:          v2.2.2
  GitCommit:        301b2dac98f15c27117da5c8af12118a041a31d9
 runc:
  Version:          1.4.2
  GitCommit:        c241c0bb5e60a8e8c1b2e53d4eca8d0068d8d57e
 docker-init:
  Version:          0.19.0
  GitCommit:        de40ad0
```

### docker-info.txt

```text
Client:
 Version:    29.4.0
 Context:    orbstack
 Debug Mode: false
 Plugins:
  buildx: Docker Buildx (Docker Inc.)
    Version:  v0.33.0
    Path:     /Users/bruno/.docker/cli-plugins/docker-buildx
  compose: Docker Compose (Docker Inc.)
    Version:  v5.1.2
    Path:     /Users/bruno/.docker/cli-plugins/docker-compose
  dev: Docker Dev Environments (Docker Inc.)
    Version:  v0.0.3
    Path:     /usr/local/lib/docker/cli-plugins/docker-dev
  extension: Manages Docker extensions (Docker Inc.)
    Version:  v0.2.13
    Path:     /usr/local/lib/docker/cli-plugins/docker-extension
  sbom: View the packaged-based Software Bill Of Materials (SBOM) for an image (Anchore Inc.)
    Version:  0.6.0
    Path:     /usr/local/lib/docker/cli-plugins/docker-sbom
  scan: Docker Scan (Docker Inc.)
    Version:  v0.21.0
    Path:     /usr/local/lib/docker/cli-plugins/docker-scan

Server:
 Containers: 0
  Running: 0
  Paused: 0
  Stopped: 0
 Images: 9
 Server Version: 29.4.0
 Storage Driver: overlayfs
  driver-type: io.containerd.snapshotter.v1
 Logging Driver: json-file
 Cgroup Driver: cgroupfs
 Cgroup Version: 2
 Plugins:
  Volume: local
  Network: bridge host ipvlan macvlan null overlay
  Log: awslogs fluentd gcplogs gelf journald json-file local splunk syslog
 CDI spec directories:
  /etc/cdi
  /var/run/cdi
 Swarm: inactive
 Runtimes: io.containerd.runc.v2 runc
 Default Runtime: runc
 Init Binary: docker-init
 containerd version: 301b2dac98f15c27117da5c8af12118a041a31d9
 runc version: c241c0bb5e60a8e8c1b2e53d4eca8d0068d8d57e
 init version: de40ad0
 Security Options:
  seccomp
   Profile: builtin
  cgroupns
 Kernel Version: 7.0.11-orbstack-00360-gc9bc4d96ac70
 Operating System: OrbStack
 OSType: linux
 Architecture: aarch64
 CPUs: 8
 Total Memory: 7.818GiB
 Name: orbstack
 ID: b2bcb197-2ded-4246-9b9d-ea26eddea79f
 Docker Root Dir: /var/lib/docker
 Debug Mode: false
 HTTP Proxy: http://proxy.orb.internal:8305
 HTTPS Proxy: http://proxy.orb.internal:8305
 No Proxy: localhost,127.0.0.1,127.0.0.0/8,::1,10.0.0.0/8,172.16.0.0/12,192.168.0.0/16,0.250.250.0/24,*.orb.internal,*.local,gateway.docker.internal,host.internal,host.docker.internal,host.lima.internal,docker.for.mac.localhost,docker.for.mac.host.internal
 Experimental: true
 Insecure Registries:
  127.0.0.0/8
  ::1/128
 Live Restore Enabled: false
 Product License: Community Engine
 Default Address Pools:
   Base: 192.168.97.0/24, Size: 24
   Base: 192.168.107.0/24, Size: 24
   Base: 192.168.117.0/24, Size: 24
   Base: 192.168.147.0/24, Size: 24
   Base: 192.168.148.0/24, Size: 24
   Base: 192.168.155.0/24, Size: 24
   Base: 192.168.156.0/24, Size: 24
   Base: 192.168.158.0/24, Size: 24
   Base: 192.168.163.0/24, Size: 24
   Base: 192.168.164.0/24, Size: 24
   Base: 192.168.165.0/24, Size: 24
   Base: 192.168.166.0/24, Size: 24
   Base: 192.168.167.0/24, Size: 24
   Base: 192.168.171.0/24, Size: 24
   Base: 192.168.172.0/24, Size: 24
   Base: 192.168.181.0/24, Size: 24
   Base: 192.168.183.0/24, Size: 24
   Base: 192.168.186.0/24, Size: 24
   Base: 192.168.207.0/24, Size: 24
   Base: 192.168.214.0/24, Size: 24
   Base: 192.168.215.0/24, Size: 24
   Base: 192.168.216.0/24, Size: 24
   Base: 192.168.223.0/24, Size: 24
   Base: 192.168.227.0/24, Size: 24
   Base: 192.168.228.0/24, Size: 24
   Base: 192.168.229.0/24, Size: 24
   Base: 192.168.237.0/24, Size: 24
   Base: 192.168.239.0/24, Size: 24
   Base: 192.168.242.0/24, Size: 24
   Base: 192.168.247.0/24, Size: 24
   Base: fd07:b51a:cc66:d000::/56, Size: 64
 Firewall Backend: iptables
```

## Raw artifacts

The accompanying workflow artifact contains the raw CSV files, metadata, Docker version output, and this report. CSV files are retained for statistical re-analysis; the tables above are the human-readable snapshot committed or uploaded for review.

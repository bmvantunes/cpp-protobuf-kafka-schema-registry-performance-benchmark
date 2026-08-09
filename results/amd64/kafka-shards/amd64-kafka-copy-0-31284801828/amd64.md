# C++ Kafka serialization benchmark — amd64

> Encoding only. No decode, parse, consumer, or deserialization benchmark is included.

## Run identity

- Requested architecture: `amd64`
- Observed machine: `Linux-6.17.0-1020-azure-x86_64-with-glibc2.39`
- Observed machine architecture: `x86_64`
- Python: `3.12.3`
- Steady-state contract: `1,000,000` encodes per measured repetition and `10` measured repetitions.
- Control-plane Registry paths intentionally use ten live requests per path; they are not hot-loop encode measurements.
- Every phase was executed through Docker; absolute values are host- and scheduler-dependent.

## Phase index

- [REPORT.md](amd64/REPORT.md)
- [REPRESENTATION_REPORT.md](amd64/REPRESENTATION_REPORT.md)
- [LATENCY_REPORT.md](amd64/LATENCY_REPORT.md)
- [DISTRIBUTION_REPORT.md](amd64/DISTRIBUTION_REPORT.md)
- [CONCURRENCY_REPORT.md](amd64/CONCURRENCY_REPORT.md)
- [PERF_REPORT.md](amd64/PERF_REPORT.md)
- [KAFKA_PRODUCER_REPORT.md](amd64/KAFKA_PRODUCER_REPORT.md)
- [SCHEMA_REGISTRY_REPORT.md](amd64/SCHEMA_REGISTRY_REPORT.md)
- [SCHEMA_REGISTRY_SECURE_REPORT.md](amd64/SCHEMA_REGISTRY_SECURE_REPORT.md)
- [SCHEMA_EVOLUTION_REPORT.md](amd64/SCHEMA_EVOLUTION_REPORT.md)

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
| one_string_ten_decimal_strings | protobuf_c / c_generated / pack_preallocated | 102.86 | 9.73 M/s | 213 |
| one_string_ten_int64 | google_protobuf / speed / SerializeToArray_preallocated | 56.13 | 17.78 M/s | 49 |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated / pack_preallocated | 623.04 | 1.60 M/s | 1193 |

## Full aggregate results

| Kind | Library | Codegen | API | Payload | Reps | Bytes | Median ns/encode | Mean ns/encode | Mean M/s |
|---|---|---|---|---|---:|---:|---:|---:|---:|
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 213 | 102.86 | 102.79 | 9.73 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 161.00 | 160.63 | 6.23 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 161.64 | 163.88 | 6.11 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 165.71 | 165.53 | 6.04 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 168.59 | 168.25 | 5.94 |
| json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 364 | 272.21 | 271.94 | 3.68 |
| json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 419 | 276.36 | 276.72 | 3.61 |
| json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 364 | 889.61 | 890.00 | 1.12 |
| json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 364 | 1,051.49 | 1,051.83 | 0.95 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 1,683.87 | 1,683.48 | 0.59 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 1,690.25 | 1,689.29 | 0.59 |
| json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 364 | 2,520.56 | 2,520.63 | 0.40 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 56.13 | 56.27 | 17.78 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 59.59 | 59.35 | 16.85 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 49 | 64.47 | 64.58 | 15.49 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 49 | 66.45 | 66.43 | 15.06 |
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 49 | 80.94 | 81.25 | 12.31 |
| json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 215 | 124.69 | 124.80 | 8.01 |
| json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 210 | 343.81 | 343.88 | 2.91 |
| json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 210 | 443.96 | 444.14 | 2.25 |
| json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 210 | 563.18 | 563.29 | 1.78 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 49 | 1,375.39 | 1,376.55 | 0.73 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 1,378.07 | 1,378.08 | 0.73 |
| json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 210 | 2,375.79 | 2,374.96 | 0.42 |
| protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 623.04 | 623.56 | 1.60 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 904.73 | 904.58 | 1.11 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 906.21 | 906.63 | 1.10 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 923.97 | 925.53 | 1.08 |
| protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 924.16 | 922.36 | 1.08 |
| json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 2279 | 1,203.38 | 1,204.22 | 0.83 |
| json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1979 | 1,406.87 | 1,407.96 | 0.71 |
| json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 1979 | 4,748.27 | 4,747.51 | 0.21 |
| json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 1979 | 5,504.40 | 5,507.53 | 0.18 |
| protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 8,639.62 | 8,648.08 | 0.12 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 8,658.39 | 8,671.60 | 0.12 |
| json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 1979 | 12,631.27 | 12,619.84 | 0.08 |

## Protobuf-only comparison

| Payload | Library / generation | API | Median ns/encode | Relative to fastest protobuf |
|---|---|---|---:|---:|
| one_string_ten_decimal_strings | protobuf_c / c_generated | pack_preallocated | 102.86 | 1.00x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 161.00 | 1.57x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 161.64 | 1.57x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToString | 165.71 | 1.61x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 168.59 | 1.64x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 1,683.87 | 16.37x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToString | 1,690.25 | 16.43x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToArray_preallocated | 56.13 | 1.00x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToArray_preallocated | 59.59 | 1.06x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToString | 64.47 | 1.15x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToString | 66.45 | 1.18x |
| one_string_ten_int64 | protobuf_c / c_generated | pack_preallocated | 80.94 | 1.44x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToString | 1,375.39 | 24.50x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToArray_preallocated | 1,378.07 | 24.55x |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated | pack_preallocated | 623.04 | 1.00x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 904.73 | 1.45x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 906.21 | 1.45x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 923.97 | 1.48x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToString | 924.16 | 1.48x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToString | 8,639.62 | 13.87x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 8,658.39 | 13.90x |

## JSON-only comparison

| Payload | Library | Median ns/encode | Relative to fastest JSON |
|---|---|---:|---:|
| one_string_ten_decimal_strings | boost_json | 272.21 | 1.00x |
| one_string_ten_decimal_strings | yyjson | 276.36 | 1.02x |
| one_string_ten_decimal_strings | rapidjson | 889.61 | 3.27x |
| one_string_ten_decimal_strings | nlohmann_json | 1,051.49 | 3.86x |
| one_string_ten_decimal_strings | jsoncpp | 2,520.56 | 9.26x |
| one_string_ten_int64 | yyjson | 124.69 | 1.00x |
| one_string_ten_int64 | boost_json | 343.81 | 2.76x |
| one_string_ten_int64 | rapidjson | 443.96 | 3.56x |
| one_string_ten_int64 | nlohmann_json | 563.18 | 4.52x |
| one_string_ten_int64 | jsoncpp | 2,375.79 | 19.05x |
| ten_strings_fifty_decimal_strings | yyjson | 1,203.38 | 1.00x |
| ten_strings_fifty_decimal_strings | boost_json | 1,406.87 | 1.17x |
| ten_strings_fifty_decimal_strings | rapidjson | 4,748.27 | 3.95x |
| ten_strings_fifty_decimal_strings | nlohmann_json | 5,504.40 | 4.57x |
| ten_strings_fifty_decimal_strings | jsoncpp | 12,631.27 | 10.50x |

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
| fixed64_scale_8 | 10 | 99 | 27.33 | 27.13 | 28.01 | 28.01 | 28.01 | 28.01 | 36.86 |
| bytes_128bit_coefficient_scale_8 | 10 | 189 | 57.83 | 57.76 | 58.07 | 58.07 | 58.07 | 58.07 | 17.31 |
| scaled_sint64_scale_8 | 10 | 79 | 68.35 | 68.46 | 69.03 | 69.03 | 69.03 | 69.03 | 14.61 |
| decimal_string | 10 | 169 | 179.61 | 179.49 | 180.66 | 180.66 | 180.66 | 180.66 | 5.57 |

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
| one_string_ten_decimal_strings | serialize_array_arena_message | 10 | 213 | 176.00 | 181.00 | 186.50 | 243.00 | 6,281.00 | 25,160.50 | 0.00 | 0.00 |
| one_string_ten_decimal_strings | serialize_array_fresh_buffer | 10 | 213 | 186.00 | 192.00 | 197.50 | 302.00 | 6,238.00 | 24,708.00 | 1,000,000.00 | 213,000,000.00 |
| one_string_ten_decimal_strings | serialize_array_reuse | 10 | 213 | 177.00 | 180.50 | 186.50 | 247.50 | 6,259.50 | 34,590.50 | 0.00 | 0.00 |
| one_string_ten_decimal_strings | serialize_string_reserved | 10 | 213 | 182.00 | 187.00 | 195.50 | 272.00 | 6,403.50 | 27,227.50 | 0.00 | 0.00 |
| one_string_ten_int64 | serialize_array_arena_message | 10 | 49 | 74.00 | 78.00 | 81.00 | 105.50 | 718.00 | 22,060.00 | 0.00 | 0.00 |
| one_string_ten_int64 | serialize_array_fresh_buffer | 10 | 49 | 85.00 | 89.00 | 92.00 | 183.00 | 1,612.00 | 14,737.00 | 1,000,000.00 | 49,000,000.00 |
| one_string_ten_int64 | serialize_array_reuse | 10 | 49 | 75.50 | 80.00 | 82.00 | 104.50 | 693.00 | 20,657.50 | 0.00 | 0.00 |
| one_string_ten_int64 | serialize_string_reserved | 10 | 49 | 80.00 | 85.00 | 88.50 | 152.50 | 1,372.00 | 23,240.50 | 0.00 | 0.00 |
| ten_strings_fifty_decimal_strings | serialize_array_arena_message | 10 | 1193 | 924.00 | 950.00 | 974.00 | 3,041.00 | 8,381.50 | 73,066.00 | 0.00 | 0.00 |
| ten_strings_fifty_decimal_strings | serialize_array_fresh_buffer | 10 | 1193 | 983.50 | 1,009.00 | 1,055.50 | 6,515.00 | 8,185.50 | 66,064.50 | 1,000,000.00 | 1,193,000,000.00 |
| ten_strings_fifty_decimal_strings | serialize_array_reuse | 10 | 1193 | 921.00 | 953.00 | 982.50 | 4,867.50 | 9,081.50 | 65,092.50 | 0.00 | 0.00 |
| ten_strings_fifty_decimal_strings | serialize_string_reserved | 10 | 1193 | 937.50 | 963.00 | 994.50 | 3,324.50 | 8,631.00 | 61,394.50 | 0.00 | 0.00 |

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
| int64_small_varints | 10 | 29.00 | 55.57 | 55.33 | 18.08 |
| string_short_ascii | 10 | 49.00 | 62.42 | 62.28 | 16.06 |
| string_long_ascii_256 | 10 | 299.00 | 74.17 | 74.07 | 13.50 |
| int64_mixed_distribution | 10 | 76.50 | 80.32 | 80.40 | 12.44 |
| string_multibyte_utf8 | 10 | 67.92 | 89.53 | 90.02 | 11.11 |
| int64_negative_standard_varint | 10 | 119.00 | 101.16 | 101.40 | 9.86 |
| int64_large_varints | 10 | 109.00 | 101.78 | 101.94 | 9.81 |

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
| shared_buffer_mutex | 1 | 10 | 49 | 69.84 | 69.71 | 14.35 |
| shared_buffer_mutex | 2 | 10 | 49 | 392.15 | 389.84 | 2.57 |
| shared_buffer_mutex | 4 | 10 | 49 | 396.34 | 392.59 | 2.55 |
| shared_buffer_mutex | 8 | 10 | 49 | 392.21 | 392.36 | 2.55 |
| shared_readonly | 1 | 10 | 49 | 61.47 | 61.09 | 16.38 |
| shared_readonly | 2 | 10 | 49 | 46.23 | 46.23 | 21.63 |
| shared_readonly | 4 | 10 | 49 | 45.29 | 45.33 | 22.06 |
| shared_readonly | 8 | 10 | 49 | 45.34 | 45.35 | 22.05 |
| thread_local | 1 | 10 | 49 | 60.49 | 59.90 | 16.72 |
| thread_local | 2 | 10 | 49 | 43.90 | 43.88 | 22.79 |
| thread_local | 4 | 10 | 49 | 43.93 | 44.35 | 22.56 |
| thread_local | 8 | 10 | 49 | 44.23 | 44.29 | 22.58 |

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

- Configurations: `72`
- Encodes/messages per repetition: `1000000`
- Repetitions per configuration: `10`
- `enqueue` covers serialization plus the librdkafka `produce()` handoff.
- `flush` covers the remaining producer/broker delivery time for the configured acknowledgement mode.
- `end_to_end` is enqueue plus flush and is the relevant result for this producer-path benchmark.

## Results

| Mode | Acks | Compression | Linger ms | Batch messages | Reps | Bytes | Median enqueue ns/msg | Median end-to-end ns/msg | p95 end-to-end ns/msg | p99 end-to-end ns/msg | Errors |
|---|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| copy | 0 | lz4 | 0 | 1 | 10 | 49 | 41,629.88 | 46,258.35 | 46,887.61 | 46,887.61 | 0 |
| copy | 0 | lz4 | 0 | 100 | 10 | 49 | 754.05 | 755.18 | 1,747.56 | 1,747.56 | 0 |
| copy | 0 | lz4 | 5 | 1 | 10 | 49 | 41,304.02 | 45,885.84 | 46,761.23 | 46,761.23 | 0 |
| copy | 0 | lz4 | 5 | 100 | 10 | 49 | 700.09 | 701.36 | 1,711.20 | 1,711.20 | 0 |
| copy | 0 | none | 0 | 1 | 10 | 49 | 40,093.94 | 44,525.34 | 46,950.59 | 46,950.59 | 0 |
| copy | 0 | none | 0 | 100 | 10 | 49 | 735.34 | 742.09 | 1,749.44 | 1,749.44 | 0 |
| copy | 0 | none | 5 | 1 | 10 | 49 | 40,204.99 | 44,660.04 | 45,234.81 | 45,234.81 | 0 |
| copy | 0 | none | 5 | 100 | 10 | 49 | 709.94 | 713.13 | 1,672.63 | 1,672.63 | 0 |
| copy | 0 | zstd | 0 | 1 | 10 | 49 | 45,230.20 | 50,190.51 | 50,562.32 | 50,562.32 | 0 |
| copy | 0 | zstd | 0 | 100 | 10 | 49 | 937.71 | 1,015.43 | 2,057.16 | 2,057.16 | 0 |
| copy | 0 | zstd | 5 | 1 | 10 | 49 | 45,618.95 | 50,657.31 | 51,247.02 | 51,247.02 | 0 |
| copy | 0 | zstd | 5 | 100 | 10 | 49 | 1,002.78 | 1,088.69 | 2,070.39 | 2,070.39 | 0 |
| copy | 1 | lz4 | 0 | 1 | 10 | 49 | 51,638.60 | 57,380.82 | 59,207.05 | 59,207.05 | 0 |
| copy | 1 | lz4 | 0 | 100 | 10 | 49 | 1,023.18 | 1,122.31 | 2,080.13 | 2,080.13 | 0 |
| copy | 1 | lz4 | 5 | 1 | 10 | 49 | 51,344.52 | 57,139.50 | 58,057.81 | 58,057.81 | 0 |
| copy | 1 | lz4 | 5 | 100 | 10 | 49 | 827.73 | 829.38 | 1,926.44 | 1,926.44 | 0 |
| copy | 1 | none | 0 | 1 | 10 | 49 | 52,118.34 | 57,973.83 | 61,738.96 | 61,738.96 | 0 |
| copy | 1 | none | 0 | 100 | 10 | 49 | 937.08 | 956.49 | 2,043.65 | 2,043.65 | 0 |
| copy | 1 | none | 5 | 1 | 10 | 49 | 52,444.83 | 58,187.32 | 59,044.14 | 59,044.14 | 0 |
| copy | 1 | none | 5 | 100 | 10 | 49 | 831.13 | 832.50 | 1,973.46 | 1,973.46 | 0 |
| copy | 1 | zstd | 0 | 1 | 10 | 49 | 52,259.43 | 58,031.43 | 59,862.61 | 59,862.61 | 0 |
| copy | 1 | zstd | 0 | 100 | 10 | 49 | 1,034.47 | 1,107.25 | 2,220.23 | 2,220.23 | 0 |
| copy | 1 | zstd | 5 | 1 | 10 | 49 | 52,800.79 | 58,679.68 | 60,120.47 | 60,120.47 | 0 |
| copy | 1 | zstd | 5 | 100 | 10 | 49 | 1,173.98 | 1,264.62 | 2,261.29 | 2,261.29 | 0 |
| copy | all | lz4 | 0 | 1 | 10 | 49 | 91,648.56 | 101,819.15 | 102,756.60 | 102,756.60 | 0 |
| copy | all | lz4 | 0 | 100 | 10 | 49 | 1,092.96 | 1,231.76 | 2,980.60 | 2,980.60 | 0 |
| copy | all | lz4 | 5 | 1 | 10 | 49 | 91,684.72 | 101,871.43 | 102,349.32 | 102,349.32 | 0 |
| copy | all | lz4 | 5 | 100 | 10 | 49 | 949.19 | 1,057.68 | 2,090.54 | 2,090.54 | 0 |
| copy | all | none | 0 | 1 | 10 | 49 | 93,448.53 | 103,868.92 | 104,760.47 | 104,760.47 | 0 |
| copy | all | none | 0 | 100 | 10 | 49 | 1,046.61 | 1,161.62 | 2,513.18 | 2,513.18 | 0 |
| copy | all | none | 5 | 1 | 10 | 49 | 90,757.42 | 100,812.28 | 102,084.48 | 102,084.48 | 0 |
| copy | all | none | 5 | 100 | 10 | 49 | 1,019.66 | 1,131.92 | 2,186.83 | 2,186.83 | 0 |
| copy | all | zstd | 0 | 1 | 10 | 49 | 91,557.01 | 101,691.52 | 104,493.01 | 104,493.01 | 0 |
| copy | all | zstd | 0 | 100 | 10 | 49 | 1,168.72 | 1,290.00 | 2,392.55 | 2,392.55 | 0 |
| copy | all | zstd | 5 | 1 | 10 | 49 | 91,572.22 | 101,715.50 | 102,753.03 | 102,753.03 | 0 |
| copy | all | zstd | 5 | 100 | 10 | 49 | 1,318.15 | 1,439.49 | 2,398.78 | 2,398.78 | 0 |
| owned | 0 | lz4 | 0 | 1 | 10 | 49 | 42,532.98 | 47,271.20 | 47,655.31 | 47,655.31 | 0 |
| owned | 0 | lz4 | 0 | 100 | 10 | 49 | 842.94 | 843.54 | 1,772.53 | 1,772.53 | 0 |
| owned | 0 | lz4 | 5 | 1 | 10 | 49 | 42,689.16 | 47,384.44 | 47,901.73 | 47,901.73 | 0 |
| owned | 0 | lz4 | 5 | 100 | 10 | 49 | 725.19 | 725.93 | 1,707.30 | 1,707.30 | 0 |
| owned | 0 | none | 0 | 1 | 10 | 49 | 41,365.82 | 45,952.09 | 48,356.37 | 48,356.37 | 0 |
| owned | 0 | none | 0 | 100 | 10 | 49 | 629.74 | 674.71 | 1,919.16 | 1,919.16 | 0 |
| owned | 0 | none | 5 | 1 | 10 | 49 | 41,148.26 | 45,780.68 | 46,441.96 | 46,441.96 | 0 |
| owned | 0 | none | 5 | 100 | 10 | 49 | 743.27 | 743.92 | 1,662.93 | 1,662.93 | 0 |
| owned | 0 | zstd | 0 | 1 | 10 | 49 | 46,594.87 | 51,750.28 | 52,139.38 | 52,139.38 | 0 |
| owned | 0 | zstd | 0 | 100 | 10 | 49 | 976.41 | 1,061.38 | 2,110.68 | 2,110.68 | 0 |
| owned | 0 | zstd | 5 | 1 | 10 | 49 | 46,581.52 | 51,722.52 | 52,233.66 | 52,233.66 | 0 |
| owned | 0 | zstd | 5 | 100 | 10 | 49 | 990.00 | 1,074.82 | 2,059.08 | 2,059.08 | 0 |
| owned | 1 | lz4 | 0 | 1 | 10 | 49 | 65,741.31 | 73,029.17 | 74,086.36 | 74,086.36 | 0 |
| owned | 1 | lz4 | 0 | 100 | 10 | 49 | 766.57 | 837.95 | 2,044.67 | 2,044.67 | 0 |
| owned | 1 | lz4 | 5 | 1 | 10 | 49 | 66,202.05 | 73,543.47 | 74,043.83 | 74,043.83 | 0 |
| owned | 1 | lz4 | 5 | 100 | 10 | 49 | 811.78 | 859.68 | 1,891.06 | 1,891.06 | 0 |
| owned | 1 | none | 0 | 1 | 10 | 49 | 65,808.71 | 73,097.03 | 76,804.50 | 76,804.50 | 0 |
| owned | 1 | none | 0 | 100 | 10 | 49 | 849.80 | 933.10 | 2,431.78 | 2,431.78 | 0 |
| owned | 1 | none | 5 | 1 | 10 | 49 | 65,945.18 | 73,226.65 | 74,144.09 | 74,144.09 | 0 |
| owned | 1 | none | 5 | 100 | 10 | 49 | 784.79 | 860.70 | 1,874.02 | 1,874.02 | 0 |
| owned | 1 | zstd | 0 | 1 | 10 | 49 | 70,727.42 | 78,550.09 | 79,934.25 | 79,934.25 | 0 |
| owned | 1 | zstd | 0 | 100 | 10 | 49 | 1,104.00 | 1,205.05 | 2,300.14 | 2,300.14 | 0 |
| owned | 1 | zstd | 5 | 1 | 10 | 49 | 70,284.77 | 78,048.89 | 82,140.47 | 82,140.47 | 0 |
| owned | 1 | zstd | 5 | 100 | 10 | 49 | 1,271.39 | 1,375.58 | 2,388.67 | 2,388.67 | 0 |
| owned | all | lz4 | 0 | 1 | 10 | 49 | 88,617.48 | 98,448.78 | 100,167.29 | 100,167.29 | 0 |
| owned | all | lz4 | 0 | 100 | 10 | 49 | 1,072.25 | 1,201.58 | 2,730.08 | 2,730.08 | 0 |
| owned | all | lz4 | 5 | 1 | 10 | 49 | 88,658.58 | 98,529.81 | 99,088.77 | 99,088.77 | 0 |
| owned | all | lz4 | 5 | 100 | 10 | 49 | 936.90 | 1,045.58 | 2,075.54 | 2,075.54 | 0 |
| owned | all | none | 0 | 1 | 10 | 49 | 90,565.72 | 100,599.80 | 103,554.14 | 103,554.14 | 0 |
| owned | all | none | 0 | 100 | 10 | 49 | 1,006.37 | 1,119.22 | 2,536.77 | 2,536.77 | 0 |
| owned | all | none | 5 | 1 | 10 | 49 | 88,333.05 | 98,157.04 | 99,070.87 | 99,070.87 | 0 |
| owned | all | none | 5 | 100 | 10 | 49 | 992.66 | 1,106.45 | 2,147.19 | 2,147.19 | 0 |
| owned | all | zstd | 0 | 1 | 10 | 49 | 91,599.34 | 101,760.01 | 102,576.21 | 102,576.21 | 0 |
| owned | all | zstd | 0 | 100 | 10 | 49 | 1,212.98 | 1,332.53 | 2,403.89 | 2,403.89 | 0 |
| owned | all | zstd | 5 | 1 | 10 | 49 | 90,129.53 | 100,107.88 | 102,515.85 | 102,515.85 | 0 |
| owned | all | zstd | 5 | 100 | 10 | 49 | 1,243.67 | 1,364.25 | 2,514.37 | 2,514.37 | 0 |

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
| one_string_ten_int64 | 56.13 | 57.17 | 59.18 | 65.14 | 55 | 1.85% | 5.44% |
| one_string_ten_decimal_strings | 161.64 | 164.46 | 168.76 | 177.55 | 219 | 1.74% | 4.40% |
| ten_strings_fifty_decimal_strings | 904.73 | 897.20 | 920.17 | 935.16 | 1199 | -0.83% | 1.71% |

## All cached paths

| Payload | API | Reps | Framed bytes | Median ns/encode | Mean M/s | p95 ns |
|---|---|---:|---:|---:|---:|---:|
| one_string_ten_decimal_strings | cached_id_framed_in_place | 10 | 219 | 164.46 | 6,054,687.15 | 170.20 |
| one_string_ten_decimal_strings | cached_id_framed_with_copy | 10 | 219 | 168.76 | 5,929,801.82 | 170.16 |
| one_string_ten_decimal_strings | cached_serializer_string | 10 | 219 | 177.55 | 5,637,296.35 | 179.09 |
| one_string_ten_int64 | cached_id_framed_in_place | 10 | 55 | 57.17 | 17,548,262.11 | 60.87 |
| one_string_ten_int64 | cached_id_framed_with_copy | 10 | 55 | 59.18 | 16,829,605.30 | 61.07 |
| one_string_ten_int64 | cached_serializer_string | 10 | 55 | 65.14 | 15,337,141.03 | 66.30 |
| ten_strings_fifty_decimal_strings | cached_id_framed_in_place | 10 | 1199 | 897.20 | 1,114,032.85 | 909.46 |
| ten_strings_fifty_decimal_strings | cached_id_framed_with_copy | 10 | 1199 | 920.17 | 1,086,569.61 | 929.36 |
| ten_strings_fifty_decimal_strings | cached_serializer_string | 10 | 1199 | 935.16 | 1,070,257.82 | 938.95 |

## Live Schema Registry paths

These measurements include HTTP request/response and Registry processing. `registry_lookup_keepalive` means a keep-alive HTTP lookup; it is not a local schema-ID cache hit. The local cached-ID paths are the steady-state framing rows above. All live rows are control-plane latency measurements, not per-message serializer benchmarks.

| Path | Reps | Status/result | Median ns | Mean ns | p95 ns | Min ns | Max ns | Requests/sec |
|---|---:|---|---:|---:|---:|---:|---:|---:|
| cache_miss_404 | 10 | 404 | 3,035,841.00 | 4,312,432.60 | 12,198,970.00 | 2,360,504.00 | 12,198,970.00 | 231.89 |
| cold_lookup_new_connection | 10 | 1 | 3,710,801.50 | 4,163,718.40 | 9,112,567.00 | 2,878,328.00 | 9,112,567.00 | 240.17 |
| cold_register | 10 | 1 | 17,757,481.00 | 18,455,193.30 | 27,027,036.00 | 14,254,120.00 | 27,027,036.00 | 54.19 |
| concurrent_registration | 10 | 200 | 99,270,014.00 | 96,031,441.40 | 149,898,614.00 | 27,358,194.00 | 149,898,614.00 | 10.41 |
| registry_lookup_keepalive | 10 | 1 | 2,981,710.50 | 4,176,344.40 | 11,265,721.00 | 2,623,313.00 | 11,265,721.00 | 239.44 |
| registry_unavailable | 10 | 0 | 458,754.00 | 1,343,060.10 | 3,282,424.00 | 250,895.00 | 3,282,424.00 | 744.57 |
| retry_failure_then_success | 10 | 200 | 5,723,839.00 | 5,313,711.50 | 7,912,594.00 | 3,077,377.00 | 7,912,594.00 | 188.19 |

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
| one_string_ten_int64 | 56.13 | 57.14 | 60.70 | 65.42 | 55 | 1.79% | 8.15% |
| one_string_ten_decimal_strings | 161.64 | 162.50 | 166.40 | 175.68 | 219 | 0.53% | 2.94% |
| ten_strings_fifty_decimal_strings | 904.73 | 902.58 | 922.61 | 931.33 | 1199 | -0.24% | 1.98% |

## All cached paths

| Payload | API | Reps | Framed bytes | Median ns/encode | Mean M/s | p95 ns |
|---|---|---:|---:|---:|---:|---:|
| one_string_ten_decimal_strings | cached_id_framed_in_place | 10 | 219 | 162.50 | 6,163,825.86 | 163.40 |
| one_string_ten_decimal_strings | cached_id_framed_with_copy | 10 | 219 | 166.40 | 5,998,138.18 | 169.08 |
| one_string_ten_decimal_strings | cached_serializer_string | 10 | 219 | 175.68 | 5,695,866.91 | 178.01 |
| one_string_ten_int64 | cached_id_framed_in_place | 10 | 55 | 57.14 | 17,545,090.88 | 58.51 |
| one_string_ten_int64 | cached_id_framed_with_copy | 10 | 55 | 60.70 | 16,467,194.06 | 62.35 |
| one_string_ten_int64 | cached_serializer_string | 10 | 55 | 65.42 | 15,291,384.99 | 66.33 |
| ten_strings_fifty_decimal_strings | cached_id_framed_in_place | 10 | 1199 | 902.58 | 1,107,987.08 | 907.90 |
| ten_strings_fifty_decimal_strings | cached_id_framed_with_copy | 10 | 1199 | 922.61 | 1,082,844.30 | 935.35 |
| ten_strings_fifty_decimal_strings | cached_serializer_string | 10 | 1199 | 931.33 | 1,073,554.82 | 936.71 |

## Live Schema Registry paths

These measurements include HTTP request/response and Registry processing. `registry_lookup_keepalive` means a keep-alive HTTP lookup; it is not a local schema-ID cache hit. The local cached-ID paths are the steady-state framing rows above. All live rows are control-plane latency measurements, not per-message serializer benchmarks.

| Path | Reps | Status/result | Median ns | Mean ns | p95 ns | Min ns | Max ns | Requests/sec |
|---|---:|---|---:|---:|---:|---:|---:|---:|
| cache_miss_404 | 10 | 404 | 10,882,515.00 | 11,570,972.80 | 19,232,759.00 | 8,308,853.00 | 19,232,759.00 | 86.42 |
| cold_lookup_new_connection | 10 | 1 | 15,858,214.00 | 15,782,154.20 | 18,325,800.00 | 13,455,685.00 | 18,325,800.00 | 63.36 |
| cold_register | 10 | 1 | 17,231,843.50 | 18,870,405.30 | 31,627,368.00 | 14,408,122.00 | 31,627,368.00 | 52.99 |
| concurrent_registration | 10 | 200 | 119,109,780.00 | 118,457,359.80 | 184,805,953.00 | 46,503,186.00 | 184,805,953.00 | 8.44 |
| registry_lookup_keepalive | 10 | 1 | 5,014,369.00 | 8,705,215.00 | 40,706,901.00 | 3,355,987.00 | 40,706,901.00 | 114.87 |
| registry_unavailable | 10 | 0 | 454,790.50 | 1,094,973.20 | 2,723,178.00 | 245,171.00 | 2,723,178.00 | 913.26 |
| retry_failure_then_success | 10 | 200 | 12,411,743.50 | 12,437,988.50 | 16,016,312.00 | 9,568,411.00 | 16,016,312.00 | 80.40 |

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
| lookup_versions | 10 | 200 | 2,765,678.00 | 3,373,372.60 | 8,631,855.00 | 5.0 |
| register_v1 | 10 | 200 | 17,563,184.50 | 47,268,788.20 | 311,233,879.00 | 2400.0 |
| register_v2 | 10 | 200 | 19,908,238.00 | 21,623,327.90 | 40,933,178.00 | 2422.0 |

## Interpretation

- A successful v2 registration under the v1 subject demonstrates the tested Registry compatibility path for this additive schema change.
- This does not replace a full compatibility-policy matrix; production rollout should also test breaking changes and the configured BACKWARD/FORWARD/FULL policy explicitly.


## Toolchain and host metadata

### toolchain_versions.txt

```text
architecture=x86_64
kernel=6.17.0-1020-azure
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
Linux runnervmvrwv9 6.17.0-1020-azure #20~24.04.1-Ubuntu SMP Fri Jun 19 20:09:14 UTC 2026 x86_64 x86_64 x86_64 GNU/Linux
```

### docker-version.txt

```text
Client: Docker Engine - Community
 Version:           28.0.4
 API version:       1.48
 Go version:        go1.23.7
 Git commit:        b8034c0
 Built:             Tue Mar 25 15:07:16 2025
 OS/Arch:           linux/amd64
 Context:           default

Server: Docker Engine - Community
 Engine:
  Version:          28.0.4
  API version:      1.48 (minimum version 1.24)
  Go version:       go1.23.7
  Git commit:       6430e49
  Built:            Tue Mar 25 15:07:16 2025
  OS/Arch:          linux/amd64
  Experimental:     false
 containerd:
  Version:          v2.2.6
  GitCommit:        11ce9d5f3c68c941867e82890e93e815c1304f1b
 runc:
  Version:          1.3.6
  GitCommit:        v1.3.6-0-g491b69ba
 docker-init:
  Version:          0.19.0
  GitCommit:        de40ad0
```

### docker-info.txt

```text
Client: Docker Engine - Community
 Version:    28.0.4
 Context:    default
 Debug Mode: false
 Plugins:
  buildx: Docker Buildx (Docker Inc.)
    Version:  v0.35.0
    Path:     /usr/libexec/docker/cli-plugins/docker-buildx
  compose: Docker Compose (Docker Inc.)
    Version:  v2.38.2
    Path:     /usr/libexec/docker/cli-plugins/docker-compose

Server:
 Containers: 0
  Running: 0
  Paused: 0
  Stopped: 0
 Images: 6
 Server Version: 28.0.4
 Storage Driver: overlay2
  Backing Filesystem: extfs
  Supports d_type: true
  Using metacopy: false
  Native Overlay Diff: false
  userxattr: false
 Logging Driver: json-file
 Cgroup Driver: systemd
 Cgroup Version: 2
 Plugins:
  Volume: local
  Network: bridge host ipvlan macvlan null overlay
  Log: awslogs fluentd gcplogs gelf journald json-file local splunk syslog
 Swarm: inactive
 Runtimes: io.containerd.runc.v2 runc
 Default Runtime: runc
 Init Binary: docker-init
 containerd version: 11ce9d5f3c68c941867e82890e93e815c1304f1b
 runc version: v1.3.6-0-g491b69ba
 init version: de40ad0
 Security Options:
  apparmor
  seccomp
   Profile: builtin
  cgroupns
 Kernel Version: 6.17.0-1020-azure
 Operating System: Ubuntu 24.04.4 LTS
 OSType: linux
 Architecture: x86_64
 CPUs: 4
 Total Memory: 15.61GiB
 Name: runnervmvrwv9
 ID: dce1e4a9-fc26-4218-aaf0-feee5194c8cb
 Docker Root Dir: /var/lib/docker
 Debug Mode: false
 Username: githubactions
 Experimental: false
 Insecure Registries:
  ::1/128
  127.0.0.0/8
 Live Restore Enabled: false
```

## Raw artifacts

The accompanying workflow artifact contains the raw CSV files, metadata, Docker version output, and this report. CSV files are retained for statistical re-analysis; the tables above are the human-readable snapshot committed or uploaded for review.

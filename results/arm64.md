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
- [COMPILER_REPORT.md](#compiler-reportmd)
- [REPRESENTATION_REPORT.md](#representation-reportmd)
- [LATENCY_REPORT.md](#latency-reportmd)
- [DISTRIBUTION_REPORT.md](#distribution-reportmd)
- [CONCURRENCY_REPORT.md](#concurrency-reportmd)
- [ASYNC_PIPELINE_REPORT.md](#async-pipeline-reportmd)
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
| one_string_ten_decimal_strings | google_protobuf / speed / SerializeToString | 160.29 | 5.57 M/s | 213 |
| one_string_ten_int64 | google_protobuf / lite_runtime / SerializeToArray_preallocated | 80.39 | 12.44 M/s | 49 |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated / pack_preallocated | 496.38 | 1.87 M/s | 1193 |

## Full aggregate results

| Kind | Library | Codegen | API | Payload | Reps | Bytes | Median ns/encode | Mean ns/encode | Mean M/s |
|---|---|---|---|---|---:|---:|---:|---:|---:|
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 160.29 | 195.62 | 5.57 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 170.56 | 178.56 | 5.69 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 171.94 | 182.42 | 5.58 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 203.28 | 240.16 | 4.59 |
| json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 414 | 211.49 | 212.72 | 4.70 |
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 213 | 267.42 | 345.98 | 3.82 |
| json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 364 | 304.78 | 311.81 | 3.22 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 1,131.89 | 1,161.42 | 0.86 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 1,220.67 | 1,593.44 | 0.72 |
| json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 364 | 1,413.99 | 1,563.76 | 0.67 |
| json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 364 | 1,559.01 | 1,943.75 | 0.60 |
| json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 364 | 3,048.01 | 3,150.35 | 0.32 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 80.39 | 80.38 | 12.44 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 49 | 82.35 | 82.56 | 12.11 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 82.52 | 82.95 | 12.06 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 49 | 87.47 | 87.67 | 11.41 |
| json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 210 | 95.66 | 120.10 | 9.15 |
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 49 | 176.61 | 176.93 | 5.83 |
| json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 210 | 289.74 | 294.88 | 3.40 |
| json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 210 | 382.38 | 397.41 | 2.53 |
| json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 210 | 729.05 | 813.57 | 1.31 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 49 | 857.79 | 864.11 | 1.16 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 897.92 | 913.13 | 1.10 |
| json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 210 | 2,611.16 | 3,270.59 | 0.35 |
| protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 496.38 | 557.15 | 1.87 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 600.22 | 601.80 | 1.66 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 629.42 | 629.95 | 1.59 |
| protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 641.62 | 649.35 | 1.55 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 641.78 | 840.17 | 1.35 |
| json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 2229 | 746.76 | 1,130.55 | 1.10 |
| json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1979 | 1,687.97 | 1,820.24 | 0.58 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 5,882.74 | 6,085.42 | 0.17 |
| protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 5,954.40 | 5,932.69 | 0.17 |
| json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 1979 | 7,862.55 | 8,213.46 | 0.13 |
| json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 1979 | 8,324.13 | 8,410.97 | 0.13 |
| json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 1979 | 14,717.33 | 16,717.85 | 0.06 |

## Protobuf-only comparison

| Payload | Library / generation | API | Median ns/encode | Relative to fastest protobuf |
|---|---|---|---:|---:|
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToString | 160.29 | 1.00x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 170.56 | 1.06x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 171.94 | 1.07x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 203.28 | 1.27x |
| one_string_ten_decimal_strings | protobuf_c / c_generated | pack_preallocated | 267.42 | 1.67x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToString | 1,131.89 | 7.06x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 1,220.67 | 7.62x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToArray_preallocated | 80.39 | 1.00x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToString | 82.35 | 1.02x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToArray_preallocated | 82.52 | 1.03x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToString | 87.47 | 1.09x |
| one_string_ten_int64 | protobuf_c / c_generated | pack_preallocated | 176.61 | 2.20x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToString | 857.79 | 10.67x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToArray_preallocated | 897.92 | 11.17x |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated | pack_preallocated | 496.38 | 1.00x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 600.22 | 1.21x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 629.42 | 1.27x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToString | 641.62 | 1.29x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 641.78 | 1.29x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 5,882.74 | 11.85x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToString | 5,954.40 | 12.00x |

## JSON-only comparison

| Payload | Library | Median ns/encode | Relative to fastest JSON |
|---|---|---:|---:|
| one_string_ten_decimal_strings | yyjson | 211.49 | 1.00x |
| one_string_ten_decimal_strings | boost_json | 304.78 | 1.44x |
| one_string_ten_decimal_strings | nlohmann_json | 1,413.99 | 6.69x |
| one_string_ten_decimal_strings | rapidjson | 1,559.01 | 7.37x |
| one_string_ten_decimal_strings | jsoncpp | 3,048.01 | 14.41x |
| one_string_ten_int64 | yyjson | 95.66 | 1.00x |
| one_string_ten_int64 | boost_json | 289.74 | 3.03x |
| one_string_ten_int64 | rapidjson | 382.38 | 4.00x |
| one_string_ten_int64 | nlohmann_json | 729.05 | 7.62x |
| one_string_ten_int64 | jsoncpp | 2,611.16 | 27.30x |
| ten_strings_fifty_decimal_strings | yyjson | 746.76 | 1.00x |
| ten_strings_fifty_decimal_strings | boost_json | 1,687.97 | 2.26x |
| ten_strings_fifty_decimal_strings | rapidjson | 7,862.55 | 10.53x |
| ten_strings_fifty_decimal_strings | nlohmann_json | 8,324.13 | 11.15x |
| ten_strings_fifty_decimal_strings | jsoncpp | 14,717.33 | 19.71x |

## Raw data

- `raw.csv` contains every individual repetition and is the source for the aggregates above.
- Re-run on a quiet, pinned CPU if comparing small differences; CPU frequency, thermal state, compiler, allocator, and container host affect absolute numbers.


## COMPILER_REPORT.md

# Compiler and C++ standard comparison

> Encoding only. No decoding, parsing, Schema Registry, or Kafka network work is included.

## Contract

- Every successful variant uses 1,000,000 encodes per repetition and 10 measured repetitions.
- All variants use the same Docker image, Buf-generated sources, `-O3 -march=native -DNDEBUG`, and pre-populated messages.
- C++23 is the established baseline; C++26 is included where the compiler accepts the standard mode.

## Variants

| Variant | Compiler | Standard | Architecture | Repetitions |
|---|---|---:|---|---:|
| clang-cxx23 | Clang 22.1.2 C++23 (Ubuntu clang version 22.1.2 (1ubuntu1)) | C++23 | aarch64 | 10 |
| clang-cxx26 | Clang 22.1.2 C++26 (Ubuntu clang version 22.1.2 (1ubuntu1)) | C++26 | aarch64 | 10 |
| gcc-cxx23 | GCC 15 C++23 (g++ (Ubuntu 15.2.0-16ubuntu1) 15.2.0) | C++23 | aarch64 | 10 |
| gcc-cxx26 | GCC 15 C++26 (g++ (Ubuntu 15.2.0-16ubuntu1) 15.2.0) | C++26 | aarch64 | 10 |

## Fastest row per payload and compiler

| Variant | Payload | Fastest implementation | Median ns/encode | Mean M/s |
|---|---|---|---:|---:|
| clang-cxx23 | one_string_ten_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 101.57 | 9.77 |
| clang-cxx23 | one_string_ten_int64 | protobuf / google_protobuf / lite_runtime / SerializeToArray_preallocated | 70.09 | 14.25 |
| clang-cxx23 | ten_strings_fifty_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 518.44 | 1.93 |
| clang-cxx26 | one_string_ten_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 96.24 | 10.30 |
| clang-cxx26 | one_string_ten_int64 | protobuf / google_protobuf / lite_runtime / SerializeToArray_preallocated | 72.04 | 13.38 |
| clang-cxx26 | ten_strings_fifty_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 493.45 | 2.03 |
| gcc-cxx23 | one_string_ten_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 149.69 | 6.40 |
| gcc-cxx23 | one_string_ten_int64 | protobuf / protobuf_c / c_generated / pack_preallocated | 82.06 | 12.11 |
| gcc-cxx23 | ten_strings_fifty_decimal_strings | protobuf / google_protobuf / speed / SerializeToArray_preallocated | 615.05 | 1.59 |
| gcc-cxx26 | one_string_ten_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 94.70 | 10.56 |
| gcc-cxx26 | one_string_ten_int64 | protobuf / google_protobuf / lite_runtime / SerializeToArray_preallocated | 76.59 | 13.04 |
| gcc-cxx26 | ten_strings_fifty_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 491.30 | 2.01 |

## Full aggregate results

| Variant | Kind | Library | Codegen | API | Payload | Reps | Median ns/encode | Mean ns/encode |
|---|---|---|---|---|---|---:|---:|---:|
| clang-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 101.57 | 102.37 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 148.75 | 148.55 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 151.32 | 151.61 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 164.65 | 165.20 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 168.66 | 177.00 |
| clang-cxx23 | json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 219.25 | 220.20 |
| clang-cxx23 | json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 280.24 | 285.57 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 1,166.81 | 1,257.95 |
| clang-cxx23 | json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 1,290.89 | 1,301.44 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 1,560.76 | 1,617.89 |
| clang-cxx23 | json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 1,587.35 | 1,609.64 |
| clang-cxx23 | json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 2,950.14 | 2,967.52 |
| clang-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 96.24 | 97.11 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 147.04 | 147.21 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 153.98 | 159.20 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 162.90 | 173.47 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 168.35 | 170.50 |
| clang-cxx26 | json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 219.67 | 219.89 |
| clang-cxx26 | json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 278.72 | 279.70 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 1,069.66 | 1,083.52 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 1,128.64 | 1,146.99 |
| clang-cxx26 | json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 1,870.81 | 2,210.83 |
| clang-cxx26 | json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 2,467.13 | 3,105.11 |
| clang-cxx26 | json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 3,223.87 | 3,920.10 |
| gcc-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 149.69 | 156.15 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 151.65 | 153.35 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 155.78 | 156.21 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 157.31 | 253.11 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 164.21 | 170.24 |
| gcc-cxx23 | json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 224.62 | 249.68 |
| gcc-cxx23 | json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 380.10 | 485.48 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 1,142.10 | 1,149.42 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 1,202.17 | 1,382.40 |
| gcc-cxx23 | json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 1,575.50 | 1,727.76 |
| gcc-cxx23 | json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 1,710.81 | 1,861.84 |
| gcc-cxx23 | json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 3,114.16 | 3,159.95 |
| gcc-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 94.70 | 94.72 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 142.38 | 142.52 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 147.94 | 154.76 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 150.24 | 150.72 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 154.96 | 155.82 |
| gcc-cxx26 | json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 211.06 | 217.86 |
| gcc-cxx26 | json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 288.21 | 293.10 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 1,063.63 | 1,076.54 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 1,087.85 | 1,088.47 |
| gcc-cxx26 | json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 1,180.24 | 1,194.73 |
| gcc-cxx26 | json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 1,182.08 | 1,194.00 |
| gcc-cxx26 | json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 2,850.77 | 2,919.19 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 70.09 | 70.17 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 74.89 | 75.17 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 91.15 | 92.81 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 91.34 | 93.33 |
| clang-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 96.53 | 108.82 |
| clang-cxx23 | json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 134.58 | 138.82 |
| clang-cxx23 | json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 301.98 | 306.29 |
| clang-cxx23 | json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 451.49 | 464.60 |
| clang-cxx23 | json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 755.59 | 757.11 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 937.51 | 958.17 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 1,155.56 | 1,705.66 |
| clang-cxx23 | json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 2,349.01 | 2,373.23 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 72.04 | 74.72 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 81.72 | 82.55 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 85.05 | 85.22 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 95.25 | 94.73 |
| clang-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 96.80 | 101.68 |
| clang-cxx26 | json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 252.02 | 307.19 |
| clang-cxx26 | json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 307.07 | 410.98 |
| clang-cxx26 | json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 427.80 | 440.47 |
| clang-cxx26 | json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 719.17 | 724.50 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 886.94 | 906.27 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 898.46 | 918.61 |
| clang-cxx26 | json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 2,393.96 | 2,876.92 |
| gcc-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 82.06 | 82.56 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 85.04 | 88.84 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 89.09 | 89.35 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 89.39 | 98.31 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 90.87 | 98.55 |
| gcc-cxx23 | json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 92.48 | 92.45 |
| gcc-cxx23 | json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 303.34 | 313.22 |
| gcc-cxx23 | json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 572.02 | 579.57 |
| gcc-cxx23 | json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 672.98 | 683.03 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 1,004.23 | 1,221.48 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 1,104.40 | 1,292.05 |
| gcc-cxx23 | json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 2,295.87 | 2,848.74 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 76.59 | 76.70 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 80.61 | 84.58 |
| gcc-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 80.99 | 81.01 |
| gcc-cxx26 | json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 87.07 | 87.08 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 88.94 | 93.41 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 93.00 | 93.36 |
| gcc-cxx26 | json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 284.21 | 286.00 |
| gcc-cxx26 | json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 372.03 | 376.65 |
| gcc-cxx26 | json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 662.90 | 675.68 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 844.13 | 852.68 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 846.77 | 859.45 |
| gcc-cxx26 | json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 2,147.70 | 2,161.53 |
| clang-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 518.44 | 517.48 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 645.74 | 654.83 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 648.79 | 665.29 |
| clang-cxx23 | json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 803.26 | 811.32 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 815.29 | 806.62 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 881.13 | 875.17 |
| clang-cxx23 | json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1,383.94 | 1,396.50 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 5,611.24 | 5,909.05 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 5,799.23 | 6,186.40 |
| clang-cxx23 | json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 6,312.21 | 6,711.34 |
| clang-cxx23 | json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 7,620.30 | 8,415.63 |
| clang-cxx23 | json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 14,495.96 | 14,520.71 |
| clang-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 493.45 | 493.51 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 615.99 | 809.03 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 623.46 | 631.36 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 653.21 | 668.55 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 661.65 | 663.23 |
| clang-cxx26 | json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 941.28 | 970.99 |
| clang-cxx26 | json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1,424.85 | 1,577.78 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 5,541.22 | 5,558.23 |
| clang-cxx26 | json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 5,972.68 | 5,987.90 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 6,066.22 | 6,980.86 |
| clang-cxx26 | json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 7,494.98 | 7,843.45 |
| clang-cxx26 | json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 15,177.73 | 15,655.13 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 615.05 | 629.41 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 615.78 | 625.36 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 620.05 | 632.70 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 624.49 | 662.73 |
| gcc-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 763.18 | 765.34 |
| gcc-cxx23 | json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 805.28 | 846.40 |
| gcc-cxx23 | json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1,509.66 | 1,527.49 |
| gcc-cxx23 | json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 5,788.82 | 6,083.29 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 5,802.77 | 6,009.62 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 6,008.46 | 6,610.64 |
| gcc-cxx23 | json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 8,555.00 | 9,445.58 |
| gcc-cxx23 | json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 16,479.83 | 17,288.98 |
| gcc-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 491.30 | 498.00 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 595.93 | 606.41 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 607.92 | 610.15 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 607.98 | 616.26 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 617.83 | 621.41 |
| gcc-cxx26 | json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 803.45 | 823.30 |
| gcc-cxx26 | json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1,514.97 | 1,526.72 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 5,470.17 | 5,464.20 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 5,490.99 | 5,489.79 |
| gcc-cxx26 | json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 6,532.13 | 6,907.56 |
| gcc-cxx26 | json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 6,980.36 | 7,119.05 |
| gcc-cxx26 | json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 14,948.90 | 15,335.65 |

## Interpretation

- Compare compiler rows only within the same host architecture and Docker host; compiler and standard effects are smaller than CPU, frequency, allocator, and scheduler variance in many rows.
- A successful C++26 row proves the selected compiler accepted that language mode; it does not imply all generated dependencies have adopted every C++26 feature.
- Raw per-repetition data remains in `results/compiler/<variant>/raw.csv`.


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
| fixed64_scale_8 | 10 | 99 | 58.70 | 59.71 | 68.95 | 68.95 | 68.95 | 68.95 | 16.75 |
| bytes_128bit_coefficient_scale_8 | 10 | 189 | 77.30 | 81.21 | 114.97 | 114.97 | 114.97 | 114.97 | 12.31 |
| decimal_string | 10 | 169 | 202.87 | 205.13 | 278.78 | 278.78 | 278.78 | 278.78 | 4.87 |
| scaled_sint64_scale_8 | 10 | 79 | 243.30 | 224.62 | 360.52 | 360.52 | 360.52 | 360.52 | 4.45 |

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
| one_string_ten_decimal_strings | serialize_array_arena_message | 10 | 213 | 167.00 | 208.00 | 209.00 | 209.00 | 4,833.50 | 37,022.00 | 0.00 | 0.00 |
| one_string_ten_decimal_strings | serialize_array_fresh_buffer | 10 | 213 | 208.00 | 209.00 | 209.00 | 250.00 | 6,250.00 | 59,252.00 | 1,000,000.00 | 213,000,000.00 |
| one_string_ten_decimal_strings | serialize_array_reuse | 10 | 213 | 167.00 | 208.00 | 209.00 | 209.00 | 3,479.00 | 46,501.50 | 0.00 | 0.00 |
| one_string_ten_decimal_strings | serialize_string_reserved | 10 | 213 | 167.00 | 209.00 | 209.00 | 250.00 | 6,208.50 | 30,979.50 | 0.00 | 0.00 |
| one_string_ten_int64 | serialize_array_arena_message | 10 | 49 | 125.00 | 125.00 | 125.00 | 166.00 | 2,979.50 | 39,501.50 | 0.00 | 0.00 |
| one_string_ten_int64 | serialize_array_fresh_buffer | 10 | 49 | 125.00 | 167.00 | 167.00 | 167.00 | 3,312.50 | 57,231.00 | 1,000,000.00 | 49,000,000.00 |
| one_string_ten_int64 | serialize_array_reuse | 10 | 49 | 125.00 | 125.00 | 125.00 | 166.00 | 3,000.50 | 38,980.00 | 0.00 | 0.00 |
| one_string_ten_int64 | serialize_string_reserved | 10 | 49 | 125.00 | 125.00 | 125.00 | 167.00 | 3,000.00 | 43,813.50 | 0.00 | 0.00 |
| ten_strings_fifty_decimal_strings | serialize_array_arena_message | 10 | 1193 | 625.00 | 625.50 | 667.00 | 3,312.50 | 13,104.50 | 94,294.00 | 0.00 | 0.00 |
| ten_strings_fifty_decimal_strings | serialize_array_fresh_buffer | 10 | 1193 | 667.00 | 708.50 | 750.00 | 3,458.00 | 16,437.50 | 125,337.00 | 1,000,000.00 | 1,193,000,000.00 |
| ten_strings_fifty_decimal_strings | serialize_array_reuse | 10 | 1193 | 625.00 | 667.00 | 709.00 | 3,479.50 | 22,750.50 | 482,492.00 | 0.00 | 0.00 |
| ten_strings_fifty_decimal_strings | serialize_string_reserved | 10 | 1193 | 625.00 | 667.00 | 708.00 | 3,333.50 | 14,271.50 | 105,065.50 | 0.00 | 0.00 |

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
| int64_small_varints | 10 | 29.00 | 90.14 | 102.37 | 10.27 |
| string_short_ascii | 10 | 49.00 | 98.96 | 105.26 | 9.75 |
| string_multibyte_utf8 | 10 | 67.92 | 108.46 | 112.24 | 8.98 |
| int64_mixed_distribution | 10 | 76.50 | 109.39 | 116.44 | 8.68 |
| int64_negative_standard_varint | 10 | 119.00 | 126.76 | 146.22 | 7.14 |
| int64_large_varints | 10 | 109.00 | 152.15 | 181.46 | 6.08 |
| string_long_ascii_256 | 10 | 299.00 | 159.14 | 159.16 | 6.65 |

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
| shared_buffer_mutex | 1 | 10 | 49 | 102.77 | 103.66 | 9.65 |
| shared_buffer_mutex | 2 | 10 | 49 | 103.34 | 104.13 | 9.61 |
| shared_buffer_mutex | 4 | 10 | 49 | 104.66 | 107.45 | 9.36 |
| shared_buffer_mutex | 8 | 10 | 49 | 104.79 | 104.94 | 9.53 |
| shared_readonly | 1 | 10 | 49 | 85.40 | 86.04 | 11.63 |
| shared_readonly | 2 | 10 | 49 | 86.68 | 88.48 | 11.34 |
| shared_readonly | 4 | 10 | 49 | 89.31 | 91.30 | 11.00 |
| shared_readonly | 8 | 10 | 49 | 85.43 | 85.40 | 11.71 |
| thread_local | 1 | 10 | 49 | 86.96 | 90.52 | 11.18 |
| thread_local | 2 | 10 | 49 | 84.63 | 84.56 | 11.83 |
| thread_local | 4 | 10 | 49 | 84.72 | 88.86 | 11.41 |
| thread_local | 8 | 10 | 49 | 85.82 | 86.46 | 11.57 |

## Mode definitions

- `thread_local`: each worker has its own protobuf message and output buffer.
- `shared_readonly`: workers serialize the same immutable message into independent buffers.
- `shared_buffer_mutex`: workers serialize into one shared buffer under a mutex, exposing lock contention.
- Thread creation and barrier setup are outside the timed encode loop.


## ASYNC_PIPELINE_REPORT.md

# Asynchronous hot-path pipeline benchmark

> Producer-side measurements cover the non-blocking event handoff. Protobuf and pipe formatting measurements cover worker-side work. No decoding, broker, or network work is included.

## Run contract

- Iterations per row: `1000000`
- Measured repetitions: `10`
- Warmup iterations: `10000`
- Queue capacity: `1048576` pointer slots
- Handoff model: `preallocated_immutable_event_pool_pointer`

The queue is preallocated larger than one measured repetition so the producer never waits for the worker during the baseline handoff test. A dropped count is still reported and must be zero for a valid throughput row.

## Results

| Mode | Test case | Completed | Dropped | Payload + header bytes | Hot handoff p50 ns | Hot handoff p99 ns | Worker p50 ns | Worker p99 ns | Worker wall M/s |
|---|---|---:|---:|---:|---:|---:|---:|---:|---:|
| async_both | one_string_ten_decimal_strings | 1,000,000 | 0 | 219 | 41.00 | 83.00 | 333.00 | 354.50 | 2.88 |
| async_both | one_string_ten_int64 | 1,000,000 | 0 | 55 | 41.00 | 83.00 | 250.00 | 250.00 | 3.78 |
| async_both | ten_strings_fifty_decimal_strings | 1,000,000 | 0 | 1199 | 41.00 | 42.00 | 1,250.00 | 1,375.00 | 0.78 |
| async_handoff | one_string_ten_decimal_strings | 1,000,000 | 0 | 219 | 83.00 | 84.00 | 0.00 | 42.00 | 8.36 |
| async_handoff | one_string_ten_int64 | 1,000,000 | 0 | 55 | 42.00 | 84.00 | 0.00 | 42.00 | 11.60 |
| async_handoff | ten_strings_fifty_decimal_strings | 1,000,000 | 0 | 1199 | 83.00 | 84.00 | 0.00 | 42.00 | 8.31 |
| async_pipe | one_string_ten_decimal_strings | 1,000,000 | 0 | 219 | 42.00 | 83.50 | 83.00 | 125.00 | 7.41 |
| async_pipe | one_string_ten_int64 | 1,000,000 | 0 | 55 | 41.00 | 84.00 | 125.00 | 167.00 | 6.08 |
| async_pipe | ten_strings_fifty_decimal_strings | 1,000,000 | 0 | 1199 | 41.00 | 83.00 | 250.00 | 292.00 | 3.37 |
| async_protobuf | one_string_ten_decimal_strings | 1,000,000 | 0 | 219 | 41.00 | 42.00 | 250.00 | 292.00 | 3.44 |
| async_protobuf | one_string_ten_int64 | 1,000,000 | 0 | 55 | 41.00 | 83.00 | 125.00 | 167.00 | 6.19 |
| async_protobuf | ten_strings_fifty_decimal_strings | 1,000,000 | 0 | 1199 | 41.00 | 42.00 | 1,000.00 | 1,166.50 | 0.93 |
| sync_both | one_string_ten_decimal_strings | 1,000,000 | 0 | 219 | 312.50 | 354.50 | 312.50 | 354.50 | 2.98 |
| sync_both | one_string_ten_int64 | 1,000,000 | 0 | 55 | 209.00 | 250.00 | 209.00 | 250.00 | 3.98 |
| sync_both | ten_strings_fifty_decimal_strings | 1,000,000 | 0 | 1199 | 1,229.00 | 1,354.50 | 1,229.00 | 1,354.50 | 0.79 |

## Mode definitions

- `sync_both`: control path; the producer performs Protobuf framing and pipe formatting itself.
- `async_handoff`: producer performs only the non-blocking ring handoff; the worker drains the queue without serialization work.
- `async_protobuf`: producer hands off; the worker populates the generated message and serializes the six-byte Confluent-framed payload.
- `async_pipe`: producer hands off; the worker formats the pipe-delimited line into a reusable buffer.
- `async_both`: producer hands off; the worker performs both Protobuf serialization and pipe formatting.

## Interpretation

The number to protect on the trading thread is hot handoff p50/p99, not worker encoding time. The worker rows answer whether the encoder can keep up with the event rate. `async_handoff` isolates the queue cost; `async_both` is the relevant design when one worker creates both Kafka bytes and human-readable log lines.

The benchmark uses an immutable event-pool pointer handoff to model transferring ownership from a pool without hot-thread allocation or variable-size copying. A production implementation that copies a fixed-size event into the ring should be benchmarked as a separate queue variant because its cost depends on the exact event layout and string storage strategy.

A non-zero dropped count means the configured worker/queue combination did not sustain the attempted event rate. It is not a successful 1M-encoding result and must be investigated rather than averaged away.


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

If events are unavailable under the container host, the output is retained explicitly rather than being presented as a valid counter measurement. Repeat this phase on production Linux hardware with the required perf permissions for authoritative microarchitectural comparisons.


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
| copy | 0 | lz4 | 0 | 1 | 10 | 49 | 49,224.71 | 53,941.98 | 74,960.06 | 74,960.06 | 0 |
| copy | 0 | lz4 | 0 | 100 | 10 | 49 | 459.38 | 499.93 | 637.96 | 637.96 | 0 |
| copy | 0 | lz4 | 5 | 1 | 10 | 49 | 41,544.73 | 46,174.51 | 46,690.77 | 46,690.77 | 0 |
| copy | 0 | lz4 | 5 | 100 | 10 | 49 | 507.67 | 507.76 | 551.78 | 551.78 | 0 |
| copy | 0 | none | 0 | 1 | 10 | 49 | 42,456.27 | 47,609.74 | 63,083.53 | 63,083.53 | 0 |
| copy | 0 | none | 0 | 100 | 10 | 49 | 494.98 | 551.97 | 1,156.63 | 1,156.63 | 0 |
| copy | 0 | none | 5 | 1 | 10 | 49 | 42,169.04 | 46,845.21 | 67,508.94 | 67,508.94 | 0 |
| copy | 0 | none | 5 | 100 | 10 | 49 | 702.68 | 788.12 | 954.10 | 954.10 | 0 |
| copy | 0 | zstd | 0 | 1 | 10 | 49 | 42,540.86 | 47,125.46 | 48,614.27 | 48,614.27 | 0 |
| copy | 0 | zstd | 0 | 100 | 10 | 49 | 607.80 | 610.78 | 885.15 | 885.15 | 0 |
| copy | 0 | zstd | 5 | 1 | 10 | 49 | 44,322.25 | 49,025.99 | 68,494.06 | 68,494.06 | 0 |
| copy | 0 | zstd | 5 | 100 | 10 | 49 | 610.41 | 611.21 | 932.45 | 932.45 | 0 |
| copy | 1 | lz4 | 0 | 1 | 10 | 49 | 62,735.25 | 70,327.37 | 83,922.19 | 83,922.19 | 0 |
| copy | 1 | lz4 | 0 | 100 | 10 | 49 | 934.03 | 1,002.92 | 1,911.06 | 1,911.06 | 0 |
| copy | 1 | lz4 | 5 | 1 | 10 | 49 | 65,931.82 | 73,576.38 | 88,908.52 | 88,908.52 | 0 |
| copy | 1 | lz4 | 5 | 100 | 10 | 49 | 557.53 | 623.03 | 736.04 | 736.04 | 0 |
| copy | 1 | none | 0 | 1 | 10 | 49 | 62,526.03 | 68,981.85 | 77,062.97 | 77,062.97 | 0 |
| copy | 1 | none | 0 | 100 | 10 | 49 | 658.41 | 738.26 | 842.99 | 842.99 | 0 |
| copy | 1 | none | 5 | 1 | 10 | 49 | 70,822.42 | 81,161.24 | 100,764.53 | 100,764.53 | 0 |
| copy | 1 | none | 5 | 100 | 10 | 49 | 595.26 | 669.15 | 844.43 | 844.43 | 0 |
| copy | 1 | zstd | 0 | 1 | 10 | 49 | 63,616.55 | 70,028.15 | 110,860.43 | 110,860.43 | 0 |
| copy | 1 | zstd | 0 | 100 | 10 | 49 | 649.03 | 700.72 | 1,107.21 | 1,107.21 | 0 |
| copy | 1 | zstd | 5 | 1 | 10 | 49 | 59,351.92 | 66,071.35 | 79,860.74 | 79,860.74 | 0 |
| copy | 1 | zstd | 5 | 100 | 10 | 49 | 649.31 | 714.76 | 813.68 | 813.68 | 0 |
| copy | all | lz4 | 0 | 1 | 10 | 49 | 60,738.07 | 67,908.32 | 80,326.41 | 80,326.41 | 0 |
| copy | all | lz4 | 0 | 100 | 10 | 49 | 666.26 | 740.71 | 776.62 | 776.62 | 0 |
| copy | all | lz4 | 5 | 1 | 10 | 49 | 57,704.50 | 64,074.82 | 73,636.01 | 73,636.01 | 0 |
| copy | all | lz4 | 5 | 100 | 10 | 49 | 740.27 | 810.28 | 2,064.72 | 2,064.72 | 0 |
| copy | all | none | 0 | 1 | 10 | 49 | 60,948.96 | 67,358.40 | 76,429.11 | 76,429.11 | 0 |
| copy | all | none | 0 | 100 | 10 | 49 | 677.55 | 747.95 | 3,640.39 | 3,640.39 | 0 |
| copy | all | none | 5 | 1 | 10 | 49 | 60,202.48 | 67,291.16 | 76,794.36 | 76,794.36 | 0 |
| copy | all | none | 5 | 100 | 10 | 49 | 1,010.63 | 1,098.15 | 3,546.59 | 3,546.59 | 0 |
| copy | all | zstd | 0 | 1 | 10 | 49 | 63,044.16 | 69,394.71 | 83,398.94 | 83,398.94 | 0 |
| copy | all | zstd | 0 | 100 | 10 | 49 | 870.99 | 934.84 | 2,091.94 | 2,091.94 | 0 |
| copy | all | zstd | 5 | 1 | 10 | 49 | 73,405.62 | 79,764.85 | 98,797.49 | 98,797.49 | 0 |
| copy | all | zstd | 5 | 100 | 10 | 49 | 645.95 | 678.92 | 863.84 | 863.84 | 0 |
| owned | 0 | lz4 | 0 | 1 | 10 | 49 | 52,733.74 | 60,202.20 | 74,195.87 | 74,195.87 | 0 |
| owned | 0 | lz4 | 0 | 100 | 10 | 49 | 544.79 | 572.67 | 630.00 | 630.00 | 0 |
| owned | 0 | lz4 | 5 | 1 | 10 | 49 | 55,470.67 | 61,643.35 | 142,561.94 | 142,561.94 | 0 |
| owned | 0 | lz4 | 5 | 100 | 10 | 49 | 619.35 | 635.47 | 782.13 | 782.13 | 0 |
| owned | 0 | none | 0 | 1 | 10 | 49 | 42,326.07 | 46,690.62 | 61,063.23 | 61,063.23 | 0 |
| owned | 0 | none | 0 | 100 | 10 | 49 | 694.61 | 740.57 | 1,022.17 | 1,022.17 | 0 |
| owned | 0 | none | 5 | 1 | 10 | 49 | 40,804.85 | 45,120.73 | 57,854.06 | 57,854.06 | 0 |
| owned | 0 | none | 5 | 100 | 10 | 49 | 548.51 | 552.35 | 602.28 | 602.28 | 0 |
| owned | 0 | zstd | 0 | 1 | 10 | 49 | 41,045.58 | 45,631.82 | 60,839.49 | 60,839.49 | 0 |
| owned | 0 | zstd | 0 | 100 | 10 | 49 | 684.11 | 685.48 | 738.94 | 738.94 | 0 |
| owned | 0 | zstd | 5 | 1 | 10 | 49 | 40,628.25 | 45,182.90 | 55,109.33 | 55,109.33 | 0 |
| owned | 0 | zstd | 5 | 100 | 10 | 49 | 641.97 | 642.45 | 711.65 | 711.65 | 0 |
| owned | 1 | lz4 | 0 | 1 | 10 | 49 | 56,643.71 | 62,970.52 | 63,668.73 | 63,668.73 | 0 |
| owned | 1 | lz4 | 0 | 100 | 10 | 49 | 706.72 | 775.90 | 929.09 | 929.09 | 0 |
| owned | 1 | lz4 | 5 | 1 | 10 | 49 | 57,649.83 | 63,957.77 | 82,465.22 | 82,465.22 | 0 |
| owned | 1 | lz4 | 5 | 100 | 10 | 49 | 587.72 | 597.02 | 875.34 | 875.34 | 0 |
| owned | 1 | none | 0 | 1 | 10 | 49 | 57,541.21 | 63,861.84 | 74,259.39 | 74,259.39 | 0 |
| owned | 1 | none | 0 | 100 | 10 | 49 | 641.24 | 713.57 | 857.41 | 857.41 | 0 |
| owned | 1 | none | 5 | 1 | 10 | 49 | 57,169.81 | 63,442.71 | 83,032.04 | 83,032.04 | 0 |
| owned | 1 | none | 5 | 100 | 10 | 49 | 590.73 | 650.71 | 1,687.49 | 1,687.49 | 0 |
| owned | 1 | zstd | 0 | 1 | 10 | 49 | 55,882.20 | 62,091.71 | 74,914.80 | 74,914.80 | 0 |
| owned | 1 | zstd | 0 | 100 | 10 | 49 | 695.62 | 718.79 | 832.82 | 832.82 | 0 |
| owned | 1 | zstd | 5 | 1 | 10 | 49 | 56,128.70 | 62,384.59 | 82,588.51 | 82,588.51 | 0 |
| owned | 1 | zstd | 5 | 100 | 10 | 49 | 668.10 | 689.00 | 909.91 | 909.91 | 0 |
| owned | all | lz4 | 0 | 1 | 10 | 49 | 57,615.73 | 64,027.30 | 64,340.19 | 64,340.19 | 0 |
| owned | all | lz4 | 0 | 100 | 10 | 49 | 698.13 | 765.36 | 803.90 | 803.90 | 0 |
| owned | all | lz4 | 5 | 1 | 10 | 49 | 58,771.08 | 65,673.72 | 75,995.68 | 75,995.68 | 0 |
| owned | all | lz4 | 5 | 100 | 10 | 49 | 974.66 | 1,110.79 | 1,638.29 | 1,638.29 | 0 |
| owned | all | none | 0 | 1 | 10 | 49 | 57,484.55 | 63,878.59 | 67,738.50 | 67,738.50 | 0 |
| owned | all | none | 0 | 100 | 10 | 49 | 638.75 | 705.66 | 896.26 | 896.26 | 0 |
| owned | all | none | 5 | 1 | 10 | 49 | 59,177.26 | 66,238.77 | 74,194.75 | 74,194.75 | 0 |
| owned | all | none | 5 | 100 | 10 | 49 | 597.69 | 681.44 | 895.35 | 895.35 | 0 |
| owned | all | zstd | 0 | 1 | 10 | 49 | 56,712.02 | 62,964.04 | 75,752.18 | 75,752.18 | 0 |
| owned | all | zstd | 0 | 100 | 10 | 49 | 708.19 | 756.18 | 856.22 | 856.22 | 0 |
| owned | all | zstd | 5 | 1 | 10 | 49 | 57,712.02 | 64,039.18 | 72,651.37 | 72,651.37 | 0 |
| owned | all | zstd | 5 | 100 | 10 | 49 | 664.91 | 667.73 | 886.83 | 886.83 | 0 |

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
| one_string_ten_int64 | 82.52 | 83.62 | 85.86 | 91.93 | 55 | 1.33% | 4.05% |
| one_string_ten_decimal_strings | 203.28 | 150.16 | 152.65 | 160.72 | 219 | -26.13% | -24.91% |
| ten_strings_fifty_decimal_strings | 600.22 | 608.63 | 619.08 | 628.22 | 1199 | 1.40% | 3.14% |

## All cached paths

| Payload | API | Reps | Framed bytes | Median ns/encode | Mean M/s | p95 ns |
|---|---|---:|---:|---:|---:|---:|
| one_string_ten_decimal_strings | cached_id_framed_in_place | 10 | 219 | 150.16 | 6,662,145.29 | 151.82 |
| one_string_ten_decimal_strings | cached_id_framed_with_copy | 10 | 219 | 152.65 | 6,533,398.41 | 156.15 |
| one_string_ten_decimal_strings | cached_serializer_string | 10 | 219 | 160.72 | 6,176,183.03 | 168.46 |
| one_string_ten_int64 | cached_id_framed_in_place | 10 | 55 | 83.62 | 11,940,498.11 | 85.87 |
| one_string_ten_int64 | cached_id_framed_with_copy | 10 | 55 | 85.86 | 11,537,897.96 | 90.92 |
| one_string_ten_int64 | cached_serializer_string | 10 | 55 | 91.93 | 10,598,081.54 | 111.46 |
| ten_strings_fifty_decimal_strings | cached_id_framed_in_place | 10 | 1199 | 608.63 | 1,619,626.24 | 673.89 |
| ten_strings_fifty_decimal_strings | cached_id_framed_with_copy | 10 | 1199 | 619.08 | 1,615,933.10 | 622.98 |
| ten_strings_fifty_decimal_strings | cached_serializer_string | 10 | 1199 | 628.22 | 1,568,795.28 | 710.98 |

## Live Schema Registry paths

These measurements include HTTP request/response and Registry processing. `registry_lookup_keepalive` means a keep-alive HTTP lookup; it is not a local schema-ID cache hit. The local cached-ID paths are the steady-state framing rows above. All live rows are control-plane latency measurements, not per-message serializer benchmarks.

| Path | Reps | Status/result | Median ns | Mean ns | p95 ns | Min ns | Max ns | Requests/sec |
|---|---:|---|---:|---:|---:|---:|---:|---:|
| cache_miss_404 | 10 | 404 | 2,006,081.00 | 2,455,819.60 | 6,835,914.00 | 1,778,595.00 | 6,835,914.00 | 407.20 |
| cold_lookup_new_connection | 10 | 1 | 2,845,064.50 | 3,037,162.00 | 4,479,634.00 | 2,475,616.00 | 4,479,634.00 | 329.25 |
| cold_register | 10 | 1 | 15,531,444.50 | 15,823,116.20 | 19,680,757.00 | 13,969,586.00 | 19,680,757.00 | 63.20 |
| concurrent_registration | 10 | 200 | 70,311,588.50 | 76,565,630.10 | 140,244,540.00 | 17,400,564.00 | 140,244,540.00 | 13.06 |
| registry_lookup_keepalive | 10 | 1 | 2,162,481.50 | 3,183,074.60 | 10,649,778.00 | 2,011,769.00 | 10,649,778.00 | 314.16 |
| registry_unavailable | 10 | 0 | 160,713.50 | 178,013.90 | 350,011.00 | 141,879.00 | 350,011.00 | 5,617.54 |
| retry_failure_then_success | 10 | 200 | 2,454,365.00 | 2,412,326.50 | 2,571,494.00 | 2,187,607.00 | 2,571,494.00 | 414.54 |

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
| one_string_ten_int64 | 82.52 | 82.98 | 87.42 | 90.95 | 55 | 0.56% | 5.94% |
| one_string_ten_decimal_strings | 203.28 | 148.73 | 155.29 | 157.75 | 219 | -26.84% | -23.61% |
| ten_strings_fifty_decimal_strings | 600.22 | 597.80 | 612.19 | 628.60 | 1199 | -0.40% | 1.99% |

## All cached paths

| Payload | API | Reps | Framed bytes | Median ns/encode | Mean M/s | p95 ns |
|---|---|---:|---:|---:|---:|---:|
| one_string_ten_decimal_strings | cached_id_framed_in_place | 10 | 219 | 148.73 | 6,726,492.09 | 150.18 |
| one_string_ten_decimal_strings | cached_id_framed_with_copy | 10 | 219 | 155.29 | 6,037,991.04 | 200.76 |
| one_string_ten_decimal_strings | cached_serializer_string | 10 | 219 | 157.75 | 6,302,873.23 | 162.31 |
| one_string_ten_int64 | cached_id_framed_in_place | 10 | 55 | 82.98 | 11,985,377.84 | 85.73 |
| one_string_ten_int64 | cached_id_framed_with_copy | 10 | 55 | 87.42 | 11,303,586.06 | 95.03 |
| one_string_ten_int64 | cached_serializer_string | 10 | 55 | 90.95 | 10,964,010.64 | 93.72 |
| ten_strings_fifty_decimal_strings | cached_id_framed_in_place | 10 | 1199 | 597.80 | 1,671,323.25 | 601.70 |
| ten_strings_fifty_decimal_strings | cached_id_framed_with_copy | 10 | 1199 | 612.19 | 1,614,544.59 | 673.66 |
| ten_strings_fifty_decimal_strings | cached_serializer_string | 10 | 1199 | 628.60 | 1,566,003.93 | 696.17 |

## Live Schema Registry paths

These measurements include HTTP request/response and Registry processing. `registry_lookup_keepalive` means a keep-alive HTTP lookup; it is not a local schema-ID cache hit. The local cached-ID paths are the steady-state framing rows above. All live rows are control-plane latency measurements, not per-message serializer benchmarks.

| Path | Reps | Status/result | Median ns | Mean ns | p95 ns | Min ns | Max ns | Requests/sec |
|---|---:|---|---:|---:|---:|---:|---:|---:|
| cache_miss_404 | 10 | 404 | 6,941,812.50 | 7,768,641.40 | 14,276,220.00 | 6,028,848.00 | 14,276,220.00 | 128.72 |
| cold_lookup_new_connection | 10 | 1 | 10,531,336.50 | 10,626,452.20 | 13,691,286.00 | 9,053,689.00 | 13,691,286.00 | 94.10 |
| cold_register | 10 | 1 | 15,910,310.50 | 17,045,548.80 | 26,011,489.00 | 14,269,719.00 | 26,011,489.00 | 58.67 |
| concurrent_registration | 10 | 200 | 91,220,799.50 | 88,092,621.90 | 137,813,218.00 | 35,376,686.00 | 137,813,218.00 | 11.35 |
| registry_lookup_keepalive | 10 | 1 | 3,849,698.50 | 10,024,759.10 | 64,320,096.00 | 2,705,081.00 | 64,320,096.00 | 99.75 |
| registry_unavailable | 10 | 0 | 146,379.50 | 163,313.60 | 338,761.00 | 115,504.00 | 338,761.00 | 6,123.19 |
| retry_failure_then_success | 10 | 200 | 7,671,396.50 | 7,630,116.50 | 8,898,559.00 | 6,486,778.00 | 8,898,559.00 | 131.06 |

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
| lookup_versions | 10 | 200 | 2,311,340.00 | 2,627,674.60 | 5,055,068.00 | 5.0 |
| register_v1 | 10 | 200 | 16,665,916.50 | 33,255,510.00 | 180,357,368.00 | 2400.0 |
| register_v2 | 10 | 200 | 16,896,194.00 | 21,815,966.90 | 53,886,700.00 | 2422.0 |

## Interpretation

- A successful v2 registration under the v1 subject demonstrates the tested Registry compatibility path for this additive schema change.
- This does not replace a full compatibility-policy matrix; production rollout should also test breaking changes and the configured BACKWARD/FORWARD/FULL policy explicitly.

## Conclusion

### Recommendation for this ARM64 host

Use Buf-generated Google protobuf C++ `SPEED` types with a reused caller-owned buffer and preallocated `SerializeToArray` as the default implementation. It is the best cross-architecture baseline and is the cleanest fit for Confluent framing. On this ARM64 run, `LITE_RUNTIME` was marginally fastest for the compact int64 payload (80.39 ns), Google `SPEED` plus `SerializeToString` led the smaller decimal-string payload (160.29 ns), and protobuf-c `pack_preallocated` led the largest string-heavy payload (496.38 ns).

Protobuf-c is worth a targeted optimization when a stable message type is proven hot: it won on the largest ARM64 payload, but it was not the universal winner. `CODE_SIZE` should not be used in the HFT path; it was much slower. JSON/yyjson remains useful for interoperability, not for the lowest-latency wire path.

### Schema Registry without Kafka

The cached Confluent prefix adds six bytes. ARM64 pure protobuf versus cached in-place framing measured:

| Payload | Pure protobuf ns | Cached in-place ns | Overhead |
|---|---:|---:|---:|
| `one_string_ten_int64` | 82.52 | 83.62 | +1.33% |
| `one_string_ten_decimal_strings` | 203.28 | 150.16 | -26.13%* |
| `ten_strings_fifty_decimal_strings` | 600.22 | 608.63 | +1.40% |

`*` The negative decimal row is measurement noise from separate runs, not a real Registry speedup. The stable conclusion is that cached framing is approximately low-single-digit overhead; live Registry lookup/registration is millisecond-scale and belongs in startup or recovery, never per message.

### Operating rule

Cache the schema ID before the producer hot path, serialize into a reused buffer with reserved prefix space, and keep Registry HTTP and Kafka delivery policy outside the encoding decision. This run is representative ARM64 lab evidence, not a production bare-metal SLA.

## Toolchain and host metadata

### toolchain_versions.txt

```text
architecture=aarch64
kernel=7.0.11-orbstack-00360-gc9bc4d96ac70
compiler=c++ (Ubuntu 15.2.0-16ubuntu1) 15.2.0
cmake=cmake version 4.4.2
buf=1.72.0
protoc=libprotoc 35.1
python=Python 3.14.4
protobuf_package=35.1
protobuf_c_package=1.5.2
boost_json_package=1.91.0
jsoncpp_package=not-installed
rapidjson_package=1.1.0+dfsg2-7.6ubuntu1
librdkafka=2.15.0
yyjson=0.12.0
nlohmann_json=3.12.0
jsoncpp=1.9.8
clang=Ubuntu clang version 22.1.2 (1ubuntu1)
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
 Images: 11
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

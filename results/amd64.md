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

- [REPORT.md](#reportmd)
- [COMPILER_REPORT.md](#compiler-reportmd)
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
| one_string_ten_decimal_strings | protobuf_c / c_generated / pack_preallocated | 116.36 | 8.57 M/s | 213 |
| one_string_ten_int64 | google_protobuf / speed / SerializeToArray_preallocated | 47.21 | 21.12 M/s | 49 |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated / pack_preallocated | 702.36 | 1.42 M/s | 1193 |

## Full aggregate results

| Kind | Library | Codegen | API | Payload | Reps | Bytes | Median ns/encode | Mean ns/encode | Mean M/s |
|---|---|---|---|---|---:|---:|---:|---:|---:|
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 213 | 116.36 | 116.66 | 8.57 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 139.91 | 140.81 | 7.11 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 140.67 | 142.51 | 7.03 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 143.32 | 143.77 | 6.96 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 144.26 | 146.88 | 6.83 |
| json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 364 | 222.51 | 222.56 | 4.49 |
| json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 419 | 226.96 | 227.09 | 4.40 |
| json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 364 | 748.97 | 751.91 | 1.33 |
| json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 364 | 888.05 | 889.48 | 1.12 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 1,423.92 | 1,429.49 | 0.70 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 1,426.32 | 1,429.72 | 0.70 |
| json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 364 | 1,791.66 | 1,790.86 | 0.56 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 47.21 | 47.37 | 21.12 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 50.01 | 50.11 | 19.96 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 49 | 53.73 | 53.81 | 18.59 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 49 | 54.85 | 55.81 | 17.95 |
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 49 | 82.44 | 82.41 | 12.13 |
| json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 215 | 104.31 | 104.34 | 9.58 |
| json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 210 | 312.70 | 313.53 | 3.19 |
| json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 210 | 370.57 | 370.46 | 2.70 |
| json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 210 | 507.06 | 506.81 | 1.97 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 1,157.83 | 1,160.01 | 0.86 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 49 | 1,158.06 | 1,159.56 | 0.86 |
| json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 210 | 1,618.77 | 1,617.08 | 0.62 |
| protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 702.36 | 706.53 | 1.42 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 763.89 | 762.98 | 1.31 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 765.10 | 763.42 | 1.31 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 781.52 | 783.94 | 1.28 |
| protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 783.68 | 781.81 | 1.28 |
| json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 2279 | 1,011.58 | 1,015.07 | 0.99 |
| json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1979 | 1,112.29 | 1,118.66 | 0.89 |
| json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 1979 | 4,022.24 | 4,029.08 | 0.25 |
| json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 1979 | 4,649.88 | 4,645.99 | 0.22 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 7,304.08 | 7,311.83 | 0.14 |
| protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 7,310.58 | 7,310.01 | 0.14 |
| json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 1979 | 9,682.19 | 9,695.02 | 0.10 |

## Protobuf-only comparison

| Payload | Library / generation | API | Median ns/encode | Relative to fastest protobuf |
|---|---|---|---:|---:|
| one_string_ten_decimal_strings | protobuf_c / c_generated | pack_preallocated | 116.36 | 1.00x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 139.91 | 1.20x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 140.67 | 1.21x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 143.32 | 1.23x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToString | 144.26 | 1.24x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToString | 1,423.92 | 12.24x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 1,426.32 | 12.26x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToArray_preallocated | 47.21 | 1.00x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToArray_preallocated | 50.01 | 1.06x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToString | 53.73 | 1.14x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToString | 54.85 | 1.16x |
| one_string_ten_int64 | protobuf_c / c_generated | pack_preallocated | 82.44 | 1.75x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToArray_preallocated | 1,157.83 | 24.53x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToString | 1,158.06 | 24.53x |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated | pack_preallocated | 702.36 | 1.00x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 763.89 | 1.09x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 765.10 | 1.09x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 781.52 | 1.11x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToString | 783.68 | 1.12x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 7,304.08 | 10.40x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToString | 7,310.58 | 10.41x |

## JSON-only comparison

| Payload | Library | Median ns/encode | Relative to fastest JSON |
|---|---|---:|---:|
| one_string_ten_decimal_strings | boost_json | 222.51 | 1.00x |
| one_string_ten_decimal_strings | yyjson | 226.96 | 1.02x |
| one_string_ten_decimal_strings | rapidjson | 748.97 | 3.37x |
| one_string_ten_decimal_strings | nlohmann_json | 888.05 | 3.99x |
| one_string_ten_decimal_strings | jsoncpp | 1,791.66 | 8.05x |
| one_string_ten_int64 | yyjson | 104.31 | 1.00x |
| one_string_ten_int64 | boost_json | 312.70 | 3.00x |
| one_string_ten_int64 | rapidjson | 370.57 | 3.55x |
| one_string_ten_int64 | nlohmann_json | 507.06 | 4.86x |
| one_string_ten_int64 | jsoncpp | 1,618.77 | 15.52x |
| ten_strings_fifty_decimal_strings | yyjson | 1,011.58 | 1.00x |
| ten_strings_fifty_decimal_strings | boost_json | 1,112.29 | 1.10x |
| ten_strings_fifty_decimal_strings | rapidjson | 4,022.24 | 3.98x |
| ten_strings_fifty_decimal_strings | nlohmann_json | 4,649.88 | 4.60x |
| ten_strings_fifty_decimal_strings | jsoncpp | 9,682.19 | 9.57x |

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
| clang-cxx23 | Clang 22.1.2 C++23 (Ubuntu clang version 22.1.2 (1ubuntu1)) | C++23 | x86_64 | 10 |
| clang-cxx26 | Clang 22.1.2 C++26 (Ubuntu clang version 22.1.2 (1ubuntu1)) | C++26 | x86_64 | 10 |
| gcc-cxx23 | GCC 15 C++23 (g++ (Ubuntu 15.2.0-16ubuntu1) 15.2.0) | C++23 | x86_64 | 10 |
| gcc-cxx26 | GCC 15 C++26 (g++ (Ubuntu 15.2.0-16ubuntu1) 15.2.0) | C++26 | x86_64 | 10 |

## Fastest row per payload and compiler

| Variant | Payload | Fastest implementation | Median ns/encode | Mean M/s |
|---|---|---|---:|---:|
| clang-cxx23 | one_string_ten_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 121.06 | 8.25 |
| clang-cxx23 | one_string_ten_int64 | protobuf / google_protobuf / lite_runtime / SerializeToArray_preallocated | 50.67 | 19.72 |
| clang-cxx23 | ten_strings_fifty_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 703.77 | 1.42 |
| clang-cxx26 | one_string_ten_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 121.58 | 8.26 |
| clang-cxx26 | one_string_ten_int64 | protobuf / google_protobuf / lite_runtime / SerializeToArray_preallocated | 49.12 | 20.24 |
| clang-cxx26 | ten_strings_fifty_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 701.15 | 1.43 |
| gcc-cxx23 | one_string_ten_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 116.95 | 8.55 |
| gcc-cxx23 | one_string_ten_int64 | protobuf / google_protobuf / speed / SerializeToArray_preallocated | 47.51 | 21.01 |
| gcc-cxx23 | ten_strings_fifty_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 716.54 | 1.39 |
| gcc-cxx26 | one_string_ten_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 115.85 | 8.64 |
| gcc-cxx26 | one_string_ten_int64 | protobuf / google_protobuf / speed / SerializeToArray_preallocated | 48.56 | 20.52 |
| gcc-cxx26 | ten_strings_fifty_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 709.69 | 1.41 |

## Full aggregate results

| Variant | Kind | Library | Codegen | API | Payload | Reps | Median ns/encode | Mean ns/encode |
|---|---|---|---|---|---|---:|---:|---:|
| clang-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 121.06 | 121.18 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 140.21 | 140.52 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 141.97 | 142.10 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 145.22 | 144.99 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 145.56 | 145.89 |
| clang-cxx23 | json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 224.23 | 223.89 |
| clang-cxx23 | json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 241.92 | 241.99 |
| clang-cxx23 | json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 1,006.37 | 1,006.11 |
| clang-cxx23 | json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 1,082.68 | 1,086.12 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 1,414.32 | 1,414.39 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 1,420.65 | 1,425.27 |
| clang-cxx23 | json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 1,813.95 | 1,813.65 |
| clang-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 121.58 | 121.12 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 140.52 | 141.06 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 141.25 | 141.67 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 146.62 | 148.63 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 146.99 | 149.22 |
| clang-cxx26 | json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 220.42 | 221.21 |
| clang-cxx26 | json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 241.82 | 241.77 |
| clang-cxx26 | json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 1,002.92 | 1,002.58 |
| clang-cxx26 | json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 1,070.97 | 1,073.36 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 1,414.08 | 1,414.06 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 1,417.55 | 1,417.73 |
| clang-cxx26 | json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 1,786.48 | 1,788.61 |
| gcc-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 116.95 | 116.90 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 136.76 | 136.81 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 139.42 | 139.34 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 142.20 | 142.31 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 144.22 | 146.47 |
| gcc-cxx23 | json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 220.49 | 220.60 |
| gcc-cxx23 | json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 239.55 | 239.68 |
| gcc-cxx23 | json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 754.65 | 756.63 |
| gcc-cxx23 | json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 885.72 | 886.26 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 1,421.00 | 1,421.21 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 1,440.09 | 1,450.75 |
| gcc-cxx23 | json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 1,803.09 | 1,807.38 |
| gcc-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 115.85 | 115.70 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 139.21 | 140.99 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 140.47 | 139.76 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 143.39 | 145.92 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 145.36 | 144.86 |
| gcc-cxx26 | json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 220.62 | 220.82 |
| gcc-cxx26 | json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 243.99 | 244.02 |
| gcc-cxx26 | json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 746.59 | 746.01 |
| gcc-cxx26 | json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 889.64 | 892.47 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 1,429.11 | 1,428.36 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 1,447.33 | 1,450.07 |
| gcc-cxx26 | json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 1,814.76 | 1,818.73 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 50.67 | 50.70 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 52.64 | 52.90 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 53.74 | 53.04 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 55.46 | 55.31 |
| clang-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 83.91 | 84.00 |
| clang-cxx23 | json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 143.32 | 145.87 |
| clang-cxx23 | json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 312.50 | 314.50 |
| clang-cxx23 | json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 458.79 | 459.09 |
| clang-cxx23 | json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 586.75 | 586.83 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 1,156.63 | 1,163.76 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 1,158.54 | 1,163.82 |
| clang-cxx23 | json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 1,633.86 | 1,638.60 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49.12 | 49.40 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 52.62 | 52.65 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 53.67 | 54.95 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 56.23 | 56.38 |
| clang-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 85.78 | 86.10 |
| clang-cxx26 | json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 142.82 | 143.89 |
| clang-cxx26 | json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 311.86 | 314.63 |
| clang-cxx26 | json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 467.58 | 470.92 |
| clang-cxx26 | json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 584.98 | 588.04 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 1,158.10 | 1,158.63 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 1,159.89 | 1,160.64 |
| clang-cxx26 | json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 1,619.71 | 1,626.54 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 47.51 | 47.60 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 50.62 | 51.27 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 52.93 | 53.58 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 55.64 | 55.70 |
| gcc-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 82.58 | 83.07 |
| gcc-cxx23 | json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 104.43 | 105.27 |
| gcc-cxx23 | json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 311.79 | 315.18 |
| gcc-cxx23 | json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 371.71 | 375.17 |
| gcc-cxx23 | json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 506.34 | 507.36 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 1,168.57 | 1,168.44 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 1,172.99 | 1,172.84 |
| gcc-cxx23 | json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 1,626.28 | 1,635.95 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 48.56 | 48.74 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49.22 | 49.52 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 53.79 | 53.98 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 54.88 | 54.98 |
| gcc-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 82.67 | 82.73 |
| gcc-cxx26 | json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 104.28 | 104.52 |
| gcc-cxx26 | json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 312.47 | 312.62 |
| gcc-cxx26 | json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 369.91 | 370.95 |
| gcc-cxx26 | json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 507.55 | 507.68 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 1,160.48 | 1,162.81 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 1,162.22 | 1,162.21 |
| gcc-cxx26 | json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 1,604.38 | 1,605.17 |
| clang-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 703.77 | 703.69 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 768.39 | 770.84 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 769.70 | 770.43 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 770.44 | 772.42 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 776.67 | 779.23 |
| clang-cxx23 | json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 1,005.54 | 1,010.61 |
| clang-cxx23 | json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1,125.09 | 1,125.18 |
| clang-cxx23 | json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 5,303.99 | 5,298.69 |
| clang-cxx23 | json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 5,674.76 | 5,671.66 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 7,267.82 | 7,259.13 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 7,311.51 | 7,292.73 |
| clang-cxx23 | json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 9,651.96 | 9,657.59 |
| clang-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 701.15 | 701.38 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 764.71 | 766.55 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 769.76 | 772.40 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 771.38 | 772.20 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 783.03 | 784.74 |
| clang-cxx26 | json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 1,002.96 | 1,008.25 |
| clang-cxx26 | json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1,127.39 | 1,127.58 |
| clang-cxx26 | json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 5,282.87 | 5,286.23 |
| clang-cxx26 | json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 5,700.42 | 5,691.21 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 7,239.52 | 7,241.36 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 7,285.46 | 7,288.15 |
| clang-cxx26 | json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 9,519.08 | 9,530.49 |
| gcc-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 716.54 | 718.28 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 769.47 | 775.05 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 771.61 | 772.76 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 772.98 | 773.86 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 784.69 | 787.38 |
| gcc-cxx23 | json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 1,017.34 | 1,017.84 |
| gcc-cxx23 | json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1,108.55 | 1,109.20 |
| gcc-cxx23 | json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 4,056.07 | 4,055.79 |
| gcc-cxx23 | json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 4,690.28 | 4,690.95 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 7,298.03 | 7,301.57 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 7,326.49 | 7,316.83 |
| gcc-cxx23 | json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 9,684.86 | 9,689.26 |
| gcc-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 709.69 | 709.82 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 765.11 | 763.82 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 773.87 | 772.11 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 774.27 | 776.63 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 788.22 | 788.52 |
| gcc-cxx26 | json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 1,010.09 | 1,010.05 |
| gcc-cxx26 | json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1,111.37 | 1,113.38 |
| gcc-cxx26 | json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 4,066.44 | 4,064.12 |
| gcc-cxx26 | json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 4,669.48 | 4,664.36 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 7,251.92 | 7,269.99 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 7,287.70 | 7,293.74 |
| gcc-cxx26 | json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 9,649.04 | 9,666.17 |

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
| fixed64_scale_8 | 10 | 99 | 22.69 | 22.79 | 23.59 | 23.59 | 23.59 | 23.59 | 43.88 |
| bytes_128bit_coefficient_scale_8 | 10 | 189 | 49.07 | 49.15 | 49.59 | 49.59 | 49.59 | 49.59 | 20.35 |
| scaled_sint64_scale_8 | 10 | 79 | 58.26 | 58.17 | 59.00 | 59.00 | 59.00 | 59.00 | 17.19 |
| decimal_string | 10 | 169 | 150.75 | 149.83 | 152.14 | 152.14 | 152.14 | 152.14 | 6.67 |

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
| one_string_ten_decimal_strings | serialize_array_arena_message | 10 | 213 | 149.00 | 152.00 | 154.00 | 217.50 | 4,612.50 | 23,091.50 | 0.00 | 0.00 |
| one_string_ten_decimal_strings | serialize_array_fresh_buffer | 10 | 213 | 157.50 | 161.00 | 164.00 | 263.00 | 4,616.00 | 20,104.50 | 1,000,000.00 | 213,000,000.00 |
| one_string_ten_decimal_strings | serialize_array_reuse | 10 | 213 | 149.00 | 152.00 | 154.00 | 211.50 | 4,557.50 | 23,130.50 | 0.00 | 0.00 |
| one_string_ten_decimal_strings | serialize_string_reserved | 10 | 213 | 154.50 | 157.50 | 160.00 | 222.00 | 4,671.00 | 20,561.50 | 0.00 | 0.00 |
| one_string_ten_int64 | serialize_array_arena_message | 10 | 49 | 64.00 | 66.00 | 68.00 | 73.50 | 232.00 | 17,419.50 | 0.00 | 0.00 |
| one_string_ten_int64 | serialize_array_fresh_buffer | 10 | 49 | 72.00 | 75.00 | 78.00 | 140.00 | 336.00 | 13,710.50 | 1,000,000.00 | 49,000,000.00 |
| one_string_ten_int64 | serialize_array_reuse | 10 | 49 | 64.00 | 67.00 | 68.50 | 91.00 | 233.00 | 18,707.50 | 0.00 | 0.00 |
| one_string_ten_int64 | serialize_string_reserved | 10 | 49 | 70.00 | 72.00 | 74.50 | 105.00 | 306.00 | 13,605.00 | 0.00 | 0.00 |
| ten_strings_fifty_decimal_strings | serialize_array_arena_message | 10 | 1193 | 775.50 | 794.00 | 853.00 | 1,973.00 | 6,427.50 | 58,865.50 | 0.00 | 0.00 |
| ten_strings_fifty_decimal_strings | serialize_array_fresh_buffer | 10 | 1193 | 820.50 | 848.00 | 1,027.50 | 5,021.50 | 14,279.00 | 52,315.00 | 1,000,000.00 | 1,193,000,000.00 |
| ten_strings_fifty_decimal_strings | serialize_array_reuse | 10 | 1193 | 774.50 | 796.50 | 811.50 | 1,244.00 | 6,599.50 | 56,635.00 | 0.00 | 0.00 |
| ten_strings_fifty_decimal_strings | serialize_string_reserved | 10 | 1193 | 789.00 | 815.00 | 976.00 | 3,605.00 | 11,330.50 | 52,323.00 | 0.00 | 0.00 |

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
| int64_small_varints | 10 | 29.00 | 46.28 | 46.37 | 21.57 |
| string_short_ascii | 10 | 49.00 | 52.91 | 52.81 | 18.94 |
| string_long_ascii_256 | 10 | 299.00 | 63.15 | 63.31 | 15.80 |
| int64_mixed_distribution | 10 | 76.50 | 67.28 | 67.47 | 14.82 |
| string_multibyte_utf8 | 10 | 67.92 | 75.21 | 77.74 | 12.92 |
| int64_negative_standard_varint | 10 | 119.00 | 85.44 | 85.48 | 11.70 |
| int64_large_varints | 10 | 109.00 | 85.60 | 86.68 | 11.54 |

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
| shared_buffer_mutex | 1 | 10 | 49 | 58.28 | 57.60 | 17.38 |
| shared_buffer_mutex | 2 | 10 | 49 | 338.64 | 339.63 | 2.94 |
| shared_buffer_mutex | 4 | 10 | 49 | 340.18 | 339.31 | 2.95 |
| shared_buffer_mutex | 8 | 10 | 49 | 342.39 | 340.00 | 2.94 |
| shared_readonly | 1 | 10 | 49 | 50.57 | 50.22 | 19.93 |
| shared_readonly | 2 | 10 | 49 | 38.27 | 38.27 | 26.13 |
| shared_readonly | 4 | 10 | 49 | 38.64 | 38.66 | 25.87 |
| shared_readonly | 8 | 10 | 49 | 37.48 | 37.51 | 26.66 |
| thread_local | 1 | 10 | 49 | 51.39 | 51.55 | 19.46 |
| thread_local | 2 | 10 | 49 | 36.75 | 36.93 | 27.08 |
| thread_local | 4 | 10 | 49 | 36.86 | 37.24 | 26.88 |
| thread_local | 8 | 10 | 49 | 37.38 | 39.37 | 25.54 |

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

If events are unavailable under the container host, the output is retained explicitly rather than being presented as a valid counter measurement. Repeat this phase on production Linux hardware with the required perf permissions for authoritative microarchitectural comparisons.


## KAFKA_PRODUCER_REPORT.md

# Kafka producer benchmark

> Encoding plus real librdkafka producer handoff. No decoding benchmark is included.

## Run contract

- Configurations: `78`
- Encodes/messages per repetition: `1000000`
- Repetitions per configuration: `10`
- `enqueue` covers serialization plus the librdkafka `produce()` handoff.
- `flush` covers the remaining producer/broker delivery time for the configured acknowledgement mode.
- `end_to_end` is enqueue plus flush and is the relevant result for this producer-path benchmark.

## Results

| Mode | Acks | Compression | Linger ms | Batch messages | Reps | Bytes | Median enqueue ns/msg | Median end-to-end ns/msg | p95 end-to-end ns/msg | p99 end-to-end ns/msg | Errors |
|---|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| copy | 0 | lz4 | 0 | 1 | 70 | 49 | 41,658.49 | 46,290.02 | 48,107.20 | 48,186.16 | 0 |
| copy | 0 | lz4 | 0 | 100 | 70 | 49 | 728.35 | 729.63 | 1,747.56 | 1,747.56 | 0 |
| copy | 0 | lz4 | 5 | 1 | 70 | 49 | 41,304.78 | 45,896.48 | 48,452.12 | 48,583.95 | 0 |
| copy | 0 | lz4 | 5 | 100 | 70 | 49 | 696.93 | 698.46 | 1,711.20 | 1,711.20 | 0 |
| copy | 0 | none | 0 | 1 | 70 | 49 | 40,110.14 | 44,560.28 | 46,950.59 | 47,002.76 | 0 |
| copy | 0 | none | 0 | 100 | 70 | 49 | 733.77 | 736.24 | 1,749.44 | 1,749.44 | 0 |
| copy | 0 | none | 5 | 1 | 70 | 49 | 40,217.97 | 44,667.77 | 46,433.22 | 46,796.91 | 0 |
| copy | 0 | none | 5 | 100 | 70 | 49 | 700.18 | 705.22 | 1,672.63 | 1,672.63 | 0 |
| copy | 0 | zstd | 0 | 1 | 70 | 49 | 45,233.38 | 50,198.34 | 52,400.52 | 52,534.91 | 0 |
| copy | 0 | zstd | 0 | 100 | 70 | 49 | 944.05 | 1,019.31 | 2,057.16 | 2,057.16 | 0 |
| copy | 0 | zstd | 5 | 1 | 70 | 49 | 45,643.95 | 50,690.92 | 52,322.60 | 52,728.29 | 0 |
| copy | 0 | zstd | 5 | 100 | 70 | 49 | 1,008.99 | 1,093.76 | 2,070.39 | 2,070.39 | 0 |
| copy | 1 | lz4 | 0 | 1 | 70 | 49 | 51,733.10 | 57,480.88 | 104,969.76 | 105,258.88 | 0 |
| copy | 1 | lz4 | 0 | 100 | 70 | 49 | 1,022.08 | 1,121.46 | 2,080.13 | 2,080.13 | 0 |
| copy | 1 | lz4 | 5 | 1 | 70 | 49 | 51,365.04 | 57,174.23 | 104,774.83 | 105,504.24 | 0 |
| copy | 1 | lz4 | 5 | 100 | 70 | 49 | 830.77 | 832.11 | 1,926.44 | 1,926.44 | 0 |
| copy | 1 | none | 0 | 1 | 70 | 49 | 52,146.32 | 58,001.31 | 101,706.15 | 106,112.33 | 0 |
| copy | 1 | none | 0 | 100 | 70 | 49 | 938.44 | 959.16 | 2,043.65 | 2,043.65 | 0 |
| copy | 1 | none | 5 | 1 | 70 | 49 | 52,549.23 | 58,240.31 | 102,820.10 | 103,139.72 | 0 |
| copy | 1 | none | 5 | 100 | 70 | 49 | 831.27 | 833.04 | 1,973.46 | 1,973.46 | 0 |
| copy | 1 | zstd | 0 | 1 | 70 | 49 | 52,290.00 | 58,038.34 | 105,583.12 | 105,692.25 | 0 |
| copy | 1 | zstd | 0 | 100 | 70 | 49 | 1,035.85 | 1,112.08 | 2,220.23 | 2,220.23 | 0 |
| copy | 1 | zstd | 5 | 1 | 70 | 49 | 52,886.68 | 58,702.91 | 106,134.07 | 107,023.60 | 0 |
| copy | 1 | zstd | 5 | 100 | 70 | 49 | 1,175.19 | 1,265.47 | 2,261.29 | 2,261.29 | 0 |
| copy | all | lz4 | 0 | 1 | 70 | 49 | 91,647.54 | 101,810.96 | 102,756.60 | 102,756.60 | 0 |
| copy | all | lz4 | 0 | 100 | 70 | 49 | 1,073.24 | 1,192.60 | 2,980.60 | 2,980.60 | 0 |
| copy | all | lz4 | 5 | 1 | 70 | 49 | 91,682.76 | 101,824.83 | 102,349.32 | 102,349.32 | 0 |
| copy | all | lz4 | 5 | 100 | 70 | 49 | 948.51 | 1,055.45 | 2,090.54 | 2,090.54 | 0 |
| copy | all | none | 0 | 1 | 70 | 49 | 93,186.52 | 103,690.56 | 104,760.47 | 104,760.47 | 0 |
| copy | all | none | 0 | 100 | 70 | 49 | 1,036.36 | 1,148.15 | 2,513.18 | 2,513.18 | 0 |
| copy | all | none | 5 | 1 | 70 | 49 | 90,757.22 | 100,777.84 | 102,084.48 | 102,084.48 | 0 |
| copy | all | none | 5 | 100 | 70 | 49 | 1,014.69 | 1,128.30 | 2,186.83 | 2,186.83 | 0 |
| copy | all | zstd | 0 | 1 | 70 | 49 | 91,538.84 | 101,686.00 | 104,493.01 | 104,493.01 | 0 |
| copy | all | zstd | 0 | 100 | 70 | 49 | 1,168.75 | 1,291.53 | 2,392.55 | 2,392.55 | 0 |
| copy | all | zstd | 5 | 1 | 70 | 49 | 91,546.98 | 101,710.50 | 102,753.03 | 102,753.03 | 0 |
| copy | all | zstd | 5 | 100 | 70 | 49 | 1,303.01 | 1,421.63 | 2,398.78 | 2,398.78 | 0 |
| owned | 0 | lz4 | 0 | 1 | 70 | 49 | 42,495.32 | 47,221.99 | 47,655.31 | 47,655.31 | 0 |
| owned | 0 | lz4 | 0 | 100 | 70 | 49 | 853.75 | 854.59 | 1,772.53 | 1,772.53 | 0 |
| owned | 0 | lz4 | 5 | 1 | 70 | 49 | 42,679.75 | 47,384.08 | 47,901.73 | 47,901.73 | 0 |
| owned | 0 | lz4 | 5 | 100 | 70 | 49 | 725.93 | 726.23 | 1,707.30 | 1,707.30 | 0 |
| owned | 0 | none | 0 | 1 | 70 | 49 | 41,363.58 | 45,932.03 | 48,356.37 | 48,356.37 | 0 |
| owned | 0 | none | 0 | 100 | 70 | 49 | 630.16 | 679.90 | 1,919.16 | 1,919.16 | 0 |
| owned | 0 | none | 5 | 1 | 70 | 49 | 41,105.67 | 45,761.03 | 46,441.96 | 46,441.96 | 0 |
| owned | 0 | none | 5 | 100 | 70 | 49 | 744.76 | 745.36 | 1,662.93 | 1,662.93 | 0 |
| owned | 0 | zstd | 0 | 1 | 70 | 49 | 46,539.84 | 51,678.75 | 52,139.38 | 52,139.38 | 0 |
| owned | 0 | zstd | 0 | 100 | 70 | 49 | 982.35 | 1,061.95 | 2,110.68 | 2,110.68 | 0 |
| owned | 0 | zstd | 5 | 1 | 70 | 49 | 46,575.83 | 51,717.33 | 52,233.66 | 52,233.66 | 0 |
| owned | 0 | zstd | 5 | 100 | 70 | 49 | 991.59 | 1,082.96 | 2,059.08 | 2,059.08 | 0 |
| owned | 1 | lz4 | 0 | 1 | 70 | 49 | 65,805.17 | 73,080.74 | 101,217.85 | 101,587.15 | 0 |
| owned | 1 | lz4 | 0 | 100 | 70 | 49 | 767.17 | 841.94 | 2,044.67 | 2,044.67 | 0 |
| owned | 1 | lz4 | 5 | 1 | 70 | 49 | 66,211.93 | 73,552.50 | 99,784.79 | 100,040.57 | 0 |
| owned | 1 | lz4 | 5 | 100 | 70 | 49 | 814.02 | 860.43 | 1,891.06 | 1,891.06 | 0 |
| owned | 1 | none | 0 | 1 | 70 | 49 | 65,817.48 | 73,128.54 | 99,312.30 | 101,590.37 | 0 |
| owned | 1 | none | 0 | 100 | 70 | 49 | 853.63 | 934.37 | 2,431.78 | 2,431.78 | 0 |
| owned | 1 | none | 5 | 1 | 70 | 49 | 65,945.29 | 73,276.19 | 101,046.67 | 101,471.56 | 0 |
| owned | 1 | none | 5 | 100 | 70 | 49 | 787.09 | 860.92 | 1,874.02 | 1,874.02 | 0 |
| owned | 1 | zstd | 0 | 1 | 70 | 49 | 70,737.06 | 78,554.87 | 101,436.49 | 101,811.41 | 0 |
| owned | 1 | zstd | 0 | 100 | 70 | 49 | 1,105.73 | 1,206.32 | 2,300.14 | 2,300.14 | 0 |
| owned | 1 | zstd | 5 | 1 | 70 | 49 | 70,302.72 | 78,084.88 | 102,074.15 | 103,132.73 | 0 |
| owned | 1 | zstd | 5 | 100 | 70 | 49 | 1,271.64 | 1,377.41 | 2,388.67 | 2,388.67 | 0 |
| owned | all | lz4 | 0 | 1 | 70 | 49 | 88,654.22 | 98,451.93 | 105,025.24 | 105,826.53 | 0 |
| owned | all | lz4 | 0 | 100 | 70 | 49 | 1,082.14 | 1,210.54 | 2,730.08 | 2,730.08 | 0 |
| owned | all | lz4 | 5 | 1 | 70 | 49 | 88,676.33 | 98,533.74 | 104,982.90 | 110,636.89 | 0 |
| owned | all | lz4 | 5 | 100 | 70 | 49 | 938.42 | 1,048.53 | 2,075.54 | 2,075.54 | 0 |
| owned | all | none | 0 | 1 | 70 | 49 | 90,653.79 | 100,672.57 | 103,554.14 | 104,498.42 | 0 |
| owned | all | none | 0 | 100 | 70 | 49 | 1,006.82 | 1,119.82 | 2,536.77 | 2,536.77 | 0 |
| owned | all | none | 5 | 1 | 70 | 49 | 88,412.58 | 98,218.53 | 104,287.13 | 105,270.95 | 0 |
| owned | all | none | 5 | 100 | 70 | 49 | 996.55 | 1,109.37 | 2,147.19 | 2,147.19 | 0 |
| owned | all | zstd | 0 | 1 | 70 | 49 | 91,599.62 | 101,783.06 | 105,499.19 | 109,252.23 | 0 |
| owned | all | zstd | 0 | 100 | 70 | 49 | 1,218.25 | 1,337.62 | 2,403.89 | 2,403.89 | 0 |
| owned | all | zstd | 5 | 1 | 70 | 49 | 90,135.26 | 100,113.45 | 105,801.79 | 107,300.97 | 0 |
| owned | all | zstd | 5 | 100 | 70 | 49 | 1,244.84 | 1,365.33 | 2,514.37 | 2,514.37 | 0 |

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
| one_string_ten_int64 | 47.21 | 51.21 | 53.75 | 59.17 | 55 | 8.48% | 13.86% |
| one_string_ten_decimal_strings | 139.91 | 137.69 | 141.32 | 147.81 | 219 | -1.59% | 1.00% |
| ten_strings_fifty_decimal_strings | 763.89 | 766.77 | 768.54 | 780.52 | 1199 | 0.38% | 0.61% |

## All cached paths

| Payload | API | Reps | Framed bytes | Median ns/encode | Mean M/s | p95 ns |
|---|---|---:|---:|---:|---:|---:|
| one_string_ten_decimal_strings | cached_id_framed_in_place | 10 | 219 | 137.69 | 7,249,250.25 | 140.06 |
| one_string_ten_decimal_strings | cached_id_framed_with_copy | 10 | 219 | 141.32 | 7,081,180.06 | 146.10 |
| one_string_ten_decimal_strings | cached_serializer_string | 10 | 219 | 147.81 | 6,663,703.54 | 176.40 |
| one_string_ten_int64 | cached_id_framed_in_place | 10 | 55 | 51.21 | 19,166,377.57 | 64.11 |
| one_string_ten_int64 | cached_id_framed_with_copy | 10 | 55 | 53.75 | 17,970,485.27 | 66.17 |
| one_string_ten_int64 | cached_serializer_string | 10 | 55 | 59.17 | 16,899,913.64 | 60.34 |
| ten_strings_fifty_decimal_strings | cached_id_framed_in_place | 10 | 1199 | 766.77 | 1,288,107.87 | 819.48 |
| ten_strings_fifty_decimal_strings | cached_id_framed_with_copy | 10 | 1199 | 768.54 | 1,295,567.88 | 787.99 |
| ten_strings_fifty_decimal_strings | cached_serializer_string | 10 | 1199 | 780.52 | 1,273,880.48 | 818.82 |

## Live Schema Registry paths

These measurements include HTTP request/response and Registry processing. `registry_lookup_keepalive` means a keep-alive HTTP lookup; it is not a local schema-ID cache hit. The local cached-ID paths are the steady-state framing rows above. All live rows are control-plane latency measurements, not per-message serializer benchmarks.

| Path | Reps | Status/result | Median ns | Mean ns | p95 ns | Min ns | Max ns | Requests/sec |
|---|---:|---|---:|---:|---:|---:|---:|---:|
| cache_miss_404 | 10 | 404 | 2,647,041.00 | 3,586,423.90 | 9,154,815.00 | 2,257,785.00 | 9,154,815.00 | 278.83 |
| cold_lookup_new_connection | 10 | 1 | 3,089,504.50 | 3,428,958.10 | 5,205,971.00 | 2,512,584.00 | 5,205,971.00 | 291.63 |
| cold_register | 10 | 1 | 13,153,872.50 | 14,646,764.60 | 23,409,630.00 | 11,992,176.00 | 23,409,630.00 | 68.27 |
| concurrent_registration | 10 | 200 | 87,586,109.50 | 87,031,134.00 | 138,431,611.00 | 23,351,665.00 | 138,431,611.00 | 11.49 |
| registry_lookup_keepalive | 10 | 1 | 3,274,084.00 | 4,064,203.60 | 10,946,926.00 | 2,791,578.00 | 10,946,926.00 | 246.05 |
| registry_unavailable | 10 | 0 | 868,421.50 | 1,680,031.20 | 4,765,411.00 | 223,524.00 | 4,765,411.00 | 595.23 |
| retry_failure_then_success | 10 | 200 | 5,215,200.00 | 4,713,027.70 | 5,855,607.00 | 2,458,500.00 | 5,855,607.00 | 212.18 |

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
| one_string_ten_int64 | 47.21 | 49.97 | 51.36 | 61.80 | 55 | 5.85% | 8.81% |
| one_string_ten_decimal_strings | 139.91 | 137.54 | 141.93 | 149.07 | 219 | -1.70% | 1.44% |
| ten_strings_fifty_decimal_strings | 763.89 | 765.41 | 774.70 | 780.84 | 1199 | 0.20% | 1.42% |

## All cached paths

| Payload | API | Reps | Framed bytes | Median ns/encode | Mean M/s | p95 ns |
|---|---|---:|---:|---:|---:|---:|
| one_string_ten_decimal_strings | cached_id_framed_in_place | 10 | 219 | 137.54 | 7,120,007.01 | 163.46 |
| one_string_ten_decimal_strings | cached_id_framed_with_copy | 10 | 219 | 141.93 | 7,044,108.80 | 146.58 |
| one_string_ten_decimal_strings | cached_serializer_string | 10 | 219 | 149.07 | 6,662,558.09 | 158.82 |
| one_string_ten_int64 | cached_id_framed_in_place | 10 | 55 | 49.97 | 19,764,056.69 | 54.00 |
| one_string_ten_int64 | cached_id_framed_with_copy | 10 | 55 | 51.36 | 19,325,912.18 | 54.34 |
| one_string_ten_int64 | cached_serializer_string | 10 | 55 | 61.80 | 15,958,482.41 | 71.59 |
| ten_strings_fifty_decimal_strings | cached_id_framed_in_place | 10 | 1199 | 765.41 | 1,300,649.40 | 790.63 |
| ten_strings_fifty_decimal_strings | cached_id_framed_with_copy | 10 | 1199 | 774.70 | 1,292,225.57 | 792.83 |
| ten_strings_fifty_decimal_strings | cached_serializer_string | 10 | 1199 | 780.84 | 1,274,954.20 | 817.38 |

## Live Schema Registry paths

These measurements include HTTP request/response and Registry processing. `registry_lookup_keepalive` means a keep-alive HTTP lookup; it is not a local schema-ID cache hit. The local cached-ID paths are the steady-state framing rows above. All live rows are control-plane latency measurements, not per-message serializer benchmarks.

| Path | Reps | Status/result | Median ns | Mean ns | p95 ns | Min ns | Max ns | Requests/sec |
|---|---:|---|---:|---:|---:|---:|---:|---:|
| cache_miss_404 | 10 | 404 | 10,041,561.50 | 10,653,952.60 | 17,000,214.00 | 8,083,882.00 | 17,000,214.00 | 93.86 |
| cold_lookup_new_connection | 10 | 1 | 13,093,076.00 | 13,398,865.00 | 16,466,531.00 | 10,699,559.00 | 16,466,531.00 | 74.63 |
| cold_register | 10 | 1 | 14,613,524.00 | 17,753,729.60 | 26,862,145.00 | 12,793,716.00 | 26,862,145.00 | 56.33 |
| concurrent_registration | 10 | 200 | 96,346,192.00 | 97,306,243.70 | 144,511,730.00 | 47,431,488.00 | 144,511,730.00 | 10.28 |
| registry_lookup_keepalive | 10 | 1 | 4,100,424.50 | 7,086,557.30 | 36,754,025.00 | 2,953,406.00 | 36,754,025.00 | 141.11 |
| registry_unavailable | 10 | 0 | 388,124.00 | 1,054,339.10 | 2,748,793.00 | 238,945.00 | 2,748,793.00 | 948.46 |
| retry_failure_then_success | 10 | 200 | 9,541,902.50 | 9,749,355.40 | 13,179,511.00 | 7,484,072.00 | 13,179,511.00 | 102.57 |

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
| lookup_versions | 10 | 200 | 2,509,418.00 | 2,883,019.50 | 6,742,124.00 | 5.0 |
| register_v1 | 10 | 200 | 15,416,434.50 | 38,689,061.20 | 237,610,224.00 | 2400.0 |
| register_v2 | 10 | 200 | 16,131,620.00 | 18,493,193.90 | 31,094,495.00 | 2422.0 |

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

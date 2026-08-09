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

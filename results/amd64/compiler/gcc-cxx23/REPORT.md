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
| one_string_ten_decimal_strings | protobuf_c / c_generated / pack_preallocated | 116.95 | 8.55 M/s | 213 |
| one_string_ten_int64 | google_protobuf / speed / SerializeToArray_preallocated | 47.51 | 21.01 M/s | 49 |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated / pack_preallocated | 716.54 | 1.39 M/s | 1193 |

## Full aggregate results

| Kind | Library | Codegen | API | Payload | Reps | Bytes | Median ns/encode | Mean ns/encode | Mean M/s |
|---|---|---|---|---|---:|---:|---:|---:|---:|
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 213 | 116.95 | 116.90 | 8.55 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 136.76 | 136.81 | 7.31 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 139.42 | 139.34 | 7.18 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 142.20 | 142.31 | 7.03 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 144.22 | 146.47 | 6.84 |
| json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 364 | 220.49 | 220.60 | 4.53 |
| json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 419 | 239.55 | 239.68 | 4.17 |
| json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 364 | 754.65 | 756.63 | 1.32 |
| json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 364 | 885.72 | 886.26 | 1.13 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 1,421.00 | 1,421.21 | 0.70 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 1,440.09 | 1,450.75 | 0.69 |
| json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 364 | 1,803.09 | 1,807.38 | 0.55 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 47.51 | 47.60 | 21.01 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 50.62 | 51.27 | 19.52 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 49 | 52.93 | 53.58 | 18.68 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 49 | 55.64 | 55.70 | 17.96 |
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 49 | 82.58 | 83.07 | 12.04 |
| json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 215 | 104.43 | 105.27 | 9.50 |
| json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 210 | 311.79 | 315.18 | 3.17 |
| json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 210 | 371.71 | 375.17 | 2.67 |
| json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 210 | 506.34 | 507.36 | 1.97 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 1,168.57 | 1,168.44 | 0.86 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 49 | 1,172.99 | 1,172.84 | 0.85 |
| json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 210 | 1,626.28 | 1,635.95 | 0.61 |
| protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 716.54 | 718.28 | 1.39 |
| protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 769.47 | 775.05 | 1.29 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 771.61 | 772.76 | 1.29 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 772.98 | 773.86 | 1.29 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 784.69 | 787.38 | 1.27 |
| json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 2279 | 1,017.34 | 1,017.84 | 0.98 |
| json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1979 | 1,108.55 | 1,109.20 | 0.90 |
| json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 1979 | 4,056.07 | 4,055.79 | 0.25 |
| json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 1979 | 4,690.28 | 4,690.95 | 0.21 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 7,298.03 | 7,301.57 | 0.14 |
| protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 7,326.49 | 7,316.83 | 0.14 |
| json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 1979 | 9,684.86 | 9,689.26 | 0.10 |

## Protobuf-only comparison

| Payload | Library / generation | API | Median ns/encode | Relative to fastest protobuf |
|---|---|---|---:|---:|
| one_string_ten_decimal_strings | protobuf_c / c_generated | pack_preallocated | 116.95 | 1.00x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 136.76 | 1.17x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 139.42 | 1.19x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToString | 142.20 | 1.22x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 144.22 | 1.23x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToString | 1,421.00 | 12.15x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 1,440.09 | 12.31x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToArray_preallocated | 47.51 | 1.00x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToArray_preallocated | 50.62 | 1.07x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToString | 52.93 | 1.11x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToString | 55.64 | 1.17x |
| one_string_ten_int64 | protobuf_c / c_generated | pack_preallocated | 82.58 | 1.74x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToArray_preallocated | 1,168.57 | 24.60x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToString | 1,172.99 | 24.69x |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated | pack_preallocated | 716.54 | 1.00x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToString | 769.47 | 1.07x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 771.61 | 1.08x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 772.98 | 1.08x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 784.69 | 1.10x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 7,298.03 | 10.19x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToString | 7,326.49 | 10.22x |

## JSON-only comparison

| Payload | Library | Median ns/encode | Relative to fastest JSON |
|---|---|---:|---:|
| one_string_ten_decimal_strings | boost_json | 220.49 | 1.00x |
| one_string_ten_decimal_strings | yyjson | 239.55 | 1.09x |
| one_string_ten_decimal_strings | rapidjson | 754.65 | 3.42x |
| one_string_ten_decimal_strings | nlohmann_json | 885.72 | 4.02x |
| one_string_ten_decimal_strings | jsoncpp | 1,803.09 | 8.18x |
| one_string_ten_int64 | yyjson | 104.43 | 1.00x |
| one_string_ten_int64 | boost_json | 311.79 | 2.99x |
| one_string_ten_int64 | rapidjson | 371.71 | 3.56x |
| one_string_ten_int64 | nlohmann_json | 506.34 | 4.85x |
| one_string_ten_int64 | jsoncpp | 1,626.28 | 15.57x |
| ten_strings_fifty_decimal_strings | yyjson | 1,017.34 | 1.00x |
| ten_strings_fifty_decimal_strings | boost_json | 1,108.55 | 1.09x |
| ten_strings_fifty_decimal_strings | rapidjson | 4,056.07 | 3.99x |
| ten_strings_fifty_decimal_strings | nlohmann_json | 4,690.28 | 4.61x |
| ten_strings_fifty_decimal_strings | jsoncpp | 9,684.86 | 9.52x |

## Raw data

- `raw.csv` contains every individual repetition and is the source for the aggregates above.
- Re-run on a quiet, pinned CPU if comparing small differences; CPU frequency, thermal state, compiler, allocator, and container host affect absolute numbers.

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
| one_string_ten_decimal_strings | protobuf_c / c_generated / pack_preallocated | 121.58 | 8.26 M/s | 213 |
| one_string_ten_int64 | google_protobuf / lite_runtime / SerializeToArray_preallocated | 49.12 | 20.24 M/s | 49 |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated / pack_preallocated | 701.15 | 1.43 M/s | 1193 |

## Full aggregate results

| Kind | Library | Codegen | API | Payload | Reps | Bytes | Median ns/encode | Mean ns/encode | Mean M/s |
|---|---|---|---|---|---:|---:|---:|---:|---:|
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 213 | 121.58 | 121.12 | 8.26 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 140.52 | 141.06 | 7.09 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 141.25 | 141.67 | 7.06 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 146.62 | 148.63 | 6.74 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 146.99 | 149.22 | 6.72 |
| json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 364 | 220.42 | 221.21 | 4.52 |
| json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 419 | 241.82 | 241.77 | 4.14 |
| json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 364 | 1,002.92 | 1,002.58 | 1.00 |
| json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 364 | 1,070.97 | 1,073.36 | 0.93 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 1,414.08 | 1,414.06 | 0.71 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 1,417.55 | 1,417.73 | 0.71 |
| json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 364 | 1,786.48 | 1,788.61 | 0.56 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 49.12 | 49.40 | 20.24 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 52.62 | 52.65 | 18.99 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 49 | 53.67 | 54.95 | 18.25 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 49 | 56.23 | 56.38 | 17.74 |
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 49 | 85.78 | 86.10 | 11.62 |
| json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 265 | 142.82 | 143.89 | 6.95 |
| json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 210 | 311.86 | 314.63 | 3.18 |
| json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 210 | 467.58 | 470.92 | 2.12 |
| json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 210 | 584.98 | 588.04 | 1.70 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 1,158.10 | 1,158.63 | 0.86 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 49 | 1,159.89 | 1,160.64 | 0.86 |
| json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 210 | 1,619.71 | 1,626.54 | 0.61 |
| protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 701.15 | 701.38 | 1.43 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 764.71 | 766.55 | 1.30 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 769.76 | 772.40 | 1.29 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 771.38 | 772.20 | 1.30 |
| protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 783.03 | 784.74 | 1.27 |
| json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 2279 | 1,002.96 | 1,008.25 | 0.99 |
| json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1979 | 1,127.39 | 1,127.58 | 0.89 |
| json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 1979 | 5,282.87 | 5,286.23 | 0.19 |
| json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 1979 | 5,700.42 | 5,691.21 | 0.18 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 7,239.52 | 7,241.36 | 0.14 |
| protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 7,285.46 | 7,288.15 | 0.14 |
| json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 1979 | 9,519.08 | 9,530.49 | 0.10 |

## Protobuf-only comparison

| Payload | Library / generation | API | Median ns/encode | Relative to fastest protobuf |
|---|---|---|---:|---:|
| one_string_ten_decimal_strings | protobuf_c / c_generated | pack_preallocated | 121.58 | 1.00x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 140.52 | 1.16x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 141.25 | 1.16x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 146.62 | 1.21x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToString | 146.99 | 1.21x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToString | 1,414.08 | 11.63x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 1,417.55 | 11.66x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToArray_preallocated | 49.12 | 1.00x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToArray_preallocated | 52.62 | 1.07x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToString | 53.67 | 1.09x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToString | 56.23 | 1.14x |
| one_string_ten_int64 | protobuf_c / c_generated | pack_preallocated | 85.78 | 1.75x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToArray_preallocated | 1,158.10 | 23.58x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToString | 1,159.89 | 23.61x |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated | pack_preallocated | 701.15 | 1.00x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 764.71 | 1.09x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 769.76 | 1.10x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 771.38 | 1.10x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToString | 783.03 | 1.12x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 7,239.52 | 10.33x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToString | 7,285.46 | 10.39x |

## JSON-only comparison

| Payload | Library | Median ns/encode | Relative to fastest JSON |
|---|---|---:|---:|
| one_string_ten_decimal_strings | boost_json | 220.42 | 1.00x |
| one_string_ten_decimal_strings | yyjson | 241.82 | 1.10x |
| one_string_ten_decimal_strings | rapidjson | 1,002.92 | 4.55x |
| one_string_ten_decimal_strings | nlohmann_json | 1,070.97 | 4.86x |
| one_string_ten_decimal_strings | jsoncpp | 1,786.48 | 8.11x |
| one_string_ten_int64 | yyjson | 142.82 | 1.00x |
| one_string_ten_int64 | boost_json | 311.86 | 2.18x |
| one_string_ten_int64 | rapidjson | 467.58 | 3.27x |
| one_string_ten_int64 | nlohmann_json | 584.98 | 4.10x |
| one_string_ten_int64 | jsoncpp | 1,619.71 | 11.34x |
| ten_strings_fifty_decimal_strings | yyjson | 1,002.96 | 1.00x |
| ten_strings_fifty_decimal_strings | boost_json | 1,127.39 | 1.12x |
| ten_strings_fifty_decimal_strings | rapidjson | 5,282.87 | 5.27x |
| ten_strings_fifty_decimal_strings | nlohmann_json | 5,700.42 | 5.68x |
| ten_strings_fifty_decimal_strings | jsoncpp | 9,519.08 | 9.49x |

## Raw data

- `raw.csv` contains every individual repetition and is the source for the aggregates above.
- Re-run on a quiet, pinned CPU if comparing small differences; CPU frequency, thermal state, compiler, allocator, and container host affect absolute numbers.

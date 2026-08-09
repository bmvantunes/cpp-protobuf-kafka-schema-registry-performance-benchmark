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
| one_string_ten_decimal_strings | google_protobuf / lite_runtime / SerializeToArray_preallocated | 174.46 | 5.68 M/s | 213 |
| one_string_ten_int64 | google_protobuf / lite_runtime / SerializeToArray_preallocated | 58.37 | 17.12 M/s | 49 |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime / SerializeToArray_preallocated | 922.08 | 1.08 M/s | 1193 |

## Full aggregate results

| Kind | Library | Codegen | API | Payload | Reps | Bytes | Median ns/encode | Mean ns/encode | Mean M/s |
|---|---|---|---|---|---:|---:|---:|---:|---:|
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 174.46 | 176.15 | 5.68 |
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 213 | 180.88 | 181.33 | 5.51 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 182.95 | 182.99 | 5.46 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 247.05 | 254.20 | 3.97 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 314.29 | 337.18 | 3.16 |
| json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 364 | 408.54 | 420.26 | 2.38 |
| json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 419 | 416.29 | 422.93 | 2.37 |
| json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 364 | 1,033.83 | 1,042.69 | 0.96 |
| json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 364 | 1,132.90 | 1,158.53 | 0.86 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 2,424.18 | 2,441.95 | 0.41 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 2,431.49 | 2,452.17 | 0.41 |
| json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 364 | 2,808.41 | 2,825.63 | 0.35 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 58.37 | 58.40 | 17.12 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 49 | 66.25 | 68.74 | 14.62 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 67.65 | 70.69 | 14.29 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 49 | 76.75 | 77.39 | 12.93 |
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 49 | 149.99 | 150.10 | 6.66 |
| json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 215 | 202.02 | 202.23 | 4.94 |
| json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 210 | 457.11 | 462.64 | 2.16 |
| json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 210 | 514.58 | 530.40 | 1.89 |
| json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 210 | 758.57 | 759.39 | 1.32 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 49 | 2,152.33 | 2,198.63 | 0.46 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 2,163.29 | 2,168.68 | 0.46 |
| json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 210 | 2,369.94 | 2,415.96 | 0.41 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 922.08 | 929.64 | 1.08 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 935.01 | 942.85 | 1.06 |
| protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 982.14 | 991.49 | 1.01 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 1,119.91 | 1,234.41 | 0.83 |
| protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 1,452.20 | 1,509.08 | 0.69 |
| json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 2279 | 1,852.26 | 1,950.59 | 0.52 |
| json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1979 | 2,103.26 | 2,131.30 | 0.47 |
| json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 1979 | 5,833.63 | 5,862.60 | 0.17 |
| json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 1979 | 6,676.39 | 6,757.13 | 0.15 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 12,744.32 | 12,777.44 | 0.08 |
| protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 12,854.92 | 14,289.43 | 0.07 |
| json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 1979 | 15,010.94 | 16,233.96 | 0.06 |

## Protobuf-only comparison

| Payload | Library / generation | API | Median ns/encode | Relative to fastest protobuf |
|---|---|---|---:|---:|
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 174.46 | 1.00x |
| one_string_ten_decimal_strings | protobuf_c / c_generated | pack_preallocated | 180.88 | 1.04x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 182.95 | 1.05x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 247.05 | 1.42x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToString | 314.29 | 1.80x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToString | 2,424.18 | 13.90x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 2,431.49 | 13.94x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToArray_preallocated | 58.37 | 1.00x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToString | 66.25 | 1.13x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToArray_preallocated | 67.65 | 1.16x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToString | 76.75 | 1.31x |
| one_string_ten_int64 | protobuf_c / c_generated | pack_preallocated | 149.99 | 2.57x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToString | 2,152.33 | 36.87x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToArray_preallocated | 2,163.29 | 37.06x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 922.08 | 1.00x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 935.01 | 1.01x |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated | pack_preallocated | 982.14 | 1.07x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 1,119.91 | 1.21x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToString | 1,452.20 | 1.57x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 12,744.32 | 13.82x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToString | 12,854.92 | 13.94x |

## JSON-only comparison

| Payload | Library | Median ns/encode | Relative to fastest JSON |
|---|---|---:|---:|
| one_string_ten_decimal_strings | boost_json | 408.54 | 1.00x |
| one_string_ten_decimal_strings | yyjson | 416.29 | 1.02x |
| one_string_ten_decimal_strings | rapidjson | 1,033.83 | 2.53x |
| one_string_ten_decimal_strings | nlohmann_json | 1,132.90 | 2.77x |
| one_string_ten_decimal_strings | jsoncpp | 2,808.41 | 6.87x |
| one_string_ten_int64 | yyjson | 202.02 | 1.00x |
| one_string_ten_int64 | boost_json | 457.11 | 2.26x |
| one_string_ten_int64 | rapidjson | 514.58 | 2.55x |
| one_string_ten_int64 | nlohmann_json | 758.57 | 3.75x |
| one_string_ten_int64 | jsoncpp | 2,369.94 | 11.73x |
| ten_strings_fifty_decimal_strings | yyjson | 1,852.26 | 1.00x |
| ten_strings_fifty_decimal_strings | boost_json | 2,103.26 | 1.14x |
| ten_strings_fifty_decimal_strings | nlohmann_json | 5,833.63 | 3.15x |
| ten_strings_fifty_decimal_strings | rapidjson | 6,676.39 | 3.60x |
| ten_strings_fifty_decimal_strings | jsoncpp | 15,010.94 | 8.10x |

## Raw data

- `raw.csv` contains every individual repetition and is the source for the aggregates above.
- Re-run on a quiet, pinned CPU if comparing small differences; CPU frequency, thermal state, compiler, allocator, and container host affect absolute numbers.

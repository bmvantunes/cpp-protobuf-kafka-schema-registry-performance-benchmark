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
| one_string_ten_decimal_strings | protobuf_c / c_generated / pack_preallocated | 115.85 | 8.64 M/s | 213 |
| one_string_ten_int64 | google_protobuf / speed / SerializeToArray_preallocated | 48.56 | 20.54 M/s | 49 |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated / pack_preallocated | 709.69 | 1.41 M/s | 1193 |

## Full aggregate results

| Kind | Library | Codegen | API | Payload | Reps | Bytes | Median ns/encode | Mean ns/encode | Mean M/s |
|---|---|---|---|---|---:|---:|---:|---:|---:|
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 213 | 115.85 | 115.70 | 8.64 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 139.21 | 140.99 | 7.10 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 140.47 | 139.76 | 7.16 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 143.39 | 145.92 | 6.87 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 145.36 | 144.86 | 6.91 |
| json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 364 | 220.62 | 220.82 | 4.53 |
| json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 419 | 243.99 | 244.02 | 4.10 |
| json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 364 | 746.59 | 746.01 | 1.34 |
| json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 364 | 889.64 | 892.47 | 1.12 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 1,429.11 | 1,428.36 | 0.70 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 1,447.33 | 1,450.07 | 0.69 |
| json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 364 | 1,814.76 | 1,818.73 | 0.55 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 48.56 | 48.74 | 20.54 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 49.22 | 49.52 | 20.20 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 49 | 53.79 | 53.98 | 18.55 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 49 | 54.88 | 54.98 | 18.19 |
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 49 | 82.67 | 82.73 | 12.09 |
| json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 215 | 104.28 | 104.52 | 9.57 |
| json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 210 | 312.47 | 312.62 | 3.20 |
| json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 210 | 369.91 | 370.95 | 2.70 |
| json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 210 | 507.55 | 507.68 | 1.97 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 1,160.48 | 1,162.81 | 0.86 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 49 | 1,162.22 | 1,162.21 | 0.86 |
| json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 210 | 1,604.38 | 1,605.17 | 0.62 |
| protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 709.69 | 709.82 | 1.41 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 765.11 | 763.82 | 1.31 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 773.87 | 772.11 | 1.30 |
| protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 774.27 | 776.63 | 1.29 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 788.22 | 788.52 | 1.27 |
| json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 2279 | 1,010.09 | 1,010.05 | 0.99 |
| json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1979 | 1,111.37 | 1,113.38 | 0.90 |
| json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 1979 | 4,066.44 | 4,064.12 | 0.25 |
| json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 1979 | 4,669.48 | 4,664.36 | 0.21 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 7,251.92 | 7,269.99 | 0.14 |
| protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 7,287.70 | 7,293.74 | 0.14 |
| json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 1979 | 9,649.04 | 9,666.17 | 0.10 |

## Protobuf-only comparison

| Payload | Library / generation | API | Median ns/encode | Relative to fastest protobuf |
|---|---|---|---:|---:|
| one_string_ten_decimal_strings | protobuf_c / c_generated | pack_preallocated | 115.85 | 1.00x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 139.21 | 1.20x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 140.47 | 1.21x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 143.39 | 1.24x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToString | 145.36 | 1.25x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 1,429.11 | 12.34x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToString | 1,447.33 | 12.49x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToArray_preallocated | 48.56 | 1.00x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToArray_preallocated | 49.22 | 1.01x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToString | 53.79 | 1.11x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToString | 54.88 | 1.13x |
| one_string_ten_int64 | protobuf_c / c_generated | pack_preallocated | 82.67 | 1.70x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToArray_preallocated | 1,160.48 | 23.90x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToString | 1,162.22 | 23.93x |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated | pack_preallocated | 709.69 | 1.00x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 765.11 | 1.08x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 773.87 | 1.09x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToString | 774.27 | 1.09x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 788.22 | 1.11x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 7,251.92 | 10.22x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToString | 7,287.70 | 10.27x |

## JSON-only comparison

| Payload | Library | Median ns/encode | Relative to fastest JSON |
|---|---|---:|---:|
| one_string_ten_decimal_strings | boost_json | 220.62 | 1.00x |
| one_string_ten_decimal_strings | yyjson | 243.99 | 1.11x |
| one_string_ten_decimal_strings | rapidjson | 746.59 | 3.38x |
| one_string_ten_decimal_strings | nlohmann_json | 889.64 | 4.03x |
| one_string_ten_decimal_strings | jsoncpp | 1,814.76 | 8.23x |
| one_string_ten_int64 | yyjson | 104.28 | 1.00x |
| one_string_ten_int64 | boost_json | 312.47 | 3.00x |
| one_string_ten_int64 | rapidjson | 369.91 | 3.55x |
| one_string_ten_int64 | nlohmann_json | 507.55 | 4.87x |
| one_string_ten_int64 | jsoncpp | 1,604.38 | 15.39x |
| ten_strings_fifty_decimal_strings | yyjson | 1,010.09 | 1.00x |
| ten_strings_fifty_decimal_strings | boost_json | 1,111.37 | 1.10x |
| ten_strings_fifty_decimal_strings | rapidjson | 4,066.44 | 4.03x |
| ten_strings_fifty_decimal_strings | nlohmann_json | 4,669.48 | 4.62x |
| ten_strings_fifty_decimal_strings | jsoncpp | 9,649.04 | 9.55x |

## Raw data

- `raw.csv` contains every individual repetition and is the source for the aggregates above.
- Re-run on a quiet, pinned CPU if comparing small differences; CPU frequency, thermal state, compiler, allocator, and container host affect absolute numbers.

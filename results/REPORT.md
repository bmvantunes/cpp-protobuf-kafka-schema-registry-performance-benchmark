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

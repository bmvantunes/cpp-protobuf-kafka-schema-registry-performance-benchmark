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
| one_string_ten_decimal_strings | protobuf_c / c_generated / pack_preallocated | 121.06 | 8.25 M/s | 213 |
| one_string_ten_int64 | google_protobuf / lite_runtime / SerializeToArray_preallocated | 50.67 | 19.74 M/s | 49 |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated / pack_preallocated | 703.77 | 1.42 M/s | 1193 |

## Full aggregate results

| Kind | Library | Codegen | API | Payload | Reps | Bytes | Median ns/encode | Mean ns/encode | Mean M/s |
|---|---|---|---|---|---:|---:|---:|---:|---:|
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 213 | 121.06 | 121.18 | 8.25 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 140.21 | 140.52 | 7.12 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 141.97 | 142.10 | 7.04 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 145.22 | 144.99 | 6.90 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 145.56 | 145.89 | 6.86 |
| json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 364 | 224.23 | 223.89 | 4.47 |
| json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 419 | 241.92 | 241.99 | 4.13 |
| json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 364 | 1,006.37 | 1,006.11 | 0.99 |
| json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 364 | 1,082.68 | 1,086.12 | 0.92 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 1,414.32 | 1,414.39 | 0.71 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 1,420.65 | 1,425.27 | 0.70 |
| json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 364 | 1,813.95 | 1,813.65 | 0.55 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 50.67 | 50.70 | 19.74 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 49 | 52.64 | 52.90 | 18.92 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 53.74 | 53.04 | 18.87 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 49 | 55.46 | 55.31 | 18.09 |
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 49 | 83.91 | 84.00 | 11.91 |
| json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 265 | 143.32 | 145.87 | 6.87 |
| json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 210 | 312.50 | 314.50 | 3.18 |
| json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 210 | 458.79 | 459.09 | 2.18 |
| json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 210 | 586.75 | 586.83 | 1.70 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 1,156.63 | 1,163.76 | 0.86 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 49 | 1,158.54 | 1,163.82 | 0.86 |
| json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 210 | 1,633.86 | 1,638.60 | 0.61 |
| protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 703.77 | 703.69 | 1.42 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 768.39 | 770.84 | 1.30 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 769.70 | 770.43 | 1.30 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 770.44 | 772.42 | 1.29 |
| protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 776.67 | 779.23 | 1.28 |
| json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 2279 | 1,005.54 | 1,010.61 | 0.99 |
| json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1979 | 1,125.09 | 1,125.18 | 0.89 |
| json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 1979 | 5,303.99 | 5,298.69 | 0.19 |
| json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 1979 | 5,674.76 | 5,671.66 | 0.18 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 7,267.82 | 7,259.13 | 0.14 |
| protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 7,311.51 | 7,292.73 | 0.14 |
| json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 1979 | 9,651.96 | 9,657.59 | 0.10 |

## Protobuf-only comparison

| Payload | Library / generation | API | Median ns/encode | Relative to fastest protobuf |
|---|---|---|---:|---:|
| one_string_ten_decimal_strings | protobuf_c / c_generated | pack_preallocated | 121.06 | 1.00x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 140.21 | 1.16x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 141.97 | 1.17x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 145.22 | 1.20x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToString | 145.56 | 1.20x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToString | 1,414.32 | 11.68x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 1,420.65 | 11.74x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToArray_preallocated | 50.67 | 1.00x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToString | 52.64 | 1.04x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToArray_preallocated | 53.74 | 1.06x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToString | 55.46 | 1.09x |
| one_string_ten_int64 | protobuf_c / c_generated | pack_preallocated | 83.91 | 1.66x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToArray_preallocated | 1,156.63 | 22.83x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToString | 1,158.54 | 22.87x |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated | pack_preallocated | 703.77 | 1.00x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 768.39 | 1.09x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 769.70 | 1.09x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 770.44 | 1.09x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToString | 776.67 | 1.10x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 7,267.82 | 10.33x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToString | 7,311.51 | 10.39x |

## JSON-only comparison

| Payload | Library | Median ns/encode | Relative to fastest JSON |
|---|---|---:|---:|
| one_string_ten_decimal_strings | boost_json | 224.23 | 1.00x |
| one_string_ten_decimal_strings | yyjson | 241.92 | 1.08x |
| one_string_ten_decimal_strings | rapidjson | 1,006.37 | 4.49x |
| one_string_ten_decimal_strings | nlohmann_json | 1,082.68 | 4.83x |
| one_string_ten_decimal_strings | jsoncpp | 1,813.95 | 8.09x |
| one_string_ten_int64 | yyjson | 143.32 | 1.00x |
| one_string_ten_int64 | boost_json | 312.50 | 2.18x |
| one_string_ten_int64 | rapidjson | 458.79 | 3.20x |
| one_string_ten_int64 | nlohmann_json | 586.75 | 4.09x |
| one_string_ten_int64 | jsoncpp | 1,633.86 | 11.40x |
| ten_strings_fifty_decimal_strings | yyjson | 1,005.54 | 1.00x |
| ten_strings_fifty_decimal_strings | boost_json | 1,125.09 | 1.12x |
| ten_strings_fifty_decimal_strings | rapidjson | 5,303.99 | 5.27x |
| ten_strings_fifty_decimal_strings | nlohmann_json | 5,674.76 | 5.64x |
| ten_strings_fifty_decimal_strings | jsoncpp | 9,651.96 | 9.60x |

## Raw data

- `raw.csv` contains every individual repetition and is the source for the aggregates above.
- Re-run on a quiet, pinned CPU if comparing small differences; CPU frequency, thermal state, compiler, allocator, and container host affect absolute numbers.

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
| one_string_ten_decimal_strings | protobuf_c / c_generated / pack_preallocated | 98.94 | 9.69 M/s | 213 |
| one_string_ten_int64 | google_protobuf / lite_runtime / SerializeToArray_preallocated | 77.26 | 12.88 M/s | 49 |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated / pack_preallocated | 520.67 | 1.94 M/s | 1193 |

## Full aggregate results

| Kind | Library | Codegen | API | Payload | Reps | Bytes | Median ns/encode | Mean ns/encode | Mean M/s |
|---|---|---|---|---|---:|---:|---:|---:|---:|
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 213 | 98.94 | 104.25 | 9.69 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 148.72 | 149.95 | 6.67 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 150.86 | 152.84 | 6.55 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 154.46 | 156.68 | 6.39 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 156.00 | 161.19 | 6.23 |
| json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 414 | 223.44 | 226.13 | 4.43 |
| json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 364 | 311.93 | 316.13 | 3.17 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 1,069.04 | 1,098.56 | 0.91 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 1,077.77 | 1,079.32 | 0.93 |
| json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 364 | 1,150.09 | 1,153.73 | 0.87 |
| json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 364 | 1,193.51 | 1,193.45 | 0.84 |
| json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 364 | 2,270.36 | 2,295.65 | 0.44 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 77.26 | 77.68 | 12.88 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 82.47 | 82.84 | 12.07 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 49 | 84.30 | 84.83 | 11.81 |
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 49 | 85.70 | 89.38 | 11.32 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 49 | 87.25 | 87.69 | 11.41 |
| json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 210 | 87.91 | 90.03 | 11.13 |
| json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 210 | 274.17 | 275.53 | 3.63 |
| json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 210 | 377.03 | 380.79 | 2.63 |
| json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 210 | 675.18 | 675.90 | 1.48 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 851.99 | 871.54 | 1.15 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 49 | 864.27 | 894.08 | 1.12 |
| json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 210 | 1,750.40 | 1,768.68 | 0.57 |
| protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 520.67 | 516.92 | 1.94 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 606.25 | 611.41 | 1.64 |
| protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 612.99 | 616.04 | 1.62 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 658.03 | 657.98 | 1.52 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 662.41 | 672.29 | 1.49 |
| json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 2229 | 752.18 | 757.06 | 1.32 |
| json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1979 | 1,479.74 | 1,489.01 | 0.67 |
| protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 5,516.89 | 5,554.96 | 0.18 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 5,795.95 | 5,763.61 | 0.17 |
| json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 1979 | 5,985.25 | 6,163.27 | 0.16 |
| json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 1979 | 6,529.96 | 6,567.27 | 0.15 |
| json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 1979 | 11,261.72 | 11,275.36 | 0.09 |

## Protobuf-only comparison

| Payload | Library / generation | API | Median ns/encode | Relative to fastest protobuf |
|---|---|---|---:|---:|
| one_string_ten_decimal_strings | protobuf_c / c_generated | pack_preallocated | 98.94 | 1.00x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 148.72 | 1.50x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 150.86 | 1.52x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToString | 154.46 | 1.56x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 156.00 | 1.58x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 1,069.04 | 10.81x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToString | 1,077.77 | 10.89x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToArray_preallocated | 77.26 | 1.00x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToArray_preallocated | 82.47 | 1.07x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToString | 84.30 | 1.09x |
| one_string_ten_int64 | protobuf_c / c_generated | pack_preallocated | 85.70 | 1.11x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToString | 87.25 | 1.13x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToArray_preallocated | 851.99 | 11.03x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToString | 864.27 | 11.19x |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated | pack_preallocated | 520.67 | 1.00x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 606.25 | 1.16x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToString | 612.99 | 1.18x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 658.03 | 1.26x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 662.41 | 1.27x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToString | 5,516.89 | 10.60x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 5,795.95 | 11.13x |

## JSON-only comparison

| Payload | Library | Median ns/encode | Relative to fastest JSON |
|---|---|---:|---:|
| one_string_ten_decimal_strings | yyjson | 223.44 | 1.00x |
| one_string_ten_decimal_strings | boost_json | 311.93 | 1.40x |
| one_string_ten_decimal_strings | rapidjson | 1,150.09 | 5.15x |
| one_string_ten_decimal_strings | nlohmann_json | 1,193.51 | 5.34x |
| one_string_ten_decimal_strings | jsoncpp | 2,270.36 | 10.16x |
| one_string_ten_int64 | yyjson | 87.91 | 1.00x |
| one_string_ten_int64 | boost_json | 274.17 | 3.12x |
| one_string_ten_int64 | rapidjson | 377.03 | 4.29x |
| one_string_ten_int64 | nlohmann_json | 675.18 | 7.68x |
| one_string_ten_int64 | jsoncpp | 1,750.40 | 19.91x |
| ten_strings_fifty_decimal_strings | yyjson | 752.18 | 1.00x |
| ten_strings_fifty_decimal_strings | boost_json | 1,479.74 | 1.97x |
| ten_strings_fifty_decimal_strings | nlohmann_json | 5,985.25 | 7.96x |
| ten_strings_fifty_decimal_strings | rapidjson | 6,529.96 | 8.68x |
| ten_strings_fifty_decimal_strings | jsoncpp | 11,261.72 | 14.97x |

## Raw data

- `raw.csv` contains every individual repetition and is the source for the aggregates above.
- Re-run on a quiet, pinned CPU if comparing small differences; CPU frequency, thermal state, compiler, allocator, and container host affect absolute numbers.

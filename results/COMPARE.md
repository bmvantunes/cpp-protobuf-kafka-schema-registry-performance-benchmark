# ARM64 versus native AMD64 protocol comparison

> One aligned report for the completed Docker benchmarks. Every steady-state encoding row uses 1,000,000 operations per repetition and 10 measured repetitions. Lower latency is better.

## Executive summary

| Test payload | ARM64 fastest | ARM64 ns/encode | AMD64 fastest | AMD64 ns/encode | AMD/ARM |
|---|---|---:|---|---:|---:|
| `one_string_ten_int64` | google_protobuf / lite_runtime / SerializeToArray_preallocated | 80.39 | google_protobuf / speed / SerializeToArray_preallocated | 56.13 | 0.70x |
| `one_string_ten_decimal_strings` | google_protobuf / speed / SerializeToString | 160.29 | protobuf_c / c_generated / pack_preallocated | 102.86 | 0.64x |
| `ten_strings_fifty_decimal_strings` | protobuf_c / c_generated / pack_preallocated | 496.38 | protobuf_c / c_generated / pack_preallocated | 623.04 | 1.26x |

The best implementation changes with payload shape: Google protobuf is strongest for the compact int64 message, while protobuf-c is strongest for the largest string-heavy message. yyjson is the fastest JSON implementation in most payloads, but protobuf remains smaller on the wire and generally faster.

## 1. Complete protobuf and JSON protocol matrix

This table includes every generated/API permutation and every JSON library tested. `SPEED`, `CODE_SIZE`, and `LITE_RUNTIME` are Google C++ types generated through `buf generate`; protobuf-c is separately generated C code.

### `one_string_ten_int64`

| Protocol / implementation | Bytes ARM/AMD | ARM64 median ns | AMD64 median ns | AMD/ARM |
|---|---:|---:|---:|---:|
| `json / boost_json / n/a / serialize` | 210 / 210 | 289.74 | 343.81 | 1.19x |
| `json / jsoncpp / n/a / writeString` | 210 / 210 | 2,611.16 | 2,375.79 | 0.91x |
| `json / nlohmann_json / n/a / dump` | 210 / 210 | 729.05 | 563.18 | 0.77x |
| `json / rapidjson / n/a / writer` | 210 / 210 | 382.38 | 443.96 | 1.16x |
| `json / yyjson / n/a / mut_write` | 210 / 215 | 95.66 | 124.69 | 1.30x |
| `protobuf / google_protobuf / code_size / SerializeToArray_preallocated` | 49 / 49 | 897.92 | 1,378.07 | 1.53x |
| `protobuf / google_protobuf / code_size / SerializeToString` | 49 / 49 | 857.79 | 1,375.39 | 1.60x |
| `protobuf / google_protobuf / lite_runtime / SerializeToArray_preallocated` | 49 / 49 | 80.39 | 59.59 | 0.74x |
| `protobuf / google_protobuf / lite_runtime / SerializeToString` | 49 / 49 | 82.35 | 66.45 | 0.81x |
| `protobuf / google_protobuf / speed / SerializeToArray_preallocated` | 49 / 49 | 82.52 | 56.13 | 0.68x |
| `protobuf / google_protobuf / speed / SerializeToString` | 49 / 49 | 87.47 | 64.47 | 0.74x |
| `protobuf / protobuf_c / c_generated / pack_preallocated` | 49 / 49 | 176.61 | 80.94 | 0.46x |

### `one_string_ten_decimal_strings`

| Protocol / implementation | Bytes ARM/AMD | ARM64 median ns | AMD64 median ns | AMD/ARM |
|---|---:|---:|---:|---:|
| `json / boost_json / n/a / serialize` | 364 / 364 | 304.78 | 272.21 | 0.89x |
| `json / jsoncpp / n/a / writeString` | 364 / 364 | 3,048.01 | 2,520.56 | 0.83x |
| `json / nlohmann_json / n/a / dump` | 364 / 364 | 1,413.99 | 1,051.49 | 0.74x |
| `json / rapidjson / n/a / writer` | 364 / 364 | 1,559.01 | 889.61 | 0.57x |
| `json / yyjson / n/a / mut_write` | 414 / 419 | 211.49 | 276.36 | 1.31x |
| `protobuf / google_protobuf / code_size / SerializeToArray_preallocated` | 213 / 213 | 1,220.67 | 1,683.87 | 1.38x |
| `protobuf / google_protobuf / code_size / SerializeToString` | 213 / 213 | 1,131.89 | 1,690.25 | 1.49x |
| `protobuf / google_protobuf / lite_runtime / SerializeToArray_preallocated` | 213 / 213 | 170.56 | 161.00 | 0.94x |
| `protobuf / google_protobuf / lite_runtime / SerializeToString` | 213 / 213 | 171.94 | 168.59 | 0.98x |
| `protobuf / google_protobuf / speed / SerializeToArray_preallocated` | 213 / 213 | 203.28 | 161.64 | 0.80x |
| `protobuf / google_protobuf / speed / SerializeToString` | 213 / 213 | 160.29 | 165.71 | 1.03x |
| `protobuf / protobuf_c / c_generated / pack_preallocated` | 213 / 213 | 267.42 | 102.86 | 0.38x |

### `ten_strings_fifty_decimal_strings`

| Protocol / implementation | Bytes ARM/AMD | ARM64 median ns | AMD64 median ns | AMD/ARM |
|---|---:|---:|---:|---:|
| `json / boost_json / n/a / serialize` | 1979 / 1979 | 1,687.97 | 1,406.87 | 0.83x |
| `json / jsoncpp / n/a / writeString` | 1979 / 1979 | 14,717.33 | 12,631.27 | 0.86x |
| `json / nlohmann_json / n/a / dump` | 1979 / 1979 | 8,324.13 | 5,504.40 | 0.66x |
| `json / rapidjson / n/a / writer` | 1979 / 1979 | 7,862.55 | 4,748.27 | 0.60x |
| `json / yyjson / n/a / mut_write` | 2229 / 2279 | 746.76 | 1,203.38 | 1.61x |
| `protobuf / google_protobuf / code_size / SerializeToArray_preallocated` | 1193 / 1193 | 5,882.74 | 8,658.39 | 1.47x |
| `protobuf / google_protobuf / code_size / SerializeToString` | 1193 / 1193 | 5,954.40 | 8,639.62 | 1.45x |
| `protobuf / google_protobuf / lite_runtime / SerializeToArray_preallocated` | 1193 / 1193 | 641.78 | 906.21 | 1.41x |
| `protobuf / google_protobuf / lite_runtime / SerializeToString` | 1193 / 1193 | 629.42 | 923.97 | 1.47x |
| `protobuf / google_protobuf / speed / SerializeToArray_preallocated` | 1193 / 1193 | 600.22 | 904.73 | 1.51x |
| `protobuf / google_protobuf / speed / SerializeToString` | 1193 / 1193 | 641.62 | 924.16 | 1.44x |
| `protobuf / protobuf_c / c_generated / pack_preallocated` | 1193 / 1193 | 496.38 | 623.04 | 1.26x |

## 2. Compiler and C++ standard comparison

Each row is the fastest implementation for that compiler variant and payload; the complete per-library compiler tables remain in the architecture reports.

| Compiler variant | Payload | ARM64 fastest ns | AMD64 fastest ns | AMD/ARM |
|---|---|---:|---:|---:|
| `clang-cxx23` | `one_string_ten_int64` | 70.09 | 50.67 | 0.72x |
| `clang-cxx23` | `one_string_ten_decimal_strings` | 101.57 | 121.06 | 1.19x |
| `clang-cxx23` | `ten_strings_fifty_decimal_strings` | 518.44 | 703.77 | 1.36x |
| `clang-cxx26` | `one_string_ten_int64` | 72.04 | 49.12 | 0.68x |
| `clang-cxx26` | `one_string_ten_decimal_strings` | 96.24 | 121.58 | 1.26x |
| `clang-cxx26` | `ten_strings_fifty_decimal_strings` | 493.45 | 701.15 | 1.42x |
| `gcc-cxx23` | `one_string_ten_int64` | 82.06 | 47.51 | 0.58x |
| `gcc-cxx23` | `one_string_ten_decimal_strings` | 149.69 | 116.95 | 0.78x |
| `gcc-cxx23` | `ten_strings_fifty_decimal_strings` | 615.05 | 716.54 | 1.17x |
| `gcc-cxx26` | `one_string_ten_int64` | 76.59 | 48.56 | 0.63x |
| `gcc-cxx26` | `one_string_ten_decimal_strings` | 94.70 | 115.85 | 1.22x |
| `gcc-cxx26` | `ten_strings_fifty_decimal_strings` | 491.30 | 709.69 | 1.44x |

## 3. Exact decimal representations

These are protobuf-only representations of exact decimal values; JSON decimal fields were deliberately encoded as strings in the protocol matrix.

| Exact representation | ARM64 bytes | AMD64 bytes | ARM64 median ns | AMD64 median ns | AMD/ARM |
|---|---:|---:|---:|---:|---:|
| `bytes_128bit_coefficient_scale_8` | 189 | 189 | 77.30 | 57.83 | 0.75x |
| `decimal_string` | 169 | 169 | 202.87 | 179.61 | 0.89x |
| `fixed64_scale_8` | 99 | 99 | 58.70 | 27.33 | 0.47x |
| `scaled_sint64_scale_8` | 79 | 79 | 243.30 | 68.35 | 0.28x |

## 4. Allocation and tail latency

Values are instrumented per-encode latency. They should be compared within this phase, not substituted directly for the aggregate throughput benchmark.

| Path | Payload | ARM64 p50 / p99.9 / max ns | AMD64 p50 / p99.9 / max ns | ARM/AMD p50 |
|---|---|---:|---:|---:|
| `serialize_array_arena_message` | `one_string_ten_int64` | 125.00 / 166.00 / 39,501.50 | 74.00 / 105.50 / 22,060.00 | 0.59x |
| `serialize_array_fresh_buffer` | `one_string_ten_int64` | 125.00 / 167.00 / 57,231.00 | 85.00 / 183.00 / 14,737.00 | 0.68x |
| `serialize_array_reuse` | `one_string_ten_int64` | 125.00 / 166.00 / 38,980.00 | 75.50 / 104.50 / 20,657.50 | 0.60x |
| `serialize_string_reserved` | `one_string_ten_int64` | 125.00 / 167.00 / 43,813.50 | 80.00 / 152.50 / 23,240.50 | 0.64x |
| `serialize_array_arena_message` | `one_string_ten_decimal_strings` | 167.00 / 209.00 / 37,022.00 | 176.00 / 243.00 / 25,160.50 | 1.05x |
| `serialize_array_fresh_buffer` | `one_string_ten_decimal_strings` | 208.00 / 250.00 / 59,252.00 | 186.00 / 302.00 / 24,708.00 | 0.89x |
| `serialize_array_reuse` | `one_string_ten_decimal_strings` | 167.00 / 209.00 / 46,501.50 | 177.00 / 247.50 / 34,590.50 | 1.06x |
| `serialize_string_reserved` | `one_string_ten_decimal_strings` | 167.00 / 250.00 / 30,979.50 | 182.00 / 272.00 / 27,227.50 | 1.09x |
| `serialize_array_arena_message` | `ten_strings_fifty_decimal_strings` | 625.00 / 3,312.50 / 94,294.00 | 924.00 / 3,041.00 / 73,066.00 | 1.48x |
| `serialize_array_fresh_buffer` | `ten_strings_fifty_decimal_strings` | 667.00 / 3,458.00 / 125,337.00 | 983.50 / 6,515.00 / 66,064.50 | 1.47x |
| `serialize_array_reuse` | `ten_strings_fifty_decimal_strings` | 625.00 / 3,479.50 / 482,492.00 | 921.00 / 4,867.50 / 65,092.50 | 1.47x |
| `serialize_string_reserved` | `ten_strings_fifty_decimal_strings` | 625.00 / 3,333.50 / 105,065.50 | 937.50 / 3,324.50 / 61,394.50 | 1.50x |

## 5. Realistic value distributions

| Distribution | ARM64 median ns | AMD64 median ns | ARM64 mean bytes | AMD64 mean bytes | AMD/ARM |
|---|---:|---:|---:|---:|---:|
| `int64_large_varints` | 152.15 | 101.78 | 109.00 | 109.00 | 0.67x |
| `int64_mixed_distribution` | 109.39 | 80.32 | 76.50 | 76.50 | 0.73x |
| `int64_negative_standard_varint` | 126.76 | 101.16 | 119.00 | 119.00 | 0.80x |
| `int64_small_varints` | 90.14 | 55.57 | 29.00 | 29.00 | 0.62x |
| `string_long_ascii_256` | 159.14 | 74.17 | 299.00 | 299.00 | 0.47x |
| `string_multibyte_utf8` | 108.46 | 89.53 | 67.92 | 67.92 | 0.83x |
| `string_short_ascii` | 98.96 | 62.42 | 49.00 | 49.00 | 0.63x |

## 6. Concurrency and contention

| Mode | Threads | ARM64 median ns | AMD64 median ns | AMD/ARM |
|---|---:|---:|---:|---:|
| `shared_buffer_mutex` | 1 | 102.77 | 69.84 | 0.68x |
| `shared_buffer_mutex` | 2 | 103.34 | 392.15 | 3.79x |
| `shared_buffer_mutex` | 4 | 104.66 | 396.34 | 3.79x |
| `shared_buffer_mutex` | 8 | 104.79 | 392.21 | 3.74x |
| `shared_readonly` | 1 | 85.40 | 61.47 | 0.72x |
| `shared_readonly` | 2 | 86.68 | 46.23 | 0.53x |
| `shared_readonly` | 4 | 89.31 | 45.29 | 0.51x |
| `shared_readonly` | 8 | 85.43 | 45.34 | 0.53x |
| `thread_local` | 1 | 86.96 | 60.49 | 0.70x |
| `thread_local` | 2 | 84.63 | 43.90 | 0.52x |
| `thread_local` | 4 | 84.72 | 43.93 | 0.52x |
| `thread_local` | 8 | 85.82 | 44.23 | 0.52x |

## 7. Real librdkafka/Kafka producer path

The producer matrix uses the representative 49-byte `one_string_ten_int64` protobuf message and covers ownership, acknowledgements, compression, linger, and batching. Both architectures ran all 72 configurations, each with 10 × 1M messages and zero delivery errors.

| Mode | Acks | Compression | Linger | Batch | ARM64 enqueue / E2E ns/msg | AMD64 enqueue / E2E ns/msg | ARM/AMD E2E | Errors ARM/AMD |
|---|---:|---|---:|---:|---:|---:|---:|---:|
| `copy` | `0` | `lz4` | 0 | 1 | 49,224.71 / 53,941.98 | 41,629.88 / 46,258.35 | 0.86x | 0/0 |
| `copy` | `0` | `lz4` | 0 | 100 | 459.38 / 499.93 | 754.05 / 755.18 | 1.51x | 0/0 |
| `copy` | `0` | `lz4` | 5 | 1 | 41,544.73 / 46,174.51 | 41,304.02 / 45,885.84 | 0.99x | 0/0 |
| `copy` | `0` | `lz4` | 5 | 100 | 507.67 / 507.76 | 700.09 / 701.36 | 1.38x | 0/0 |
| `copy` | `0` | `none` | 0 | 1 | 42,456.27 / 47,609.74 | 40,093.94 / 44,525.34 | 0.94x | 0/0 |
| `copy` | `0` | `none` | 0 | 100 | 494.98 / 551.97 | 735.34 / 742.09 | 1.34x | 0/0 |
| `copy` | `0` | `none` | 5 | 1 | 42,169.04 / 46,845.21 | 40,204.99 / 44,660.04 | 0.95x | 0/0 |
| `copy` | `0` | `none` | 5 | 100 | 702.68 / 788.12 | 709.94 / 713.13 | 0.90x | 0/0 |
| `copy` | `0` | `zstd` | 0 | 1 | 42,540.86 / 47,125.46 | 45,230.20 / 50,190.51 | 1.07x | 0/0 |
| `copy` | `0` | `zstd` | 0 | 100 | 607.80 / 610.78 | 937.71 / 1,015.43 | 1.66x | 0/0 |
| `copy` | `0` | `zstd` | 5 | 1 | 44,322.25 / 49,025.99 | 45,618.95 / 50,657.31 | 1.03x | 0/0 |
| `copy` | `0` | `zstd` | 5 | 100 | 610.41 / 611.21 | 1,002.78 / 1,088.69 | 1.78x | 0/0 |
| `copy` | `1` | `lz4` | 0 | 1 | 62,735.25 / 70,327.37 | 51,638.60 / 57,380.82 | 0.82x | 0/0 |
| `copy` | `1` | `lz4` | 0 | 100 | 934.03 / 1,002.92 | 1,023.18 / 1,122.31 | 1.12x | 0/0 |
| `copy` | `1` | `lz4` | 5 | 1 | 65,931.82 / 73,576.38 | 51,344.52 / 57,139.50 | 0.78x | 0/0 |
| `copy` | `1` | `lz4` | 5 | 100 | 557.53 / 623.03 | 827.73 / 829.38 | 1.33x | 0/0 |
| `copy` | `1` | `none` | 0 | 1 | 62,526.03 / 68,981.85 | 52,118.34 / 57,973.83 | 0.84x | 0/0 |
| `copy` | `1` | `none` | 0 | 100 | 658.41 / 738.26 | 937.08 / 956.49 | 1.30x | 0/0 |
| `copy` | `1` | `none` | 5 | 1 | 70,822.42 / 81,161.24 | 52,444.83 / 58,187.32 | 0.72x | 0/0 |
| `copy` | `1` | `none` | 5 | 100 | 595.26 / 669.15 | 831.13 / 832.50 | 1.24x | 0/0 |
| `copy` | `1` | `zstd` | 0 | 1 | 63,616.55 / 70,028.15 | 52,259.43 / 58,031.43 | 0.83x | 0/0 |
| `copy` | `1` | `zstd` | 0 | 100 | 649.03 / 700.72 | 1,034.47 / 1,107.25 | 1.58x | 0/0 |
| `copy` | `1` | `zstd` | 5 | 1 | 59,351.92 / 66,071.35 | 52,800.79 / 58,679.68 | 0.89x | 0/0 |
| `copy` | `1` | `zstd` | 5 | 100 | 649.31 / 714.76 | 1,173.98 / 1,264.62 | 1.77x | 0/0 |
| `copy` | `all` | `lz4` | 0 | 1 | 60,738.07 / 67,908.32 | 91,648.56 / 101,819.15 | 1.50x | 0/0 |
| `copy` | `all` | `lz4` | 0 | 100 | 666.26 / 740.71 | 1,092.96 / 1,231.76 | 1.66x | 0/0 |
| `copy` | `all` | `lz4` | 5 | 1 | 57,704.50 / 64,074.82 | 91,684.72 / 101,871.43 | 1.59x | 0/0 |
| `copy` | `all` | `lz4` | 5 | 100 | 740.27 / 810.28 | 949.19 / 1,057.68 | 1.31x | 0/0 |
| `copy` | `all` | `none` | 0 | 1 | 60,948.96 / 67,358.40 | 93,448.53 / 103,868.92 | 1.54x | 0/0 |
| `copy` | `all` | `none` | 0 | 100 | 677.55 / 747.95 | 1,046.61 / 1,161.62 | 1.55x | 0/0 |
| `copy` | `all` | `none` | 5 | 1 | 60,202.48 / 67,291.16 | 90,757.42 / 100,812.28 | 1.50x | 0/0 |
| `copy` | `all` | `none` | 5 | 100 | 1,010.63 / 1,098.15 | 1,019.66 / 1,131.92 | 1.03x | 0/0 |
| `copy` | `all` | `zstd` | 0 | 1 | 63,044.16 / 69,394.71 | 91,557.01 / 101,691.52 | 1.47x | 0/0 |
| `copy` | `all` | `zstd` | 0 | 100 | 870.99 / 934.84 | 1,168.72 / 1,290.00 | 1.38x | 0/0 |
| `copy` | `all` | `zstd` | 5 | 1 | 73,405.62 / 79,764.85 | 91,572.22 / 101,715.50 | 1.28x | 0/0 |
| `copy` | `all` | `zstd` | 5 | 100 | 645.95 / 678.92 | 1,318.15 / 1,439.49 | 2.12x | 0/0 |
| `owned` | `0` | `lz4` | 0 | 1 | 52,733.74 / 60,202.20 | 42,532.98 / 47,271.20 | 0.79x | 0/0 |
| `owned` | `0` | `lz4` | 0 | 100 | 544.79 / 572.67 | 842.94 / 843.54 | 1.47x | 0/0 |
| `owned` | `0` | `lz4` | 5 | 1 | 55,470.67 / 61,643.35 | 42,689.16 / 47,384.44 | 0.77x | 0/0 |
| `owned` | `0` | `lz4` | 5 | 100 | 619.35 / 635.47 | 725.19 / 725.93 | 1.14x | 0/0 |
| `owned` | `0` | `none` | 0 | 1 | 42,326.07 / 46,690.62 | 41,365.82 / 45,952.09 | 0.98x | 0/0 |
| `owned` | `0` | `none` | 0 | 100 | 694.61 / 740.57 | 629.74 / 674.71 | 0.91x | 0/0 |
| `owned` | `0` | `none` | 5 | 1 | 40,804.85 / 45,120.73 | 41,148.26 / 45,780.68 | 1.01x | 0/0 |
| `owned` | `0` | `none` | 5 | 100 | 548.51 / 552.35 | 743.27 / 743.92 | 1.35x | 0/0 |
| `owned` | `0` | `zstd` | 0 | 1 | 41,045.58 / 45,631.82 | 46,594.87 / 51,750.28 | 1.13x | 0/0 |
| `owned` | `0` | `zstd` | 0 | 100 | 684.11 / 685.48 | 976.41 / 1,061.38 | 1.55x | 0/0 |
| `owned` | `0` | `zstd` | 5 | 1 | 40,628.25 / 45,182.90 | 46,581.52 / 51,722.52 | 1.14x | 0/0 |
| `owned` | `0` | `zstd` | 5 | 100 | 641.97 / 642.45 | 990.00 / 1,074.82 | 1.67x | 0/0 |
| `owned` | `1` | `lz4` | 0 | 1 | 56,643.71 / 62,970.52 | 65,741.31 / 73,029.17 | 1.16x | 0/0 |
| `owned` | `1` | `lz4` | 0 | 100 | 706.72 / 775.90 | 766.57 / 837.95 | 1.08x | 0/0 |
| `owned` | `1` | `lz4` | 5 | 1 | 57,649.83 / 63,957.77 | 66,202.05 / 73,543.47 | 1.15x | 0/0 |
| `owned` | `1` | `lz4` | 5 | 100 | 587.72 / 597.02 | 811.78 / 859.68 | 1.44x | 0/0 |
| `owned` | `1` | `none` | 0 | 1 | 57,541.21 / 63,861.84 | 65,808.71 / 73,097.03 | 1.14x | 0/0 |
| `owned` | `1` | `none` | 0 | 100 | 641.24 / 713.57 | 849.80 / 933.10 | 1.31x | 0/0 |
| `owned` | `1` | `none` | 5 | 1 | 57,169.81 / 63,442.71 | 65,945.18 / 73,226.65 | 1.15x | 0/0 |
| `owned` | `1` | `none` | 5 | 100 | 590.73 / 650.71 | 784.79 / 860.70 | 1.32x | 0/0 |
| `owned` | `1` | `zstd` | 0 | 1 | 55,882.20 / 62,091.71 | 70,727.42 / 78,550.09 | 1.27x | 0/0 |
| `owned` | `1` | `zstd` | 0 | 100 | 695.62 / 718.79 | 1,104.00 / 1,205.05 | 1.68x | 0/0 |
| `owned` | `1` | `zstd` | 5 | 1 | 56,128.70 / 62,384.59 | 70,284.77 / 78,048.89 | 1.25x | 0/0 |
| `owned` | `1` | `zstd` | 5 | 100 | 668.10 / 689.00 | 1,271.39 / 1,375.58 | 2.00x | 0/0 |
| `owned` | `all` | `lz4` | 0 | 1 | 57,615.73 / 64,027.30 | 88,617.48 / 98,448.78 | 1.54x | 0/0 |
| `owned` | `all` | `lz4` | 0 | 100 | 698.13 / 765.36 | 1,072.25 / 1,201.58 | 1.57x | 0/0 |
| `owned` | `all` | `lz4` | 5 | 1 | 58,771.08 / 65,673.72 | 88,658.58 / 98,529.81 | 1.50x | 0/0 |
| `owned` | `all` | `lz4` | 5 | 100 | 974.66 / 1,110.79 | 936.90 / 1,045.58 | 0.94x | 0/0 |
| `owned` | `all` | `none` | 0 | 1 | 57,484.55 / 63,878.59 | 90,565.72 / 100,599.80 | 1.57x | 0/0 |
| `owned` | `all` | `none` | 0 | 100 | 638.75 / 705.66 | 1,006.37 / 1,119.22 | 1.59x | 0/0 |
| `owned` | `all` | `none` | 5 | 1 | 59,177.26 / 66,238.77 | 88,333.05 / 98,157.04 | 1.48x | 0/0 |
| `owned` | `all` | `none` | 5 | 100 | 597.69 / 681.44 | 992.66 / 1,106.45 | 1.62x | 0/0 |
| `owned` | `all` | `zstd` | 0 | 1 | 56,712.02 / 62,964.04 | 91,599.34 / 101,760.01 | 1.62x | 0/0 |
| `owned` | `all` | `zstd` | 0 | 100 | 708.19 / 756.18 | 1,212.98 / 1,332.53 | 1.76x | 0/0 |
| `owned` | `all` | `zstd` | 5 | 1 | 57,712.02 / 64,039.18 | 90,129.53 / 100,107.88 | 1.56x | 0/0 |
| `owned` | `all` | `zstd` | 5 | 100 | 664.91 / 667.73 | 1,243.67 / 1,364.25 | 2.04x | 0/0 |

## 8. Schema Registry cached framing and live paths

The Confluent protobuf prefix is six bytes: magic byte + four-byte schema ID + one-byte message index. Cached rows are steady-state framing; non-payload rows are HTTP/control-plane operations.

### Plain Registry

| Payload / operation | API | ARM64 median ns | AMD64 median ns | AMD/ARM |
|---|---|---:|---:|---:|
| `one_string_ten_decimal_strings` | `cached_id_framed_in_place` | 150.16 | 164.46 | 1.10x |
| `one_string_ten_decimal_strings` | `cached_id_framed_with_copy` | 152.65 | 168.76 | 1.11x |
| `one_string_ten_decimal_strings` | `cached_serializer_string` | 160.72 | 177.55 | 1.10x |
| `one_string_ten_int64` | `cached_id_framed_in_place` | 83.62 | 57.17 | 0.68x |
| `one_string_ten_int64` | `cached_id_framed_with_copy` | 85.86 | 59.18 | 0.69x |
| `one_string_ten_int64` | `cached_serializer_string` | 91.93 | 65.14 | 0.71x |
| `ten_strings_fifty_decimal_strings` | `cached_id_framed_in_place` | 608.63 | 897.20 | 1.47x |
| `ten_strings_fifty_decimal_strings` | `cached_id_framed_with_copy` | 619.08 | 920.17 | 1.49x |
| `ten_strings_fifty_decimal_strings` | `cached_serializer_string` | 628.22 | 935.16 | 1.49x |
| `schema_registry_cache_miss` | `cache_miss_404` | 2,006,081.00 | 3,035,841.00 | 1.51x |
| `schema_registry_concurrent_register` | `concurrent_registration` | 70,311,588.50 | 99,270,014.00 | 1.41x |
| `schema_registry_lookup` | `cold_lookup_new_connection` | 2,845,064.50 | 3,710,801.50 | 1.30x |
| `schema_registry_lookup` | `registry_lookup_keepalive` | 2,162,481.50 | 2,981,710.50 | 1.38x |
| `schema_registry_registration` | `cold_register` | 15,531,444.50 | 17,757,481.00 | 1.14x |
| `schema_registry_retry` | `retry_failure_then_success` | 2,454,365.00 | 5,723,839.00 | 2.33x |
| `schema_registry_unavailable` | `registry_unavailable` | 160,713.50 | 458,754.00 | 2.85x |

### TLS and Basic Auth Registry

| Payload / operation | API | ARM64 median ns | AMD64 median ns | AMD/ARM |
|---|---|---:|---:|---:|
| `one_string_ten_decimal_strings` | `cached_id_framed_in_place` | 148.73 | 162.50 | 1.09x |
| `one_string_ten_decimal_strings` | `cached_id_framed_with_copy` | 155.29 | 166.40 | 1.07x |
| `one_string_ten_decimal_strings` | `cached_serializer_string` | 157.75 | 175.68 | 1.11x |
| `one_string_ten_int64` | `cached_id_framed_in_place` | 82.98 | 57.14 | 0.69x |
| `one_string_ten_int64` | `cached_id_framed_with_copy` | 87.42 | 60.70 | 0.69x |
| `one_string_ten_int64` | `cached_serializer_string` | 90.95 | 65.42 | 0.72x |
| `ten_strings_fifty_decimal_strings` | `cached_id_framed_in_place` | 597.80 | 902.58 | 1.51x |
| `ten_strings_fifty_decimal_strings` | `cached_id_framed_with_copy` | 612.19 | 922.61 | 1.51x |
| `ten_strings_fifty_decimal_strings` | `cached_serializer_string` | 628.60 | 931.33 | 1.48x |
| `schema_registry_cache_miss` | `cache_miss_404` | 6,941,812.50 | 10,882,515.00 | 1.57x |
| `schema_registry_concurrent_register` | `concurrent_registration` | 91,220,799.50 | 119,109,780.00 | 1.31x |
| `schema_registry_lookup` | `cold_lookup_new_connection` | 10,531,336.50 | 15,858,214.00 | 1.51x |
| `schema_registry_lookup` | `registry_lookup_keepalive` | 3,849,698.50 | 5,014,369.00 | 1.30x |
| `schema_registry_registration` | `cold_register` | 15,910,310.50 | 17,231,843.50 | 1.08x |
| `schema_registry_retry` | `retry_failure_then_success` | 7,671,396.50 | 12,411,743.50 | 1.62x |
| `schema_registry_unavailable` | `registry_unavailable` | 146,379.50 | 454,790.50 | 3.11x |

### Schema evolution

| Evolution phase | ARM64 median ms | AMD64 median ms | AMD/ARM |
|---|---:|---:|---:|
| `lookup_versions` | 2.31 | 2.77 | 1.20x |
| `register_v1` | 16.67 | 17.56 | 1.05x |
| `register_v2` | 16.90 | 19.91 | 1.18x |

## 9. Measurement availability

| Phase | ARM64 | Native AMD64 |
|---|---|---|
| Docker steady-state encoding | Complete | Complete |
| Docker Kafka producer matrix | 72/72 configurations, zero errors | 72/72 configurations, zero errors |
| Docker Schema Registry plain/TLS/auth/evolution | Complete | Complete |
| Linux `perf` counters | Unavailable (`perf` exit 127) | Unavailable (`perf` exit 127) |

## 10. Conclusion and recommendation

### What should we use?

1. **Default production choice:** keep Buf as the schema source of truth and use Buf-generated Google C++ `SPEED` types with `SerializeToArray` into a caller-owned, reused buffer. This is the best cross-architecture default, integrates cleanly with Confluent framing, and avoids choosing a different code-generation ecosystem for every payload.
2. **Use protobuf-c selectively:** `pack_preallocated` wins the ARM64 large string-heavy payload and the AMD64 decimal-string and large payloads. Use it when that specific message type is a proven latency bottleneck and the additional C API/integration trade-off is acceptable. It is not universally fastest: Google protobuf wins the compact int64 payload on both architectures.
3. **Do not use `CODE_SIZE` in the HFT hot path:** it was dramatically slower in these tests. `LITE_RUNTIME` can win a particular ARM64 row, but `SPEED` is the safer cross-architecture baseline.
4. **JSON is not the lowest-latency choice here:** yyjson is the strongest JSON option, but protobuf is smaller and faster for the tested Kafka payloads. Keep JSON for interoperability or non-hot paths unless a separate business requirement dominates.

### Schema Registry overhead without Kafka

This is the direct protobuf-versus-Registry-plus-protobuf comparison; Kafka producer handoff, broker acknowledgement, batching, and compression are not included in this table.

| Architecture | Payload | Pure Buf/Google `SPEED` preallocated ns | Cached in-place ns | In-place overhead | Framed + copy ns | Copy overhead | Serializer string ns | String overhead |
|---|---|---:|---:|---:|---:|---:|---:|---:|
| ARM64 | `one_string_ten_int64` | 82.52 | 83.62 | +1.33% | 85.86 | +4.05% | 91.93 | +11.40% |
| ARM64 | `one_string_ten_decimal_strings` | 203.28 | 150.16 | -26.13% | 152.65 | -24.91% | 160.72 | -20.94% |
| ARM64 | `ten_strings_fifty_decimal_strings` | 600.22 | 608.63 | +1.40% | 619.08 | +3.14% | 628.22 | +4.66% |
| AMD64 | `one_string_ten_int64` | 56.13 | 57.17 | +1.85% | 59.18 | +5.44% | 65.14 | +16.05% |
| AMD64 | `one_string_ten_decimal_strings` | 161.64 | 164.46 | +1.74% | 168.76 | +4.40% | 177.55 | +9.84% |
| AMD64 | `ten_strings_fifty_decimal_strings` | 904.73 | 897.20 | -0.83% | 920.17 | +1.71% | 935.16 | +3.36% |

The six-byte Confluent prefix itself is not the expensive part. Cached in-place framing is approximately 0%–2% overhead on the normal non-noisy rows; the copy path is generally low single-digit overhead, and the serializer-string convenience path is higher because it introduces extra string/copy behavior. Negative percentages in isolated rows are measurement noise, not a real Registry speedup.

### Registry control-plane cost

| Live Registry operation | ARM64 median ms | AMD64 median ms | AMD/ARM |
|---|---:|---:|---:|
| `registry_lookup_keepalive` | 2.16 | 2.98 | 1.38x |
| `cold_lookup_new_connection` | 2.85 | 3.71 | 1.30x |
| `cold_register` | 15.53 | 17.76 | 1.14x |
| `cache_miss_404` | 2.01 | 3.04 | 1.51x |
| `retry_failure_then_success` | 2.45 | 5.72 | 2.33x |
| `concurrent_registration` | 70.31 | 99.27 | 1.41x |
| `registry_unavailable` | 0.16 | 0.46 | 2.85x |

These live operations are millisecond-scale control-plane work. Resolve/register the schema during startup, deployment, or controlled recovery, then cache the schema ID locally. A cache miss in the producer hot loop should fail closed or use an explicit non-HFT recovery path; it should not synchronously call Registry.

### Final HFT recommendation

- Use the fastest payload-specific protobuf implementation after validating compiler/allocator behavior on production hardware.
- Resolve and cache the Schema Registry ID before the producer hot path. Cached framing is low-single-digit overhead; live Registry lookup/registration is millisecond-scale and must stay off the message path.
- Batching dominates the local Kafka test. `acks=0` is not durable delivery; choose `acks=1` or `acks=all` from the loss/recovery policy, then validate the choice on the real multi-node cluster.
- Reuse preallocated buffers and avoid fresh output allocation in the hot loop. Shared-buffer mutex contention is visible in the concurrency table.

## 11. Scope and reproducibility

- ARM64: Docker under OrbStack on the local macOS arm64 host.
- AMD64: native x86_64 GitHub Actions runner, Docker workflow run [31284801828](https://github.com/bmvantunes/cpp-protobuf-kafka-schema-registry-performance-benchmark/actions/runs/31284801828).
- Emulated amd64 results are retained separately in [`emulated-amd64.md`](emulated-amd64.md); they are not native AMD64 evidence.
- `perf` counters were unavailable in the container environments, so no cycles/instructions/cache claim is made.
- No bare-metal production Linux host or production multi-node Kafka cluster was available; absolute production SLA numbers still require a final pinned-core run.

See [`arm64.md`](arm64.md), [`amd64.md`](amd64.md), and the raw artifacts for the phase-specific detailed reports.

# Compiler and C++ standard comparison

> Encoding only. No decoding, parsing, Schema Registry, or Kafka network work is included.

## Contract

- Every successful variant uses 1,000,000 encodes per repetition and 10 measured repetitions.
- All variants use the same Docker image, Buf-generated sources, `-O3 -march=native -DNDEBUG`, and pre-populated messages.
- C++23 is the established baseline; C++26 is included where the compiler accepts the standard mode.

## Variants

| Variant | Compiler | Standard | Architecture | Repetitions |
|---|---|---:|---|---:|
| clang-cxx23 | Clang 22.1.2 C++23 (Ubuntu clang version 22.1.2 (1ubuntu1)) | C++23 | aarch64 | 10 |
| clang-cxx26 | Clang 22.1.2 C++26 (Ubuntu clang version 22.1.2 (1ubuntu1)) | C++26 | aarch64 | 10 |
| gcc-cxx23 | GCC 15 C++23 (g++ (Ubuntu 15.2.0-16ubuntu1) 15.2.0) | C++23 | aarch64 | 10 |
| gcc-cxx26 | GCC 15 C++26 (g++ (Ubuntu 15.2.0-16ubuntu1) 15.2.0) | C++26 | aarch64 | 10 |

## Fastest row per payload and compiler

| Variant | Payload | Fastest implementation | Median ns/encode | Mean M/s |
|---|---|---|---:|---:|
| clang-cxx23 | one_string_ten_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 101.57 | 9.77 |
| clang-cxx23 | one_string_ten_int64 | protobuf / google_protobuf / lite_runtime / SerializeToArray_preallocated | 70.09 | 14.25 |
| clang-cxx23 | ten_strings_fifty_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 518.44 | 1.93 |
| clang-cxx26 | one_string_ten_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 96.24 | 10.30 |
| clang-cxx26 | one_string_ten_int64 | protobuf / google_protobuf / lite_runtime / SerializeToArray_preallocated | 72.04 | 13.38 |
| clang-cxx26 | ten_strings_fifty_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 493.45 | 2.03 |
| gcc-cxx23 | one_string_ten_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 149.69 | 6.40 |
| gcc-cxx23 | one_string_ten_int64 | protobuf / protobuf_c / c_generated / pack_preallocated | 82.06 | 12.11 |
| gcc-cxx23 | ten_strings_fifty_decimal_strings | protobuf / google_protobuf / speed / SerializeToArray_preallocated | 615.05 | 1.59 |
| gcc-cxx26 | one_string_ten_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 94.70 | 10.56 |
| gcc-cxx26 | one_string_ten_int64 | protobuf / google_protobuf / lite_runtime / SerializeToArray_preallocated | 76.59 | 13.04 |
| gcc-cxx26 | ten_strings_fifty_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 491.30 | 2.01 |

## Full aggregate results

| Variant | Kind | Library | Codegen | API | Payload | Reps | Median ns/encode | Mean ns/encode |
|---|---|---|---|---|---|---:|---:|---:|
| clang-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 101.57 | 102.37 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 148.75 | 148.55 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 151.32 | 151.61 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 164.65 | 165.20 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 168.66 | 177.00 |
| clang-cxx23 | json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 219.25 | 220.20 |
| clang-cxx23 | json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 280.24 | 285.57 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 1,166.81 | 1,257.95 |
| clang-cxx23 | json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 1,290.89 | 1,301.44 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 1,560.76 | 1,617.89 |
| clang-cxx23 | json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 1,587.35 | 1,609.64 |
| clang-cxx23 | json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 2,950.14 | 2,967.52 |
| clang-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 96.24 | 97.11 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 147.04 | 147.21 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 153.98 | 159.20 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 162.90 | 173.47 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 168.35 | 170.50 |
| clang-cxx26 | json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 219.67 | 219.89 |
| clang-cxx26 | json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 278.72 | 279.70 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 1,069.66 | 1,083.52 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 1,128.64 | 1,146.99 |
| clang-cxx26 | json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 1,870.81 | 2,210.83 |
| clang-cxx26 | json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 2,467.13 | 3,105.11 |
| clang-cxx26 | json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 3,223.87 | 3,920.10 |
| gcc-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 149.69 | 156.15 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 151.65 | 153.35 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 155.78 | 156.21 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 157.31 | 253.11 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 164.21 | 170.24 |
| gcc-cxx23 | json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 224.62 | 249.68 |
| gcc-cxx23 | json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 380.10 | 485.48 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 1,142.10 | 1,149.42 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 1,202.17 | 1,382.40 |
| gcc-cxx23 | json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 1,575.50 | 1,727.76 |
| gcc-cxx23 | json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 1,710.81 | 1,861.84 |
| gcc-cxx23 | json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 3,114.16 | 3,159.95 |
| gcc-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 94.70 | 94.72 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 142.38 | 142.52 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 147.94 | 154.76 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 150.24 | 150.72 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 154.96 | 155.82 |
| gcc-cxx26 | json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 211.06 | 217.86 |
| gcc-cxx26 | json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 288.21 | 293.10 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 1,063.63 | 1,076.54 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 1,087.85 | 1,088.47 |
| gcc-cxx26 | json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 1,180.24 | 1,194.73 |
| gcc-cxx26 | json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 1,182.08 | 1,194.00 |
| gcc-cxx26 | json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 2,850.77 | 2,919.19 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 70.09 | 70.17 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 74.89 | 75.17 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 91.15 | 92.81 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 91.34 | 93.33 |
| clang-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 96.53 | 108.82 |
| clang-cxx23 | json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 134.58 | 138.82 |
| clang-cxx23 | json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 301.98 | 306.29 |
| clang-cxx23 | json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 451.49 | 464.60 |
| clang-cxx23 | json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 755.59 | 757.11 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 937.51 | 958.17 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 1,155.56 | 1,705.66 |
| clang-cxx23 | json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 2,349.01 | 2,373.23 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 72.04 | 74.72 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 81.72 | 82.55 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 85.05 | 85.22 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 95.25 | 94.73 |
| clang-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 96.80 | 101.68 |
| clang-cxx26 | json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 252.02 | 307.19 |
| clang-cxx26 | json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 307.07 | 410.98 |
| clang-cxx26 | json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 427.80 | 440.47 |
| clang-cxx26 | json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 719.17 | 724.50 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 886.94 | 906.27 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 898.46 | 918.61 |
| clang-cxx26 | json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 2,393.96 | 2,876.92 |
| gcc-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 82.06 | 82.56 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 85.04 | 88.84 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 89.09 | 89.35 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 89.39 | 98.31 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 90.87 | 98.55 |
| gcc-cxx23 | json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 92.48 | 92.45 |
| gcc-cxx23 | json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 303.34 | 313.22 |
| gcc-cxx23 | json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 572.02 | 579.57 |
| gcc-cxx23 | json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 672.98 | 683.03 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 1,004.23 | 1,221.48 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 1,104.40 | 1,292.05 |
| gcc-cxx23 | json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 2,295.87 | 2,848.74 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 76.59 | 76.70 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 80.61 | 84.58 |
| gcc-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 80.99 | 81.01 |
| gcc-cxx26 | json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 87.07 | 87.08 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 88.94 | 93.41 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 93.00 | 93.36 |
| gcc-cxx26 | json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 284.21 | 286.00 |
| gcc-cxx26 | json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 372.03 | 376.65 |
| gcc-cxx26 | json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 662.90 | 675.68 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 844.13 | 852.68 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 846.77 | 859.45 |
| gcc-cxx26 | json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 2,147.70 | 2,161.53 |
| clang-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 518.44 | 517.48 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 645.74 | 654.83 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 648.79 | 665.29 |
| clang-cxx23 | json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 803.26 | 811.32 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 815.29 | 806.62 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 881.13 | 875.17 |
| clang-cxx23 | json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1,383.94 | 1,396.50 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 5,611.24 | 5,909.05 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 5,799.23 | 6,186.40 |
| clang-cxx23 | json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 6,312.21 | 6,711.34 |
| clang-cxx23 | json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 7,620.30 | 8,415.63 |
| clang-cxx23 | json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 14,495.96 | 14,520.71 |
| clang-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 493.45 | 493.51 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 615.99 | 809.03 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 623.46 | 631.36 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 653.21 | 668.55 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 661.65 | 663.23 |
| clang-cxx26 | json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 941.28 | 970.99 |
| clang-cxx26 | json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1,424.85 | 1,577.78 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 5,541.22 | 5,558.23 |
| clang-cxx26 | json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 5,972.68 | 5,987.90 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 6,066.22 | 6,980.86 |
| clang-cxx26 | json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 7,494.98 | 7,843.45 |
| clang-cxx26 | json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 15,177.73 | 15,655.13 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 615.05 | 629.41 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 615.78 | 625.36 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 620.05 | 632.70 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 624.49 | 662.73 |
| gcc-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 763.18 | 765.34 |
| gcc-cxx23 | json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 805.28 | 846.40 |
| gcc-cxx23 | json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1,509.66 | 1,527.49 |
| gcc-cxx23 | json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 5,788.82 | 6,083.29 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 5,802.77 | 6,009.62 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 6,008.46 | 6,610.64 |
| gcc-cxx23 | json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 8,555.00 | 9,445.58 |
| gcc-cxx23 | json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 16,479.83 | 17,288.98 |
| gcc-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 491.30 | 498.00 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 595.93 | 606.41 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 607.92 | 610.15 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 607.98 | 616.26 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 617.83 | 621.41 |
| gcc-cxx26 | json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 803.45 | 823.30 |
| gcc-cxx26 | json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1,514.97 | 1,526.72 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 5,470.17 | 5,464.20 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 5,490.99 | 5,489.79 |
| gcc-cxx26 | json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 6,532.13 | 6,907.56 |
| gcc-cxx26 | json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 6,980.36 | 7,119.05 |
| gcc-cxx26 | json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 14,948.90 | 15,335.65 |

## Interpretation

- Compare compiler rows only within the same host architecture and Docker host; compiler and standard effects are smaller than CPU, frequency, allocator, and scheduler variance in many rows.
- A successful C++26 row proves the selected compiler accepted that language mode; it does not imply all generated dependencies have adopted every C++26 feature.
- Raw per-repetition data remains in `results/compiler/<variant>/raw.csv`.

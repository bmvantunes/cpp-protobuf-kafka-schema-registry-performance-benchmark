# Compiler and C++ standard comparison

> Encoding only. No decoding, parsing, Schema Registry, or Kafka network work is included.

## Contract

- Every successful variant uses 1,000,000 encodes per repetition and 10 measured repetitions.
- All variants use the same Docker image, Buf-generated sources, `-O3 -march=native -DNDEBUG`, and pre-populated messages.
- C++23 is the established baseline; C++26 is included where the compiler accepts the standard mode.

## Variants

| Variant | Compiler | Standard | Architecture | Repetitions |
|---|---|---:|---|---:|
| clang-cxx23 | Clang 22.1.2 C++23 (Ubuntu clang version 22.1.2 (1ubuntu1)) | C++23 | x86_64 | 10 |
| clang-cxx26 | Clang 22.1.2 C++26 (Ubuntu clang version 22.1.2 (1ubuntu1)) | C++26 | x86_64 | 10 |
| gcc-cxx23 | GCC 15 C++23 (g++ (Ubuntu 15.2.0-16ubuntu1) 15.2.0) | C++23 | x86_64 | 10 |
| gcc-cxx26 | GCC 15 C++26 (g++ (Ubuntu 15.2.0-16ubuntu1) 15.2.0) | C++26 | x86_64 | 10 |

## Fastest row per payload and compiler

| Variant | Payload | Fastest implementation | Median ns/encode | Mean M/s |
|---|---|---|---:|---:|
| clang-cxx23 | one_string_ten_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 121.06 | 8.25 |
| clang-cxx23 | one_string_ten_int64 | protobuf / google_protobuf / lite_runtime / SerializeToArray_preallocated | 50.67 | 19.72 |
| clang-cxx23 | ten_strings_fifty_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 703.77 | 1.42 |
| clang-cxx26 | one_string_ten_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 121.58 | 8.26 |
| clang-cxx26 | one_string_ten_int64 | protobuf / google_protobuf / lite_runtime / SerializeToArray_preallocated | 49.12 | 20.24 |
| clang-cxx26 | ten_strings_fifty_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 701.15 | 1.43 |
| gcc-cxx23 | one_string_ten_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 116.95 | 8.55 |
| gcc-cxx23 | one_string_ten_int64 | protobuf / google_protobuf / speed / SerializeToArray_preallocated | 47.51 | 21.01 |
| gcc-cxx23 | ten_strings_fifty_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 716.54 | 1.39 |
| gcc-cxx26 | one_string_ten_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 115.85 | 8.64 |
| gcc-cxx26 | one_string_ten_int64 | protobuf / google_protobuf / speed / SerializeToArray_preallocated | 48.56 | 20.52 |
| gcc-cxx26 | ten_strings_fifty_decimal_strings | protobuf / protobuf_c / c_generated / pack_preallocated | 709.69 | 1.41 |

## Full aggregate results

| Variant | Kind | Library | Codegen | API | Payload | Reps | Median ns/encode | Mean ns/encode |
|---|---|---|---|---|---|---:|---:|---:|
| clang-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 121.06 | 121.18 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 140.21 | 140.52 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 141.97 | 142.10 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 145.22 | 144.99 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 145.56 | 145.89 |
| clang-cxx23 | json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 224.23 | 223.89 |
| clang-cxx23 | json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 241.92 | 241.99 |
| clang-cxx23 | json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 1,006.37 | 1,006.11 |
| clang-cxx23 | json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 1,082.68 | 1,086.12 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 1,414.32 | 1,414.39 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 1,420.65 | 1,425.27 |
| clang-cxx23 | json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 1,813.95 | 1,813.65 |
| clang-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 121.58 | 121.12 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 140.52 | 141.06 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 141.25 | 141.67 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 146.62 | 148.63 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 146.99 | 149.22 |
| clang-cxx26 | json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 220.42 | 221.21 |
| clang-cxx26 | json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 241.82 | 241.77 |
| clang-cxx26 | json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 1,002.92 | 1,002.58 |
| clang-cxx26 | json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 1,070.97 | 1,073.36 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 1,414.08 | 1,414.06 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 1,417.55 | 1,417.73 |
| clang-cxx26 | json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 1,786.48 | 1,788.61 |
| gcc-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 116.95 | 116.90 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 136.76 | 136.81 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 139.42 | 139.34 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 142.20 | 142.31 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 144.22 | 146.47 |
| gcc-cxx23 | json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 220.49 | 220.60 |
| gcc-cxx23 | json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 239.55 | 239.68 |
| gcc-cxx23 | json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 754.65 | 756.63 |
| gcc-cxx23 | json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 885.72 | 886.26 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 1,421.00 | 1,421.21 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 1,440.09 | 1,450.75 |
| gcc-cxx23 | json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 1,803.09 | 1,807.38 |
| gcc-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 115.85 | 115.70 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 139.21 | 140.99 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 140.47 | 139.76 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 143.39 | 145.92 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 145.36 | 144.86 |
| gcc-cxx26 | json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 220.62 | 220.82 |
| gcc-cxx26 | json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 243.99 | 244.02 |
| gcc-cxx26 | json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 746.59 | 746.01 |
| gcc-cxx26 | json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 889.64 | 892.47 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 1,429.11 | 1,428.36 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 1,447.33 | 1,450.07 |
| gcc-cxx26 | json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 1,814.76 | 1,818.73 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 50.67 | 50.70 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 52.64 | 52.90 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 53.74 | 53.04 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 55.46 | 55.31 |
| clang-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 83.91 | 84.00 |
| clang-cxx23 | json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 143.32 | 145.87 |
| clang-cxx23 | json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 312.50 | 314.50 |
| clang-cxx23 | json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 458.79 | 459.09 |
| clang-cxx23 | json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 586.75 | 586.83 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 1,156.63 | 1,163.76 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 1,158.54 | 1,163.82 |
| clang-cxx23 | json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 1,633.86 | 1,638.60 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49.12 | 49.40 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 52.62 | 52.65 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 53.67 | 54.95 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 56.23 | 56.38 |
| clang-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 85.78 | 86.10 |
| clang-cxx26 | json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 142.82 | 143.89 |
| clang-cxx26 | json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 311.86 | 314.63 |
| clang-cxx26 | json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 467.58 | 470.92 |
| clang-cxx26 | json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 584.98 | 588.04 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 1,158.10 | 1,158.63 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 1,159.89 | 1,160.64 |
| clang-cxx26 | json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 1,619.71 | 1,626.54 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 47.51 | 47.60 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 50.62 | 51.27 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 52.93 | 53.58 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 55.64 | 55.70 |
| gcc-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 82.58 | 83.07 |
| gcc-cxx23 | json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 104.43 | 105.27 |
| gcc-cxx23 | json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 311.79 | 315.18 |
| gcc-cxx23 | json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 371.71 | 375.17 |
| gcc-cxx23 | json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 506.34 | 507.36 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 1,168.57 | 1,168.44 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 1,172.99 | 1,172.84 |
| gcc-cxx23 | json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 1,626.28 | 1,635.95 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 48.56 | 48.74 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49.22 | 49.52 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 53.79 | 53.98 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 54.88 | 54.98 |
| gcc-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 82.67 | 82.73 |
| gcc-cxx26 | json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 104.28 | 104.52 |
| gcc-cxx26 | json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 312.47 | 312.62 |
| gcc-cxx26 | json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 369.91 | 370.95 |
| gcc-cxx26 | json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 507.55 | 507.68 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 1,160.48 | 1,162.81 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 1,162.22 | 1,162.21 |
| gcc-cxx26 | json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 1,604.38 | 1,605.17 |
| clang-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 703.77 | 703.69 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 768.39 | 770.84 |
| clang-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 769.70 | 770.43 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 770.44 | 772.42 |
| clang-cxx23 | protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 776.67 | 779.23 |
| clang-cxx23 | json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 1,005.54 | 1,010.61 |
| clang-cxx23 | json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1,125.09 | 1,125.18 |
| clang-cxx23 | json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 5,303.99 | 5,298.69 |
| clang-cxx23 | json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 5,674.76 | 5,671.66 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 7,267.82 | 7,259.13 |
| clang-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 7,311.51 | 7,292.73 |
| clang-cxx23 | json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 9,651.96 | 9,657.59 |
| clang-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 701.15 | 701.38 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 764.71 | 766.55 |
| clang-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 769.76 | 772.40 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 771.38 | 772.20 |
| clang-cxx26 | protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 783.03 | 784.74 |
| clang-cxx26 | json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 1,002.96 | 1,008.25 |
| clang-cxx26 | json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1,127.39 | 1,127.58 |
| clang-cxx26 | json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 5,282.87 | 5,286.23 |
| clang-cxx26 | json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 5,700.42 | 5,691.21 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 7,239.52 | 7,241.36 |
| clang-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 7,285.46 | 7,288.15 |
| clang-cxx26 | json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 9,519.08 | 9,530.49 |
| gcc-cxx23 | protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 716.54 | 718.28 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 769.47 | 775.05 |
| gcc-cxx23 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 771.61 | 772.76 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 772.98 | 773.86 |
| gcc-cxx23 | protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 784.69 | 787.38 |
| gcc-cxx23 | json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 1,017.34 | 1,017.84 |
| gcc-cxx23 | json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1,108.55 | 1,109.20 |
| gcc-cxx23 | json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 4,056.07 | 4,055.79 |
| gcc-cxx23 | json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 4,690.28 | 4,690.95 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 7,298.03 | 7,301.57 |
| gcc-cxx23 | protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 7,326.49 | 7,316.83 |
| gcc-cxx23 | json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 9,684.86 | 9,689.26 |
| gcc-cxx26 | protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 709.69 | 709.82 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 765.11 | 763.82 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 773.87 | 772.11 |
| gcc-cxx26 | protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 774.27 | 776.63 |
| gcc-cxx26 | protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 788.22 | 788.52 |
| gcc-cxx26 | json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 1,010.09 | 1,010.05 |
| gcc-cxx26 | json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1,111.37 | 1,113.38 |
| gcc-cxx26 | json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 4,066.44 | 4,064.12 |
| gcc-cxx26 | json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 4,669.48 | 4,664.36 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 7,251.92 | 7,269.99 |
| gcc-cxx26 | protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 7,287.70 | 7,293.74 |
| gcc-cxx26 | json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 9,649.04 | 9,666.17 |

## Interpretation

- Compare compiler rows only within the same host architecture and Docker host; compiler and standard effects are smaller than CPU, frequency, allocator, and scheduler variance in many rows.
- A successful C++26 row proves the selected compiler accepted that language mode; it does not imply all generated dependencies have adopted every C++26 feature.
- Raw per-repetition data remains in `results/compiler/<variant>/raw.csv`.

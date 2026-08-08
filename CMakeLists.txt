cmake_minimum_required(VERSION 3.24)
project(encoding_benchmark LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23 CACHE STRING "C++ language standard used by the benchmark")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)

find_package(Protobuf REQUIRED)
find_package(Boost REQUIRED COMPONENTS json)
find_package(nlohmann_json REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(CURL REQUIRED)

find_path(PROTOBUF_C_INCLUDE_DIR protobuf-c/protobuf-c.h REQUIRED)
find_library(PROTOBUF_C_LIBRARY protobuf-c REQUIRED)
find_path(YYJSON_INCLUDE_DIR yyjson.h REQUIRED)
find_library(YYJSON_LIBRARY yyjson REQUIRED)
find_path(RDKAFKA_INCLUDE_DIR librdkafka/rdkafka.h REQUIRED)
find_library(RDKAFKA_LIBRARY rdkafka REQUIRED)

set(GENERATED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated)
set(GOOGLE_SPEED_CC ${GENERATED_DIR}/google_speed/google_speed/market.pb.cc)
set(GOOGLE_CODESIZE_CC ${GENERATED_DIR}/google_codesize/google_codesize/market.pb.cc)
set(GOOGLE_LITE_CC ${GENERATED_DIR}/google_lite/google_lite/market.pb.cc)
set(PROTOBUF_C_C ${GENERATED_DIR}/protobuf_c/protobuf_c/market.pb-c.c)
set(REPRESENTATIONS_CC ${GENERATED_DIR}/representations/representations/decimal.pb.cc)

foreach(required_file IN ITEMS ${GOOGLE_SPEED_CC} ${GOOGLE_CODESIZE_CC} ${GOOGLE_LITE_CC} ${PROTOBUF_C_C} ${REPRESENTATIONS_CC})
  if(NOT EXISTS ${required_file})
    message(FATAL_ERROR "Missing generated source ${required_file}; run buf generate first")
  endif()
endforeach()

add_executable(encode_benchmark
  src/encode_benchmark.cc
  ${GOOGLE_SPEED_CC}
  ${GOOGLE_CODESIZE_CC}
  ${GOOGLE_LITE_CC}
  ${PROTOBUF_C_C}
)

add_executable(schema_registry_benchmark
  src/schema_registry_benchmark.cc
  ${GOOGLE_SPEED_CC}
)

add_executable(kafka_producer_benchmark
  src/kafka_producer_benchmark.cc
  ${GOOGLE_SPEED_CC}
)

add_executable(representation_benchmark
  src/representation_benchmark.cc
  ${GOOGLE_SPEED_CC}
  ${REPRESENTATIONS_CC}
)

add_executable(latency_benchmark
  src/latency_benchmark.cc
  ${GOOGLE_SPEED_CC}
)

add_executable(distribution_benchmark
  src/distribution_benchmark.cc
  ${GOOGLE_SPEED_CC}
)

add_executable(concurrency_benchmark
  src/concurrency_benchmark.cc
  ${GOOGLE_SPEED_CC}
)

add_executable(schema_evolution_benchmark
  src/schema_evolution_benchmark.cc
)

target_include_directories(encode_benchmark PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${GENERATED_DIR}/google_speed
  ${GENERATED_DIR}/google_codesize
  ${GENERATED_DIR}/google_lite
  ${GENERATED_DIR}/protobuf_c
  ${PROTOBUF_C_INCLUDE_DIR}
  ${YYJSON_INCLUDE_DIR}
)

target_link_libraries(encode_benchmark PRIVATE
  protobuf::libprotobuf
  protobuf::libprotobuf-lite
  ${PROTOBUF_C_LIBRARY}
  Boost::json
  nlohmann_json::nlohmann_json
  JsonCpp::JsonCpp
  ${YYJSON_LIBRARY}
)

target_include_directories(schema_registry_benchmark PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${GENERATED_DIR}/google_speed
)

target_link_libraries(schema_registry_benchmark PRIVATE
  protobuf::libprotobuf
  CURL::libcurl
)

target_include_directories(kafka_producer_benchmark PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${GENERATED_DIR}/google_speed
  ${RDKAFKA_INCLUDE_DIR}
)

target_link_libraries(kafka_producer_benchmark PRIVATE
  protobuf::libprotobuf
  ${RDKAFKA_LIBRARY}
)

target_include_directories(representation_benchmark PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${GENERATED_DIR}/google_speed
  ${GENERATED_DIR}/representations
)

target_link_libraries(representation_benchmark PRIVATE
  protobuf::libprotobuf
)

target_include_directories(latency_benchmark PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${GENERATED_DIR}/google_speed
)

target_link_libraries(latency_benchmark PRIVATE
  protobuf::libprotobuf
)

target_include_directories(distribution_benchmark PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${GENERATED_DIR}/google_speed
)

target_link_libraries(distribution_benchmark PRIVATE
  protobuf::libprotobuf
)

target_include_directories(concurrency_benchmark PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${GENERATED_DIR}/google_speed
)

target_link_libraries(concurrency_benchmark PRIVATE
  protobuf::libprotobuf
)

target_include_directories(schema_evolution_benchmark PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(schema_evolution_benchmark PRIVATE
  CURL::libcurl
)

target_compile_options(encode_benchmark PRIVATE -O3 -march=native -DNDEBUG)
target_compile_options(schema_registry_benchmark PRIVATE -O3 -march=native -DNDEBUG)
target_compile_options(kafka_producer_benchmark PRIVATE -O3 -march=native -DNDEBUG)
target_compile_options(representation_benchmark PRIVATE -O3 -march=native -DNDEBUG)
target_compile_options(latency_benchmark PRIVATE -O3 -march=native -DNDEBUG)
target_compile_options(distribution_benchmark PRIVATE -O3 -march=native -DNDEBUG)
target_compile_options(concurrency_benchmark PRIVATE -O3 -march=native -DNDEBUG)
target_compile_options(schema_evolution_benchmark PRIVATE -O3 -march=native -DNDEBUG)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(encode_benchmark PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(schema_registry_benchmark PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(kafka_producer_benchmark PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(representation_benchmark PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(latency_benchmark PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(distribution_benchmark PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(concurrency_benchmark PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(schema_evolution_benchmark PRIVATE -Wall -Wextra -Wpedantic)
endif()

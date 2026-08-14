# Winning path: Buf-generated Protobuf key/value producer for Kafka

This is a handoff for the Claude Code agent who will implement the production-shaped producer at work.

Target contract:

- Kafka topic: test-topic.
- Kafka key: a Buf-generated C++ Protobuf message from test-keys.proto.
- Kafka value: a Buf-generated C++ Protobuf message from test-values.proto.
- Schema Registry: Confluent Protobuf schemas with separate key and value subjects and IDs.
- Hot path: populate messages, serialize key and value, add cached Confluent framing, and enqueue both bytes with librdkafka.
- No decode benchmark.
- No Schema Registry HTTP request per Kafka message.
- All build, service, and validation commands run in Docker.

The safe baseline is caller-owned reusable buffers plus RD_KAFKA_MSG_F_COPY. Once it is correct, add an ownership-transfer mode and measure it. Do not replace the safe baseline with an unmeasured zero-copy design.

## 1. What the agent must deliver

Create this layout, adapting only existing equivalent target names:

    proto/
      winning/
        test-keys.proto
        test-values.proto
    buf.gen.winning.yaml
    src/
      winning_producer.cc
      schema_registry_client.cc
    include/
      winning/
        confluent_protobuf.h
        schema_registry_client.h

The application must:

1. Read both .proto files at startup.
2. Resolve or register the key schema under test-topic-key.
3. Resolve or register the value schema under test-topic-value.
4. Cache both returned schema IDs in process memory.
5. Build generated Protobuf messages.
6. Serialize directly into reusable buffers after a six-byte prefix.
7. Produce the framed key and framed value to test-topic using librdkafka.
8. Poll librdkafka, retry QUEUE_FULL, flush before exit, and fail on delivery errors.
9. Expose broker, Registry URL, credentials, topic, Kafka settings, iterations, and repetitions.
10. Provide benchmark mode that refuses fewer than 1,000,000 messages per repetition or fewer than 10 repetitions.

Schema Registry calls belong to startup, deployment, rollout, or an explicitly bounded recovery path. They must not be inside encode, produce, or the per-message loop.

## 2. Exact proto files

Use self-contained schemas initially. Imported Protobuf files require Schema Registry references and should be added only after this path works.

### proto/winning/test-keys.proto

~~~protobuf
syntax = "proto3";

package winning.keys.v1;

// Keep key fields stable. Changing encoded key bytes can change Kafka partitioning.
option optimize_for = SPEED;

message TestKey {
  string venue = 1;
  string symbol = 2;
  uint64 instrument_id = 3;
}
~~~

### proto/winning/test-values.proto

~~~protobuf
syntax = "proto3";

package winning.values.v1;

option optimize_for = SPEED;

message TestValue {
  string symbol = 1;
  uint64 instrument_id = 2;
  int64 sequence = 3;
  int64 event_time_unix_nanos = 4;

  // 123456789 means 1.23456789 at fixed scale 8.
  sint64 price_scaled_8 = 5;
  sint64 quantity = 6;
  string venue = 7;
  string source = 8;
}
~~~

Rules:

- Never use double for an exact financial decimal.
- Use sint64 when signed values are usually small; ZigZag keeps small negative values compact.
- Use fixed64 when fixed-width unsigned values are preferable; measure this rather than assuming it wins.
- Use a string only when arbitrary decimal scale or exact decimal text is required.
- Never add a random nonce, changing wall-clock value, or other unstable field to TestKey.
- Never reuse a Protobuf field number for a different meaning. Reserve removed numbers.
- If the real production message already exists, preserve its field numbers and semantics. This example is a test shape, not permission to renumber a live contract.

## 3. Exact Buf generator file

The conventional Buf names are buf.gen.yaml or a named template such as buf.gen.winning.yaml. Use buf.gen.winning.yaml; buf.generate.yaml is not the normal Buf template name.

Create buf.gen.winning.yaml at the repository root:

~~~yaml
version: v2
managed:
  enabled: true
  override:
    - file_option: optimize_for
      value: SPEED
plugins:
  - protoc_builtin: cpp
    out: generated/winning
    opt:
      - paths=source_relative
~~~

Generate in the container:

~~~bash
buf lint
buf generate proto --template buf.gen.winning.yaml
~~~

Expected generated paths:

~~~text
generated/winning/winning/test-keys.pb.h
generated/winning/winning/test-keys.pb.cc
generated/winning/winning/test-values.pb.h
generated/winning/winning/test-values.pb.cc
~~~

paths=source_relative makes generated paths follow the source path. It does not change the wire format. The important generated-code performance choice is option optimize_for = SPEED in each schema. SPEED is also the default, but declare it explicitly so it cannot silently change.

The managed-mode override is the enforcement mechanism: Buf rewrites the file option in the generation input for this template, so a developer cannot accidentally generate this target as CODE_SIZE or LITE_RUNTIME. Keep option optimize_for = SPEED in the checked-in proto files as well. That makes the intended contract visible to reviewers and keeps direct protoc or other tooling aligned; Buf still enforces SPEED when this template runs.

If the repository has multiple Buf generation templates that must all enforce the same policy, add the same managed block to each template or consolidate generation behind one canonical template. Managed mode is configured per generation template; adding it only to buf.gen.winning.yaml does not change buf.gen.google_speed.yaml or unrelated templates.

Do not add LITE_RUNTIME to this target unless binary size is a measured requirement. The default recommendation is full Google Protobuf SPEED.

## 4. Build settings

Build in Release mode:

~~~bash
cmake -S . -B build-winning -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_STANDARD=23
cmake --build build-winning --parallel --target winning_producer
~~~

The target should use:

~~~text
-O3 -DNDEBUG -Wall -Wextra -Wpedantic
~~~

Use -march=native only when the binary is built and run on the same homogeneous CPU class. For a fleet, use the approved deployment CPU target and benchmark that exact target. Consider LTO only after measurement.

The hot API is:

~~~cpp
const std::size_t bytes = message.ByteSizeLong();
message.SerializeToArray(destination, static_cast<int>(bytes));
~~~

Check every return value; reject payloads larger than INT_MAX before the cast; size after populating fields; reuse buffers; and give each producer thread its own message and buffers. Arena is not automatically faster and must be benchmarked separately.

SerializeToString is a valid comparison but not the default because it can add string growth/allocation and another copy when the Confluent prefix is attached.

## 5. Confluent Protobuf framing

For one top-level Protobuf message:

~~~text
byte 0       : magic byte 0x00
bytes 1..4   : schema ID, unsigned 32-bit big-endian
byte 5       : message-index encoding for top-level message index 0x00
bytes 6..end : normal Protobuf wire payload
~~~

Both key and value need a six-byte prefix but normally have different subjects and IDs:

~~~text
test-topic-key   -> winning.keys.v1.TestKey    -> key_schema_id
test-topic-value -> winning.values.v1.TestValue -> value_schema_id
~~~

The prefix is local work, not a Registry request. Never hard-code schema ID 1.

Create include/winning/confluent_protobuf.h:

~~~cpp
#pragma once

#include <google/protobuf/message_lite.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace winning {

inline void write_big_endian_u32(std::uint8_t* destination, std::uint32_t value) {
  destination[0] = static_cast<std::uint8_t>(value >> 24);
  destination[1] = static_cast<std::uint8_t>(value >> 16);
  destination[2] = static_cast<std::uint8_t>(value >> 8);
  destination[3] = static_cast<std::uint8_t>(value);
}

inline std::size_t serialize_confluent_protobuf(
    const google::protobuf::MessageLite& message,
    std::uint32_t schema_id,
    std::vector<std::uint8_t>& output) {
  const std::size_t payload_size = message.ByteSizeLong();
  if (payload_size > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
    throw std::length_error("Protobuf payload is larger than SerializeToArray's int limit");
  }

  constexpr std::size_t prefix_size = 6;
  output.resize(prefix_size + payload_size);
  output[0] = 0;
  write_big_endian_u32(output.data() + 1, schema_id);
  output[5] = 0;  // one-byte message-index array [0]

  if (!message.SerializeToArray(output.data() + prefix_size,
                                static_cast<int>(payload_size))) {
    throw std::runtime_error("Protobuf serialization failed");
  }
  return output.size();
}

}  // namespace winning
~~~

This avoids serializing into one buffer and then copying the payload into a framed buffer. Reserve a known upper bound if available.

## 6. Schema Registry client

Use libcurl for Registry HTTP and nlohmann_json (or the work-standard JSON library) for the control plane. Do not shell out to curl.

The client should expose an API equivalent to:

~~~cpp
struct RegisteredSchema {
  std::string subject;
  std::uint32_t id;
  std::string fingerprint;
};

class SchemaRegistryClient {
 public:
  SchemaRegistryClient(std::string base_url,
                       std::string username,
                       std::string password);

  RegisteredSchema register_or_lookup(const std::string& subject,
                                      const std::string& proto_source);
};
~~~

For each subject:

1. Read the exact .proto source used for the build.
2. Create a JSON body with a JSON library:
   
       {"schemaType":"PROTOBUF","schema":"syntax = \"proto3\";\npackage winning.keys.v1;\n..."}

3. Resolve with POST /subjects/{subject} using that body.
4. If exact-schema lookup returns 404, register with POST /subjects/{subject}/versions.
5. Parse {"id": 123}.
6. Validate the ID is positive and fits uint32_t.
7. Cache the ID in immutable/read-mostly producer state.
8. Fail startup if resolution or registration fails. Never send unframed data as a fallback.

Use these headers:

~~~text
Content-Type: application/vnd.schemaregistry.v1+json
Accept: application/vnd.schemaregistry.v1+json
~~~

The example schemas have no imports, so no references array is needed. If production schemas import another .proto, register dependencies first and include Schema Registry references with dependency name, subject, and version. Test this against the exact Registry version used at work.

Use separate compatibility policy for key and value. BACKWARD or FULL may be appropriate, but that is a platform decision. Compatibility does not guarantee that a key change preserves desired Kafka partitioning semantics.

The client must support HTTP/HTTPS, connection and total timeouts, TLS verification by default, authentication, bounded startup retries, an explicit invalid-ID recovery path, and metrics. Never log credentials. For HTTPS, the default must verify certificates: CURLOPT_SSL_VERIFYPEER=1 and CURLOPT_SSL_VERIFYHOST=2.

The existing repository Schema Registry benchmark already uses libcurl, the PROTOBUF schema type, timeouts, TLS options, and response validation. Reuse those patterns.

## 7. Producer core

Create src/winning_producer.cc. This is the core shape the agent should make fully compilable, with RAII wrappers added around librdkafka handles:

~~~cpp
#include "generated/winning/winning/test-keys.pb.h"
#include "generated/winning/winning/test-values.pb.h"
#include "winning/confluent_protobuf.h"
#include "winning/schema_registry_client.h"

#include <librdkafka/rdkafka.h>

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::atomic<std::uint64_t> delivery_errors{0};

void delivery_callback(rd_kafka_t*, const rd_kafka_message_t* message, void*) {
  if (message->err != RD_KAFKA_RESP_ERR_NO_ERROR) {
    delivery_errors.fetch_add(1, std::memory_order_relaxed);
    std::cerr << "Kafka delivery error: "
              << rd_kafka_message_errstr(message) << '\n';
  }
}

std::string env_or(const char* name, std::string fallback) {
  if (const char* value = std::getenv(name); value != nullptr && *value != '\0') {
    return value;
  }
  return fallback;
}

std::uint64_t env_u64(const char* name, std::uint64_t fallback) {
  if (const char* value = std::getenv(name); value != nullptr && *value != '\0') {
    return std::stoull(value);
  }
  return fallback;
}

winning::keys::v1::TestKey make_key() {
  winning::keys::v1::TestKey key;
  key.set_venue("XNAS");
  key.set_symbol("BTC-USD");
  key.set_instrument_id(42);
  return key;
}

winning::values::v1::TestValue make_value(std::uint64_t sequence) {
  winning::values::v1::TestValue value;
  value.set_symbol("BTC-USD");
  value.set_instrument_id(42);
  value.set_sequence(static_cast<std::int64_t>(sequence));
  value.set_event_time_unix_nanos(1'750'000'000'000'000'000LL);
  value.set_price_scaled_8(123'456'789);
  value.set_quantity(100);
  value.set_venue("XNAS");
  value.set_source("feed-a");
  return value;
}

void set_conf(rd_kafka_conf_t* conf, const char* key, const std::string& value) {
  char error[512]{};
  if (rd_kafka_conf_set(conf, key, value.c_str(), error, sizeof(error))
      != RD_KAFKA_CONF_OK) {
    throw std::runtime_error(std::string("Kafka config ") + key + ": " + error);
  }
}

rd_kafka_t* make_producer(const std::string& brokers) {
  rd_kafka_conf_t* conf = rd_kafka_conf_new();
  set_conf(conf, "bootstrap.servers", brokers);
  set_conf(conf, "enable.idempotence", "true");
  set_conf(conf, "acks", "all");
  set_conf(conf, "retries", "2147483647");
  set_conf(conf, "max.in.flight.requests.per.connection", "5");
  set_conf(conf, "compression.type", "none");
  set_conf(conf, "linger.ms", "0");
  set_conf(conf, "batch.num.messages", "1");
  set_conf(conf, "message.timeout.ms", "30000");
  rd_kafka_conf_set_dr_msg_cb(conf, delivery_callback);

  char error[512]{};
  rd_kafka_t* producer = rd_kafka_new(
      RD_KAFKA_PRODUCER, conf, error, sizeof(error));
  if (producer == nullptr) {
    throw std::runtime_error(std::string("Kafka producer: ") + error);
  }
  return producer;
}

bool produce_copy(rd_kafka_topic_t* topic,
                  const std::vector<std::uint8_t>& key,
                  const std::vector<std::uint8_t>& value) {
  // COPY makes librdkafka copy key and value before this returns.
  return rd_kafka_produce(
      topic,
      RD_KAFKA_PARTITION_UA,
      RD_KAFKA_MSG_F_COPY,
      const_cast<std::uint8_t*>(value.data()), value.size(),
      key.data(), key.size(), nullptr) == 0;
}

void produce_with_retry(rd_kafka_t* producer,
                        rd_kafka_topic_t* topic,
                        const std::vector<std::uint8_t>& key,
                        const std::vector<std::uint8_t>& value) {
  for (;;) {
    if (produce_copy(topic, key, value)) return;

    const auto error = rd_kafka_last_error();
    if (error != RD_KAFKA_RESP_ERR__QUEUE_FULL) {
      throw std::runtime_error(
          std::string("rd_kafka_produce: ") + rd_kafka_err2str(error));
    }
    rd_kafka_poll(producer, 1);
  }
}

}  // namespace

int main() {
  try {
    const std::string brokers = env_or("KAFKA_BROKERS", "kafka:29092");
    const std::string registry_url =
        env_or("SCHEMA_REGISTRY_URL", "http://schema-registry:8081");
    const std::string topic = env_or("KAFKA_TOPIC", "test-topic");
    const std::uint64_t iterations = env_u64("ITERATIONS", 1'000'000);
    const std::uint64_t repetitions = env_u64("REPETITIONS", 10);

    if (iterations < 1'000'000 || repetitions < 10) {
      throw std::runtime_error(
          "benchmark requires ITERATIONS >= 1000000 and REPETITIONS >= 10");
    }

    winning::SchemaRegistryClient registry(
        registry_url,
        std::getenv("SCHEMA_REGISTRY_USERNAME")
            ? std::getenv("SCHEMA_REGISTRY_USERNAME") : "",
        std::getenv("SCHEMA_REGISTRY_PASSWORD")
            ? std::getenv("SCHEMA_REGISTRY_PASSWORD") : "");

    const auto key_schema = registry.register_or_lookup(
        topic + "-key",
        winning::read_file("proto/winning/test-keys.proto"));
    const auto value_schema = registry.register_or_lookup(
        topic + "-value",
        winning::read_file("proto/winning/test-values.proto"));

    std::cerr << "key subject=" << key_schema.subject
              << " id=" << key_schema.id
              << "; value subject=" << value_schema.subject
              << " id=" << value_schema.id << '\n';

    rd_kafka_t* producer = make_producer(brokers);
    rd_kafka_topic_t* kafka_topic =
        rd_kafka_topic_new(producer, topic.c_str(), nullptr);
    if (kafka_topic == nullptr) {
      throw std::runtime_error("cannot create Kafka topic handle");
    }

    const auto key_message = make_key();
    std::vector<std::uint8_t> key_bytes;
    std::vector<std::uint8_t> value_bytes;
    key_bytes.reserve(256);
    value_bytes.reserve(1024);

    for (std::uint64_t repetition = 1; repetition <= repetitions; ++repetition) {
      delivery_errors.store(0, std::memory_order_relaxed);

      for (std::uint64_t i = 0; i < iterations; ++i) {
        // For maximum throughput, construct once and mutate only changing
        // fields if that matches the real application's ownership model.
        const auto value_message = make_value(i);

        winning::serialize_confluent_protobuf(
            key_message, key_schema.id, key_bytes);
        winning::serialize_confluent_protobuf(
            value_message, value_schema.id, value_bytes);

        produce_with_retry(producer, kafka_topic, key_bytes, value_bytes);

        if ((i & 0x3fff) == 0) {
          rd_kafka_poll(producer, 0);
        }
      }

      if (rd_kafka_flush(producer, 120'000)
          != RD_KAFKA_RESP_ERR_NO_ERROR) {
        throw std::runtime_error("Kafka flush failed");
      }

      const auto errors = delivery_errors.load(std::memory_order_relaxed);
      if (errors != 0) {
        throw std::runtime_error(
            "Kafka delivery errors: " + std::to_string(errors));
      }
      std::cerr << "completed repetition " << repetition << '\n';
    }

    rd_kafka_topic_destroy(kafka_topic);
    rd_kafka_destroy(producer);
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "ERROR: " << error.what() << '\n';
    return 1;
  }
}
~~~

Required corrections:

- Put read_file in a source file or inline utility; do not define a normal non-inline function in a multiply included header.
- Use RAII wrappers for librdkafka handles.
- Do not construct a new message and allocate all strings per record unless that is real application behavior. Benchmark immutable-message reuse and realistic mutation separately.
- Record enqueue time separately from flush/delivery time.
- For correctness, use RD_KAFKA_MSG_F_COPY first. An ownership-transfer path must allocate memory under librdkafka's ownership contract, transfer only after successful serialization, and free on every produce failure.
- Optimize key and value together. Optimizing only the value can hide the cost of copying the key.

## 8. CMake target

Add a separate target:

~~~cmake
add_executable(winning_producer
  src/winning_producer.cc
  src/schema_registry_client.cc
  generated/winning/winning/test-keys.pb.cc
  generated/winning/winning/test-values.pb.cc
)

target_include_directories(winning_producer PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/generated/winning
  ${RDKAFKA_INCLUDE_DIR}
)

target_link_libraries(winning_producer PRIVATE
  protobuf::libprotobuf
  ${RDKAFKA_LIBRARY}
  CURL::libcurl
  nlohmann_json::nlohmann_json
)

target_compile_options(winning_producer PRIVATE
  -O3 -DNDEBUG -Wall -Wextra -Wpedantic
)
~~~

The current repository already discovers Protobuf, nlohmann/json, CURL, and librdkafka. If the work repository does not, add those dependencies to CMake and Docker. Do not link a second Kafka client library.

If generated paths differ, use paths emitted by Buf. Never hand-edit generated files.

## 9. Docker-only build and run

Build the image and start the existing Confluent stack:

~~~bash
docker build --progress=plain -t winning-producer:local .
docker compose -f docker-compose.schema-registry.yml up -d
~~~

From the producer container use service names, not localhost:

~~~text
KAFKA_BROKERS=kafka:29092
SCHEMA_REGISTRY_URL=http://schema-registry:8081
KAFKA_TOPIC=test-topic
~~~

Inside the build container:

~~~bash
buf lint
buf generate proto --template buf.gen.winning.yaml
cmake -S . -B build-winning -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-winning --parallel --target winning_producer
build-winning/winning_producer
~~~

The host-published Registry port in this repository is 18081; from another container use schema-registry:8081. Discover the exact Docker network with:

~~~bash
docker network ls
docker compose -f docker-compose.schema-registry.yml ps
~~~

For a smoke test, use ITERATIONS=1 REPETITIONS=1, but do not call that a benchmark. The benchmark gate is 1,000,000 x 10.

## 10. Kafka profiles

Implement named profiles so durability trade-offs are visible.

Latency-oriented:

~~~text
compression.type=none
linger.ms=0
batch.num.messages=1
acks=1
enable.idempotence=false
~~~

Durable/idempotent:

~~~text
compression.type=none
linger.ms=0
batch.num.messages=1
acks=all
enable.idempotence=true
retries=2147483647
max.in.flight.requests.per.connection=5
~~~

Throughput/batching:

~~~text
compression.type=zstd
linger.ms=1
batch.num.messages=100
acks=all
enable.idempotence=true
~~~

Benchmark profiles separately. A faster profile is not a production win if its delivery semantics are unacceptable.

Record broker/client versions, CPU model and architecture, compiler and flags, raw and framed bytes, both schema IDs, acks, idempotence, compression, linger, batch size, retries, queue-full retries, delivery errors, enqueue time, and flush time.

## 11. Validation checklist

Schema and generated code:

~~~bash
buf lint
buf generate proto --template buf.gen.winning.yaml
git diff --check
~~~

Check generated namespaces:

~~~cpp
winning::keys::v1::TestKey
winning::values::v1::TestValue
~~~

Registry subjects from the host:

~~~bash
curl --fail http://localhost:18081/subjects/test-topic-key/versions
curl --fail http://localhost:18081/subjects/test-topic-value/versions
curl --fail http://localhost:18081/subjects/test-topic-key/versions/latest
curl --fail http://localhost:18081/subjects/test-topic-value/versions/latest
~~~

The application must log both subjects and IDs, never credentials. Different key/value IDs are normal.

Byte-level framing for both key and value:

~~~text
bytes[0] == 0x00
bytes[1..4] == that subject's schema ID in big-endian order
bytes[5] == 0x00
bytes[6..] == normal SerializeToArray output
~~~

The key must use key_schema_id and the value must use value_schema_id. Using the value ID for the key is a serious bug.

Kafka checks:

- Topic is exactly test-topic.
- Key bytes are non-null Kafka key data.
- Value bytes are non-null Kafka value data.
- Stable identical key bytes produce stable partition selection.
- QUEUE_FULL is retried with polling and a bounded policy.
- rd_kafka_flush runs before destruction.
- Any non-zero delivery error count exits non-zero.
- No consumer or decoding code is included in producer timing.

A consumer may be used as an external smoke-test tool, but it is not part of producer latency measurement.

## 12. Benchmark matrix

Keep these as separate rows:

1. Pure Google Protobuf key/value encoding, no framing.
2. Cached six-byte framing with direct in-place SerializeToArray.
3. Cached framing with serialize-then-copy control path.
4. Key/value encoding plus librdkafka enqueue with RD_KAFKA_MSG_F_COPY.
5. Key/value encoding plus ownership transfer, only after correctness.
6. Full Kafka path with acks=1.
7. Full Kafka path with acks=all and idempotence.
8. Full Kafka path with batching/compression.
9. Registry lookup with an existing schema.
10. Cold registration under a new subject.
11. Registry unavailable during startup/recovery.

Rows 1–8 must run at least 1,000,000 messages per repetition and at least 10 repetitions. Live HTTP rows should run at least 10 independent requests; they are control-plane measurements, not hot-path serializer measurements.

Report median, min, max, standard deviation, bytes, operations/second, delivery errors, and queue-full retries. Add p99/p99.9 in individually timed latency runs. Record CPU pinning and architecture. Run ARM64 through OrbStack and AMD64 natively in GitHub Actions when comparing platforms; do not use emulated AMD64 values as production latency budgets.

## 13. Verdict to preserve

The default production path should be:

~~~text
Buf source of truth
  -> Google C++ Protobuf with optimize_for = SPEED
  -> caller-owned reusable buffers
  -> direct SerializeToArray after a six-byte cached Confluent prefix
  -> librdkafka with explicit binary key and value
  -> Schema Registry IDs resolved once before the hot loop
~~~

Schema Registry is acceptable in this design. The steady-state cost is a small local framing header, usually low-single-digit percentage over pure Protobuf in this repository's direct comparison. The unacceptable design is a Registry HTTP lookup or registration for every message: that adds millisecond-scale control-plane latency, creates a network dependency in the trading path, and can turn a Registry incident into a producer-path incident.

Start with Google Protobuf SPEED and the safe copy path. Test protobuf-c, ownership transfer, batching, and compression only as targeted alternatives. Choose another implementation only when it wins on the real message shape, compiler, CPU, allocator behavior, Kafka settings, and required delivery semantics.

## 14. Definition of done

- [ ] Both exact proto files exist and pass buf lint.
- [ ] buf.gen.winning.yaml generates both C++ types in Docker.
- [ ] Generated code builds as a Release target.
- [ ] Key and value are both Confluent-framed Protobuf.
- [ ] Key and value use separate subjects and the correct separate IDs.
- [ ] Registry calls are outside the hot loop.
- [ ] test-topic receives binary Protobuf keys and values.
- [ ] Delivery errors are surfaced and fail the process.
- [ ] Safe RD_KAFKA_MSG_F_COPY works before ownership transfer.
- [ ] Docker/OrbStack commands are reproducible.
- [ ] Benchmark mode enforces 1,000,000 x 10.
- [ ] Reports separate pure encoding, framing, enqueue, Kafka delivery, and Registry control-plane timing.
- [ ] A smoke test checks the first bytes and IDs of both key and value.
- [ ] The implementation is committed and pushed only after checks pass.


## 15. GitLab CI/CD artifact publishing

Use GitLab as the distribution point:

~~~text
GitLab Container Registry
  -> contracts-toolchain:<pinned Buf/protobuf/plugin versions>

GitLab Generic Package Registry
  -> trading-contracts-cpp-protobuf:<version>
  -> trading-contracts-cpp-grpc:<version>

GitLab npm Registry
  -> @company/trading-contracts-es

GitLab PyPI Registry
  -> company-trading-contracts-protobuf
  -> company-trading-contracts-connect
~~~

The Container Registry is for OCI images, not ordinary generated headers and .pb.cc files. Store C++ libraries/tarballs in Generic Packages. GitLab's Generic Package Registry is designed for arbitrary versioned files and supports CI job tokens and deploy tokens:
https://docs.gitlab.com/user/packages/generic_packages/

### 15.1 One release tag drives every language

Use one immutable Git tag:

~~~text
trading-contracts-v1.4.0
~~~

That tag must generate and publish all artifacts from the same .proto sources:

~~~text
ES/TypeScript package       1.4.0
Python Protobuf package     1.4.0
Python Connect package      1.4.0
C++ Protobuf package        1.4.0
C++ gRPC package            1.4.0
~~~

Never publish production artifacts from an untagged branch. Pre-release artifacts may use a pipeline ID or commit SHA, but production dependencies use an immutable tag.

### 15.2 Generation differences

| Artifact | Buf generation | Distribution |
|---|---|---|
| ES/TypeScript Connect client | bufbuild/es | npm package |
| Python Protobuf + Connect | protocolbuffers/python plus connectrpc/py | PyPI packages |
| C++ Protobuf | built-in cpp | CMake library with .pb.h/.pb.cc |
| C++ Protobuf + official gRPC | built-in cpp plus the official gRPC C++ plugin | CMake library with .pb.* and .grpc.pb.* |

Current Connect-ES uses a unified bufbuild/es plugin that generates message types and service definitions in one pass. Do not blindly add an old separate connect-es plugin to a Connect-ES 2 setup.

Python should publish the Protobuf message package plus the Connect package. Do not generate or publish standard Python gRPC clients for this company unless a legacy consumer explicitly requires them. Retain the exact Connect plugin name already working in the company's repository and pin its version.

C++ pure Protobuf does not need gRPC. Keep trading-contracts-cpp-protobuf separate from trading-contracts-cpp-grpc so an HFT producer does not pull gRPC into its dependency graph. If C++ RPC clients are required, use the official gRPC C++ plugin and runtime; do not invent a custom C++ Connect implementation.

### 15.3 Buf templates

ES/Connect-ES:

~~~yaml
version: v2
plugins:
  - remote: buf.build/bufbuild/es:PINNED_VERSION
    out: gen/es
~~~

Python with Connect:

~~~yaml
version: v2
plugins:
  - remote: buf.build/protocolbuffers/python:PINNED_VERSION
    out: gen/python
  - remote: buf.build/protocolbuffers/pyi:PINNED_VERSION
    out: gen/python
  - remote: buf.build/connectrpc/py:PINNED_VERSION
    out: gen/python
    opt:
      - protobuf=google
~~~

Pure C++ Protobuf:

~~~yaml
version: v2
managed:
  enabled: true
  override:
    - file_option: optimize_for
      value: SPEED
plugins:
  - protoc_builtin: cpp
    out: gen/cpp
    opt:
      - paths=source_relative
~~~

C++ Protobuf plus official gRPC C++:

~~~yaml
version: v2
managed:
  enabled: true
  override:
    - file_option: optimize_for
      value: SPEED
plugins:
  - protoc_builtin: cpp
    out: gen/cpp
    opt:
      - paths=source_relative
  - remote: buf.build/grpc/cpp:PINNED_VERSION
    out: gen/cpp
    opt:
      - paths=source_relative
~~~

If hosted plugin execution is not allowed, install the official gRPC C++ plugin inside the pinned Docker toolchain and replace the second entry with:

~~~yaml
  - local: protoc-gen-grpc
    out: gen/cpp
    opt:
      - paths=source_relative
~~~

Buf's official remote-plugin guide also documents standard Python gRPC generation, but that is intentionally excluded from this company's default package set. Pin every plugin version that is actually used in release CI.

### 15.4 C++ package contents

Pure Protobuf package:

~~~text
trading-contracts-cpp-protobuf-1.4.0.tar.gz
  include/company/trading/*.pb.h
  src/company/trading/*.pb.cc
  lib/cmake/company_trading_contracts/
  CMakeLists.txt
  proto/*.proto
  schema/file_descriptor_set.pb
  SHA256SUMS
~~~

The gRPC package additionally contains:

~~~text
  include/company/trading/*.grpc.pb.h
  src/company/trading/*.grpc.pb.cc
~~~

Expose stable CMake targets:

~~~text
company::trading_contracts_protobuf
company::trading_contracts_grpc
~~~

The Protobuf package links to the compatible Protobuf runtime. The gRPC package links to Protobuf plus the approved gRPC C++ runtime. Include the original .proto files or a descriptor-set artifact because deployment may need to register schemas with Confluent Schema Registry.

### 15.5 Pinned Buf toolchain image

Create a dedicated Dockerfile for generation and pin:

~~~text
BUF_VERSION
PROTOBUF_VERSION
GRPC_VERSION
ES plugin version
Python plugin versions
Connect plugin versions
C++ compiler version
~~~

Build and push the toolchain image:

~~~bash
docker login \
  --username "$CI_REGISTRY_USER" \
  --password "$CI_REGISTRY_PASSWORD" \
  "$CI_REGISTRY"

docker build \
  --file Dockerfile.contracts-toolchain \
  --tag "$CI_REGISTRY_IMAGE/contracts-toolchain:$CI_COMMIT_TAG" \
  .

docker push "$CI_REGISTRY_IMAGE/contracts-toolchain:$CI_COMMIT_TAG"
~~~

Do not use latest for release generation. The image tag or digest must identify the exact toolchain used to create the packages.

If remote plugins are not acceptable, install pinned plugin binaries inside this image and use local entries in the templates. Buf remains the orchestrator; plugins do not have to be downloaded by developers.

### 15.6 GitLab CI skeleton

Replace the image name, package names, current npm/PyPI commands, and C++ target names with the work repository's values.

~~~yaml
stages:
  - validate
  - generate
  - package
  - publish

variables:
  CONTRACT_VERSION: "$CI_COMMIT_TAG"
  TOOLCHAIN_IMAGE: "$CI_REGISTRY_IMAGE/contracts-toolchain:1.4.0"

workflow:
  rules:
    - if: '$CI_COMMIT_TAG =~ /^trading-contracts-v[0-9]+\.[0-9]+\.[0-9]+$/'
    - when: never

validate:
  stage: validate
  image: "$TOOLCHAIN_IMAGE"
  script:
    - buf lint
    - buf build
    - buf breaking --against '.git#branch=main'

generate:
  stage: generate
  image: "$TOOLCHAIN_IMAGE"
  script:
    - rm -rf gen
    - buf generate --template buf.gen.es.yaml
    - buf generate --template buf.gen.python.connect.yaml
    - buf generate --template buf.gen.cpp.yaml
    - buf generate --template buf.gen.cpp.grpc.yaml
  artifacts:
    expire_in: 7 days
    paths:
      - gen/
      - proto/
      - buf.yaml
      - buf.lock
      - buf.gen.*.yaml

package_cpp:
  stage: package
  image: "$TOOLCHAIN_IMAGE"
  needs:
    - job: generate
      artifacts: true
  script:
    - cmake -S . -B build-cpp -G Ninja -DCMAKE_BUILD_TYPE=Release
    - cmake --build build-cpp --parallel --target trading_contracts_protobuf
    - cmake --install build-cpp --prefix dist/cpp-protobuf
    - cmake -S . -B build-cpp-grpc -G Ninja -DCMAKE_BUILD_TYPE=Release
    - cmake --build build-cpp-grpc --parallel --target trading_contracts_grpc
    - cmake --install build-cpp-grpc --prefix dist/cpp-grpc
    - tar -czf "trading-contracts-cpp-protobuf-$CONTRACT_VERSION.tar.gz" -C dist/cpp-protobuf .
    - tar -czf "trading-contracts-cpp-grpc-$CONTRACT_VERSION.tar.gz" -C dist/cpp-grpc .
    - sha256sum trading-contracts-cpp-*.tar.gz > SHA256SUMS
  artifacts:
    expire_in: 7 days
    paths:
      - trading-contracts-cpp-*.tar.gz
      - SHA256SUMS

package_es:
  stage: package
  image: "$TOOLCHAIN_IMAGE"
  needs:
    - job: generate
      artifacts: true
  script:
    - cd packages/es
    - npm ci
    - npm pack
  artifacts:
    expire_in: 7 days
    paths:
      - packages/es/*.tgz

package_python:
  stage: package
  image: "$TOOLCHAIN_IMAGE"
  needs:
    - job: generate
      artifacts: true
  script:
    - python -m build packages/python-protobuf
    - python -m build packages/python-connect
  artifacts:
    expire_in: 7 days
    paths:
      - packages/*/dist/*.whl
      - packages/*/dist/*.tar.gz

publish_generic_cpp:
  stage: publish
  image: curlimages/curl:latest
  needs:
    - job: package_cpp
      artifacts: true
  script:
    - |
      for file in trading-contracts-cpp-*.tar.gz SHA256SUMS; do
        curl --fail --location \
          --header "JOB-TOKEN: $CI_JOB_TOKEN" \
          --upload-file "$file" \
          "$CI_API_V4_URL/projects/$CI_PROJECT_ID/packages/generic/trading-contracts-cpp/$CONTRACT_VERSION/$file"
      done

publish_npm:
  stage: publish
  image: node:22
  needs:
    - job: package_es
      artifacts: true
  script:
    - npm publish packages/es/*.tgz --registry "$CI_API_V4_URL/projects/$CI_PROJECT_ID/packages/npm/"

publish_pypi:
  stage: publish
  image: python:3.13
  needs:
    - job: package_python
      artifacts: true
  script:
    - python -m pip install --no-cache-dir twine
    - >
      python -m twine upload
      --repository-url "$CI_API_V4_URL/projects/$CI_PROJECT_ID/packages/pypi"
      --username "gitlab-ci-token"
      --password "$CI_JOB_TOKEN"
      packages/*/dist/*
~~~

The C++ job is separate from ES/Python because C++ output is source plus a CMake library, while ES/Python output is a language-native package.

### 15.7 Pulling the C++ package from another GitLab project

Use an authorized job token in downstream CI or a read-only deploy token for developer machines:

~~~bash
VERSION=1.4.0
PACKAGE=trading-contracts-cpp
PROJECT_ID=CONTRACT_PROJECT_ID

curl --fail --location \
  --header "JOB-TOKEN: $CI_JOB_TOKEN" \
  "$CI_API_V4_URL/projects/$PROJECT_ID/packages/generic/$PACKAGE/$VERSION/trading-contracts-cpp-protobuf-$VERSION.tar.gz" \
  --output /tmp/trading-contracts-cpp.tar.gz

curl --fail --location \
  --header "JOB-TOKEN: $CI_JOB_TOKEN" \
  "$CI_API_V4_URL/projects/$PROJECT_ID/packages/generic/$PACKAGE/$VERSION/SHA256SUMS" \
  --output /tmp/SHA256SUMS

cd /tmp
sha256sum -c SHA256SUMS
mkdir -p trading-contracts
tar -xzf trading-contracts-cpp.tar.gz -C trading-contracts
~~~

Then:

~~~cmake
add_subdirectory(/tmp/trading-contracts)

target_link_libraries(my_app
  PRIVATE
    company::trading_contracts_protobuf
)
~~~

Never commit GitLab tokens into CMake, Dockerfiles, CI configuration, or source code.

### 15.8 Schema Registry is a separate deployment step

The package pipeline generates and packages code. Deployment registers the matching schemas:

~~~text
test-topic-key   <- proto from contracts release v1.4.0
test-topic-value <- proto from contracts release v1.4.0
~~~

Deployment should:

1. Register or look up the key schema.
2. Register or look up the value schema.
3. Verify compatibility.
4. Record the resulting IDs for that Registry environment.
5. Start the producer with those subjects/IDs available.
6. Let the producer cache the IDs before its hot loop.

Do not confuse Git/package version 1.4.0 with a Schema Registry numeric ID. The package version can be consistent across languages; the numeric ID is assigned by each Registry environment and must not be hard-coded into the C++ package.

### 15.9 Release checks

~~~bash
buf lint
buf build
buf breaking --against '.git#branch=main'
buf generate --template buf.gen.es.yaml
buf generate --template buf.gen.python.connect.yaml
buf generate --template buf.gen.cpp.yaml
buf generate --template buf.gen.cpp.grpc.yaml
git diff --exit-code
~~~

Then test imports/builds for ES, Python Protobuf, Python Connect, pure C++ Protobuf, and C++ gRPC. Also test checksum verification, clean downstream CMake consumption, and Registry registration from the exact release's schema artifact.

The HFT producer should depend only on trading-contracts-cpp-protobuf unless it genuinely makes RPC calls. The final language policy is: ConnectRPC for Node/Python services, official Google Protobuf C++ for Kafka payloads, and official gRPC C++ for C++ RPC clients. A Connect-compatible server can still be called from C++ through its gRPC-compatible endpoint.


## 16. Pipe-delimited logs and Protobuf Kafka values

The current system writes pipe-delimited text to both logs and Kafka. Change the output contract to:

~~~text
one typed in-memory event
        |
        +--> pipe-delimited human-readable log line
        |
        +--> one Confluent-framed Protobuf serialization for Kafka
~~~

Do not serialize Protobuf and then parse it back to create the log line. Do not call DebugString, TextFormat, or Protobuf JSON for logging. Keep the original typed event as the source for both outputs.

The encoding loop shown below is an encoder-worker loop, never code that runs on the market-data, strategy, or order-management hot thread. The hot thread must only capture an owned event and perform a non-blocking handoff to a bounded queue. Protobuf serialization, pipe formatting, Kafka producer interaction, and log writing belong outside the hot path.

### 16.1 Use one event model

Create or retain an application-level event structure:

~~~cpp
struct MarketEvent {
  std::string venue;
  std::string symbol;
  std::uint64_t instrument_id;
  std::int64_t sequence;
  std::int64_t event_time_unix_nanos;
  std::int64_t price_scaled_8;
  std::int64_t quantity;
  std::string source;
};
~~~

For each event:

1. Populate MarketEvent from the feed or trading logic.
2. Copy its fields into the generated Protobuf key/value messages.
3. Serialize the key and value exactly once into reusable Kafka buffers.
4. Enqueue those buffers to librdkafka.
5. If logging is enabled for this event, format a pipe-delimited line directly from MarketEvent.

Example:

~~~text
XNAS|BTC-USD|42|123456|1750000000000000000|123456789|100|feed-a
~~~

Document the log column order permanently. A log format is also an interface; changing field order casually will break parsers.

### 16.2 Core loop shape

~~~cpp
MarketEvent event = next_market_event();

winning::keys::v1::TestKey key;
key.set_venue(event.venue);
key.set_symbol(event.symbol);
key.set_instrument_id(event.instrument_id);

winning::values::v1::TestValue value;
value.set_symbol(event.symbol);
value.set_instrument_id(event.instrument_id);
value.set_sequence(event.sequence);
value.set_event_time_unix_nanos(event.event_time_unix_nanos);
value.set_price_scaled_8(event.price_scaled_8);
value.set_quantity(event.quantity);
value.set_venue(event.venue);
value.set_source(event.source);

winning::serialize_confluent_protobuf(
    key, key_schema_id, reusable_key_bytes);

winning::serialize_confluent_protobuf(
    value, value_schema_id, reusable_value_bytes);

// Kafka receives the framed binary bytes.
produce_to_kafka(reusable_key_bytes, reusable_value_bytes);

// Logging is a separate projection from the same event.
if (log_policy.should_log(event)) {
  log_queue.try_push(format_pipe_line(event));
}
~~~

This performs one Protobuf serialization for Kafka. The pipe formatter is not a second Protobuf serialization; it is a separate human-readable representation generated from the already available application event.

If the log formatter runs synchronously, it still adds text-formatting cost to the producer loop. Prefer putting a complete log record into a bounded asynchronous queue and letting a logging thread write stdout or a file.

### 16.3 Formatting without expensive stream APIs

Do not use these in the producer loop:

~~~cpp
std::ostringstream
message.DebugString()
google::protobuf::TextFormat
protobuf JSON serialization
~~~

Use a preallocated formatter such as the company's existing logging library, fmt::memory_buffer, or a small formatter based on std::to_chars. The formatter should:

- reserve enough space for the largest line;
- append fields in the documented order;
- convert integers without locale handling;
- preserve exact decimal text when the source value is a string;
- append one newline;
- avoid heap allocation in the steady state where practical.

A simple formatter may look like:

~~~cpp
std::string format_pipe_line(const MarketEvent& event) {
  std::string line;
  line.reserve(256);
  line += event.venue;
  line += '|';
  line += event.symbol;
  line += '|';
  line += std::to_string(event.instrument_id);
  line += '|';
  line += std::to_string(event.sequence);
  line += '|';
  line += std::to_string(event.event_time_unix_nanos);
  line += '|';
  line += std::to_string(event.price_scaled_8);
  line += '|';
  line += std::to_string(event.quantity);
  line += '|';
  line += event.source;
  line += '\n';
  return line;
}
~~~

This example is intentionally clear rather than maximally optimized. Benchmark it against the work logger. For the lowest latency, replace std::to_string with a reusable buffer and std::to_chars, or use the organization's approved asynchronous logger.

If the business requires decimal text such as 1.23456789 in logs, retain the original decimal string in MarketEvent. Do not reconstruct decimal text from a scaled integer unless the formatting rule is explicitly fixed and tested.

### 16.4 Logging must not block Kafka

stdout is not a free output channel. A pipe, terminal, Docker log driver, or collector can block. A blocking write can stall the Kafka producer and invalidate the encoding benchmark.

Use one of these policies:

~~~text
Production:
  log only selected fields
  sample records
  enqueue to an asynchronous logger
  drop logs when the bounded queue is full
  expose a dropped-log counter

Debug/replay:
  allow every event to be logged
  accept that throughput and latency are not production numbers
~~~

Never make Kafka delivery wait indefinitely for stdout. If a log line must be durable, send it through a separate durable logging pipeline; do not turn the Kafka producer's hot path into a synchronous log writer.

The logging queue must own its data. Do not place pointers to reusable Kafka buffers in an asynchronous queue and then reuse those buffers before the logging thread is finished. Queue either:

- the already formatted pipe line; or
- a copied compact LogRecord containing the fields needed by the logger.

The Kafka byte buffers and the log queue therefore have independent lifetimes.

### 16.5 Do not include the pipe text in Kafka

The Kafka value must be only:

~~~text
six-byte Confluent Protobuf prefix
+
generated Protobuf wire payload
~~~

Do not concatenate:

~~~text
pipe text + delimiter + Protobuf bytes
~~~

That would create a custom payload format, break normal Protobuf consumers, and prevent standard Schema Registry tooling from understanding the value.

If downstream humans need readable Kafka messages, use an offline consumer or inspection tool that decodes the Protobuf bytes using the schema ID. The producer should keep Kafka values as pure framed Protobuf.

### 16.6 Benchmark the output modes separately

Add explicit modes:

~~~text
protobuf_kafka_only
protobuf_kafka_plus_sampled_pipe_log
protobuf_kafka_plus_async_pipe_log
protobuf_kafka_plus_sync_pipe_log
pipe_text_kafka_control
~~~

The first three are meaningful production candidates. The last mode represents the old design and is useful as a comparison, not as the new contract.

Measure:

- Protobuf encoding time;
- pipe-formatting time;
- Kafka enqueue time;
- Kafka flush/delivery time;
- logger queue enqueue time;
- logger queue depth;
- dropped log count;
- stdout/file write time;
- end-to-end producer throughput.

The HFT decision should use the Kafka-only and asynchronous/sampled-log rows. A synchronous pipe log row demonstrates the cost of the old design and should not be mistaken for the cost of Protobuf itself.

### 16.7 Final output rule

The implementation is correct when one event produces:

~~~text
stdout/log:
  XNAS|BTC-USD|42|123456|1750000000000000000|123456789|100|feed-a

Kafka key:
  Confluent-framed winning.keys.v1.TestKey bytes

Kafka value:
  Confluent-framed winning.values.v1.TestValue bytes
~~~

The text line and Protobuf payload must come from the same MarketEvent, but they are separate output representations. The Kafka path must never depend on the logger being enabled or healthy.

## 17. Fully off-hot-path encoding pipeline

The production architecture should make the hot thread almost boring:

~~~text
market-data / strategy / order-management thread
                |
                | non-blocking try_push of an owned fixed-size event
                v
        bounded SPSC/MPSC event ring
                |
                v
          encoder worker(s)
                |
                +--> Protobuf key/value serialization
                |       -> six-byte Confluent framing
                |       -> Kafka producer queue
                |
                +--> pipe-line formatting
                        -> asynchronous log queue/writer
~~~

### 17.1 What the hot thread may and may not do

The hot thread may:

- Read the already-available market or order data.
- Copy it into a pre-sized, owned `MarketEvent`.
- Attach a monotonically increasing sequence number and capture timestamp.
- Execute a bounded, non-blocking `try_push` or equivalent ring-buffer operation.
- Update lock-free counters for queue overflow or rejected handoffs.

The hot thread must not:

- Call `SerializeToArray`, `SerializeToString`, `ByteSizeLong`, or any generated Protobuf serializer.
- Call `std::to_string`, `fmt::format`, `ostringstream`, or any other text formatter.
- Allocate from the general heap, take a mutex, wait on a condition variable, or perform filesystem I/O.
- Call `std::cout`, `printf`, a synchronous logger, `rd_kafka_produce`, `rd_kafka_poll`, or Schema Registry HTTP code.

The handoff should look conceptually like this:

~~~cpp
MarketEvent event = capture_event_into_preallocated_storage();
event.sequence = next_sequence.fetch_add(1, std::memory_order_relaxed);
event.captured_ns = steady_clock_now_ns();

if (!event_ring.try_push(std::move(event))) {
  hot_path_metrics.event_ring_overflow.fetch_add(1, std::memory_order_relaxed);
  apply_the_explicit_overflow_policy(event);
}
~~~

For a genuinely latency-sensitive path, prefer one SPSC ring per producer/consumer pair. If there are multiple producers, use a proven bounded MPSC queue or route each producer to its own SPSC queue. Do not build a new lock-free queue casually; benchmark the chosen queue independently.

### 17.2 Event ownership and queue capacity

Every string, decimal, symbol, or binary field must remain valid until the encoder worker has consumed the event. Do not enqueue `string_view`, raw pointers, or references to exchange buffers whose lifetime ends when the callback returns. Use fixed-capacity inline storage, an object pool, or another ownership scheme that does not allocate on the hot thread.

The queue must be sized from measured bursts, not average throughput. Measure at least:

- handoff latency on the hot thread;
- queue depth and high-water mark;
- event age when the worker starts encoding;
- overflow count and duration;
- encoder throughput while Kafka and logging are healthy;
- behavior when Kafka or logging is slow or unavailable.

Queue-full behavior is a business decision and must be explicit. Suitable policies include failover to another capture path, a preallocated emergency queue, overwriting only non-critical snapshots, or rejecting the event while raising an immediate health signal. Never silently drop trading-critical events merely because a benchmark queue filled. A worker is allowed to block or backpressure; the hot thread is not.

### 17.3 Worker responsibilities

The encoder worker owns the expensive work:

1. Pop an owned `MarketEvent` from the ring.
2. Populate the generated Protobuf key and value messages.
3. Serialize directly into reusable per-worker buffers. Reserve enough capacity once and reuse it.
4. Prefix the serialized value with the six-byte Confluent wire header when Schema Registry framing is enabled.
5. Hand the key and value to a Kafka producer worker or producer queue.
6. Format the same event into a pipe-delimited line and hand it to an asynchronous log writer, if logging is enabled.
7. Record separate timing fields for queue wait, Protobuf encoding, pipe formatting, Kafka enqueue, and end-to-end event age.

Keep Kafka producer interaction on the producer side of the handoff. If `rd_kafka_produce` returns `QUEUE_FULL`, polling and retrying belong to that worker, with bounded buffers and an explicit failure policy. They must never turn the market-data thread into a waiting producer.

One encoder worker is the safest starting point for preserving event order. Multiple workers can improve throughput but may reorder records. If ordering matters, route the same Kafka key or instrument to the same worker, use one SPSC queue per ordered stream, and retain the event sequence number so the consumer can detect violations.

### 17.4 Benchmark interpretation

The benchmark must report two different numbers:

- `hot_handoff_ns`: time spent by the producer thread copying/capturing the event and attempting the non-blocking queue handoff;
- `worker_encode_ns`: time spent by the worker doing Protobuf serialization and pipe formatting.

Also report queue wait and end-to-end age. A low `worker_encode_ns` does not make a blocking hot-thread implementation acceptable, and a low handoff latency does not prove the system can sustain bursts. Run separate modes for Protobuf only, pipe formatting only, both on one encoder worker, Kafka enqueue, and the full asynchronous pipeline. Include queue-full and slow-sink tests so the report demonstrates that sink degradation does not block the hot path.

The preferred production mode is therefore: hot thread -> bounded non-blocking event ring -> encoder worker -> Protobuf/Kafka and asynchronous pipe logger. The benchmark's existing serialization loop is useful for measuring worker cost, but it must not be copied into the trading thread.

The repository now includes `./scripts/benchmark_async_pipeline.sh` for this architecture. It compares the synchronous control path with handoff-only, worker-side Protobuf, worker-side pipe formatting, and the combined asynchronous pipeline across all three payload shapes. Its report separates hot-thread handoff percentiles from worker encoding percentiles and records queue drops. The default handoff uses a preallocated immutable event-pool pointer; a production implementation that copies a fixed-size event into the queue should add and run a copy-ring variant before treating the pointer handoff as its final latency budget.

### 17.5 `int64` versus `fixed64`

Run `./scripts/benchmark_fixed64.sh` before changing production schemas. The repository already compares `fixed64` with `sint64` for scaled decimals, but this dedicated phase compares identical positive values encoded as `int64` and `fixed64` across small, scaled-price, and near-maximum ranges. `fixed64` is always eight payload bytes per field; `int64` is a varint and can be smaller for common small values. `fixed64` may reduce CPU for large values, but it is not universally faster, so use the production value distribution and wire-size trade-off rather than assuming fixed-width wins.

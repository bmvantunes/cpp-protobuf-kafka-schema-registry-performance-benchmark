#include "generated/google_speed/google_speed/market.pb.h"

#include <librdkafka/rdkafka.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;
using Nanoseconds = std::chrono::duration<double, std::nano>;
using Message = benchmark::google_speed::OneStringTenInt64;

std::atomic<std::uint64_t> delivery_errors{0};

template <typename T>
inline void do_not_optimize(const T& value) {
#if defined(__GNUC__) || defined(__clang__)
  asm volatile("" : : "m"(value) : "memory");
#else
  (void)value;
#endif
}

Message make_message() {
  Message message;
  message.set_symbol("BTC-USD");
  message.set_value_01(1000001);
  message.set_value_02(1000002);
  message.set_value_03(1000003);
  message.set_value_04(1000004);
  message.set_value_05(1000005);
  message.set_value_06(1000006);
  message.set_value_07(1000007);
  message.set_value_08(1000008);
  message.set_value_09(1000009);
  message.set_value_10(1000010);
  return message;
}

struct Config {
  std::uint64_t iterations = 1'000'000;
  std::uint32_t repetitions = 10;
  std::uint64_t warmup_iterations = 10'000;
  std::string broker = "kafka:29092";
  std::string topic = "protobuf_encoding_kafka_benchmark";
  std::string mode = "copy";
  std::string acks = "0";
  std::string compression = "none";
  std::uint32_t linger_ms = 0;
  std::uint32_t batch_num_messages = 1;
  std::string csv_path = "/work/results/kafka_producer_raw.csv";
  std::string metadata_path = "/work/results/kafka_producer_metadata.txt";
};

struct Csv {
  explicit Csv(const std::string& path) : file(path) {
    if (!file) throw std::runtime_error("cannot open CSV output: " + path);
    file << "kind,library,codegen,api,test_case,repetition,iterations,bytes,enqueue_elapsed_ns,flush_elapsed_ns,end_to_end_ns,ns_per_encode,encodes_per_second,delivery_errors,mode,acks,compression,linger_ms,batch_num_messages\n";
  }
  std::ofstream file;
};

void delivery_callback(rd_kafka_t*, const rd_kafka_message_t* message, void*) {
  if (message->err != RD_KAFKA_RESP_ERR_NO_ERROR) delivery_errors.fetch_add(1, std::memory_order_relaxed);
}

std::string kafka_error(const char* operation, rd_kafka_resp_err_t error) {
  return std::string(operation) + ": " + rd_kafka_err2str(error);
}

std::unique_ptr<rd_kafka_t, decltype(&rd_kafka_destroy)> make_producer(const Config& config) {
  rd_kafka_conf_t* raw_conf = rd_kafka_conf_new();
  char error_buffer[512]{};
  auto set = [&](const char* key, const std::string& value) {
    if (rd_kafka_conf_set(raw_conf, key, value.c_str(), error_buffer, sizeof(error_buffer)) != RD_KAFKA_CONF_OK) {
      throw std::runtime_error(std::string("Kafka config ") + key + ": " + error_buffer);
    }
  };
  set("bootstrap.servers", config.broker);
  set("acks", config.acks);
  set("compression.type", config.compression);
  set("linger.ms", std::to_string(config.linger_ms));
  set("batch.num.messages", std::to_string(config.batch_num_messages));
  rd_kafka_conf_set_dr_msg_cb(raw_conf, delivery_callback);

  rd_kafka_t* producer = rd_kafka_new(RD_KAFKA_PRODUCER, raw_conf, error_buffer, sizeof(error_buffer));
  if (producer == nullptr) throw std::runtime_error(std::string("Kafka producer: ") + error_buffer);
  return {producer, rd_kafka_destroy};
}

std::unique_ptr<rd_kafka_topic_t, decltype(&rd_kafka_topic_destroy)> make_topic(rd_kafka_t* producer, const Config& config) {
  rd_kafka_topic_t* topic = rd_kafka_topic_new(producer, config.topic.c_str(), nullptr);
  if (topic == nullptr) throw std::runtime_error(std::string("Kafka topic: ") + rd_kafka_err2str(rd_kafka_last_error()));
  return {topic, rd_kafka_topic_destroy};
}

bool produce_copy(rd_kafka_topic_t* topic, const std::vector<std::uint8_t>& payload) {
  return rd_kafka_produce(topic, RD_KAFKA_PARTITION_UA, RD_KAFKA_MSG_F_COPY,
                          const_cast<std::uint8_t*>(payload.data()), payload.size(), nullptr, 0, nullptr) == 0;
}

bool produce_owned(rd_kafka_topic_t* topic, const Message& message, std::size_t bytes) {
  auto* payload = static_cast<std::uint8_t*>(std::malloc(bytes));
  if (payload == nullptr) throw std::bad_alloc();
  if (!message.SerializeToArray(payload, static_cast<int>(bytes))) {
    std::free(payload);
    return false;
  }
  if (rd_kafka_produce(topic, RD_KAFKA_PARTITION_UA, RD_KAFKA_MSG_F_FREE,
                       payload, bytes, nullptr, 0, nullptr) == 0) return true;
  std::free(payload);
  return false;
}

bool produce_with_retry(const Config& config, rd_kafka_t* producer, rd_kafka_topic_t* topic,
                        const Message& message, std::vector<std::uint8_t>& reusable, std::size_t bytes) {
  for (;;) {
    const bool serialized = config.mode == "copy"
      ? (message.SerializeToArray(reusable.data(), static_cast<int>(bytes)) && produce_copy(topic, reusable))
      : produce_owned(topic, message, bytes);
    if (serialized) return true;
    if (rd_kafka_last_error() != RD_KAFKA_RESP_ERR__QUEUE_FULL) {
      throw std::runtime_error(kafka_error("rd_kafka_produce", rd_kafka_last_error()));
    }
    rd_kafka_poll(producer, 1);
  }
}

void run(const Config& config) {
  if (config.mode != "copy" && config.mode != "owned") throw std::runtime_error("mode must be copy or owned");
  const Message message = make_message();
  const std::size_t bytes = message.ByteSizeLong();
  auto producer = make_producer(config);
  auto topic = make_topic(producer.get(), config);
  std::vector<std::uint8_t> reusable(bytes);
  Csv csv(config.csv_path);

  for (std::uint64_t i = 0; i < config.warmup_iterations; ++i) {
    message.SerializeToArray(reusable.data(), static_cast<int>(bytes));
    do_not_optimize(reusable[0]);
  }

  for (std::uint32_t repetition = 1; repetition <= config.repetitions; ++repetition) {
    delivery_errors.store(0, std::memory_order_relaxed);
    std::uint64_t checksum = 0;
    const auto start = Clock::now();
    for (std::uint64_t i = 0; i < config.iterations; ++i) {
      if (!produce_with_retry(config, producer.get(), topic.get(), message, reusable, bytes)) {
        throw std::runtime_error("Kafka serialization/produce failed");
      }
      checksum += bytes;
      if ((i & 0x3fff) == 0) rd_kafka_poll(producer.get(), 0);
    }
    const auto enqueue_end = Clock::now();
    const auto enqueue_ns = std::chrono::duration_cast<Nanoseconds>(enqueue_end - start).count();
    if (rd_kafka_flush(producer.get(), 120'000) != RD_KAFKA_RESP_ERR_NO_ERROR) {
      throw std::runtime_error(kafka_error("rd_kafka_flush", rd_kafka_last_error()));
    }
    const auto end = Clock::now();
    const auto flush_ns = std::chrono::duration_cast<Nanoseconds>(end - enqueue_end).count();
    const auto total_ns = std::chrono::duration_cast<Nanoseconds>(end - start).count();
    const auto errors = delivery_errors.load(std::memory_order_relaxed);
    do_not_optimize(checksum);
    csv.file << "kafka,confluent_librdkafka,google_speed,produce," << "one_string_ten_int64" << ','
             << repetition << ',' << config.iterations << ',' << bytes << ','
             << std::fixed << std::setprecision(3) << enqueue_ns << ',' << flush_ns << ',' << total_ns << ','
             << total_ns / static_cast<double>(config.iterations) << ','
             << (1'000'000'000.0 / (total_ns / static_cast<double>(config.iterations))) << ','
             << errors << ',' << config.mode << ',' << config.acks << ',' << config.compression << ','
             << config.linger_ms << ',' << config.batch_num_messages << '\n';
    std::cerr << "repetition " << repetition << ": enqueue " << enqueue_ns / 1'000'000.0
              << " ms, flush " << flush_ns / 1'000'000.0 << " ms, errors " << errors << '\n';
  }
}

Config parse_args(int argc, char** argv) {
  Config config;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    auto next = [&] {
      if (i + 1 >= argc) throw std::runtime_error("missing value for " + arg);
      return std::string(argv[++i]);
    };
    if (arg == "--iterations") config.iterations = std::stoull(next());
    else if (arg == "--repetitions") config.repetitions = static_cast<std::uint32_t>(std::stoul(next()));
    else if (arg == "--warmup-iterations") config.warmup_iterations = std::stoull(next());
    else if (arg == "--broker") config.broker = next();
    else if (arg == "--topic") config.topic = next();
    else if (arg == "--mode") config.mode = next();
    else if (arg == "--acks") config.acks = next();
    else if (arg == "--compression") config.compression = next();
    else if (arg == "--linger-ms") config.linger_ms = static_cast<std::uint32_t>(std::stoul(next()));
    else if (arg == "--batch-num-messages") config.batch_num_messages = static_cast<std::uint32_t>(std::stoul(next()));
    else if (arg == "--csv") config.csv_path = next();
    else if (arg == "--metadata") config.metadata_path = next();
    else throw std::runtime_error("unknown argument: " + arg);
  }
  if (config.iterations < 1'000'000 || config.repetitions < 10) {
    throw std::runtime_error("iterations must be >= 1000000 and repetitions must be >= 10");
  }
  return config;
}

void write_metadata(const Config& config) {
  std::ofstream metadata(config.metadata_path);
  metadata << "iterations=" << config.iterations << '\n';
  metadata << "repetitions=" << config.repetitions << '\n';
  metadata << "warmup_iterations=" << config.warmup_iterations << '\n';
  metadata << "encoding_plus_kafka_produce=true\n";
  metadata << "decode_benchmark=false\n";
  metadata << "mode=" << config.mode << '\n';
  metadata << "acks=" << config.acks << '\n';
  metadata << "compression=" << config.compression << '\n';
  metadata << "linger_ms=" << config.linger_ms << '\n';
  metadata << "batch_num_messages=" << config.batch_num_messages << '\n';
  metadata << "compiler=" << __VERSION__ << '\n';
}

}  // namespace

int main(int argc, char** argv) {
  try {
    const Config config = parse_args(argc, argv);
    write_metadata(config);
    run(config);
    std::cerr << "Kafka producer benchmark complete: " << config.csv_path << '\n';
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "ERROR: " << error.what() << '\n';
    return 1;
  }
}

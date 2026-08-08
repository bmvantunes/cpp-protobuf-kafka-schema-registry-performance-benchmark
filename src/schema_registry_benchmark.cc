#include "generated/google_speed/google_speed/market.pb.h"

#include <curl/curl.h>

#include <array>
#include <barrier>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;
using Nanoseconds = std::chrono::duration<double, std::nano>;
using Message = benchmark::google_speed::OneStringTenInt64;
using DecimalMessage = benchmark::google_speed::OneStringTenDecimal;
using WideMessage = benchmark::google_speed::TenStringFiftyDecimal;

enum class Case { Int64, Decimal, Wide };

const char* case_name(Case test_case) {
  switch (test_case) {
    case Case::Int64: return "one_string_ten_int64";
    case Case::Decimal: return "one_string_ten_decimal_strings";
    case Case::Wide: return "ten_strings_fifty_decimal_strings";
  }
  return "unknown";
}

std::string symbol(std::size_t index) {
  return "SYMBOL_" + std::to_string(index + 1);
}

std::string decimal(std::size_t index) {
  return "12345." + std::string(8 + (index % 7), '0') + std::to_string(index + 17);
}

template <typename T>
inline void do_not_optimize(const T& value) {
#if defined(__GNUC__) || defined(__clang__)
  asm volatile("" : : "m"(value) : "memory");
#else
  (void)value;
#endif
}

#define SET_INT64_FIELDS(m) \
  m.set_value_01(1000001); m.set_value_02(1000002); m.set_value_03(1000003); \
  m.set_value_04(1000004); m.set_value_05(1000005); m.set_value_06(1000006); \
  m.set_value_07(1000007); m.set_value_08(1000008); m.set_value_09(1000009); \
  m.set_value_10(1000010)

#define SET_DECIMAL_FIELDS(m) \
  m.set_decimal_01(decimal(0)); m.set_decimal_02(decimal(1)); m.set_decimal_03(decimal(2)); \
  m.set_decimal_04(decimal(3)); m.set_decimal_05(decimal(4)); m.set_decimal_06(decimal(5)); \
  m.set_decimal_07(decimal(6)); m.set_decimal_08(decimal(7)); m.set_decimal_09(decimal(8)); \
  m.set_decimal_10(decimal(9))

#define SET_WIDE_SYMBOL_FIELDS(m) \
  m.set_symbol_01(symbol(0)); m.set_symbol_02(symbol(1)); m.set_symbol_03(symbol(2)); \
  m.set_symbol_04(symbol(3)); m.set_symbol_05(symbol(4)); m.set_symbol_06(symbol(5)); \
  m.set_symbol_07(symbol(6)); m.set_symbol_08(symbol(7)); m.set_symbol_09(symbol(8)); \
  m.set_symbol_10(symbol(9))

#define SET_WIDE_DECIMAL_FIELDS(m) \
  m.set_decimal_01(decimal(0)); m.set_decimal_02(decimal(1)); m.set_decimal_03(decimal(2)); \
  m.set_decimal_04(decimal(3)); m.set_decimal_05(decimal(4)); m.set_decimal_06(decimal(5)); \
  m.set_decimal_07(decimal(6)); m.set_decimal_08(decimal(7)); m.set_decimal_09(decimal(8)); \
  m.set_decimal_10(decimal(9)); m.set_decimal_11(decimal(10)); m.set_decimal_12(decimal(11)); \
  m.set_decimal_13(decimal(12)); m.set_decimal_14(decimal(13)); m.set_decimal_15(decimal(14)); \
  m.set_decimal_16(decimal(15)); m.set_decimal_17(decimal(16)); m.set_decimal_18(decimal(17)); \
  m.set_decimal_19(decimal(18)); m.set_decimal_20(decimal(19)); m.set_decimal_21(decimal(20)); \
  m.set_decimal_22(decimal(21)); m.set_decimal_23(decimal(22)); m.set_decimal_24(decimal(23)); \
  m.set_decimal_25(decimal(24)); m.set_decimal_26(decimal(25)); m.set_decimal_27(decimal(26)); \
  m.set_decimal_28(decimal(27)); m.set_decimal_29(decimal(28)); m.set_decimal_30(decimal(29)); \
  m.set_decimal_31(decimal(30)); m.set_decimal_32(decimal(31)); m.set_decimal_33(decimal(32)); \
  m.set_decimal_34(decimal(33)); m.set_decimal_35(decimal(34)); m.set_decimal_36(decimal(35)); \
  m.set_decimal_37(decimal(36)); m.set_decimal_38(decimal(37)); m.set_decimal_39(decimal(38)); \
  m.set_decimal_40(decimal(39)); m.set_decimal_41(decimal(40)); m.set_decimal_42(decimal(41)); \
  m.set_decimal_43(decimal(42)); m.set_decimal_44(decimal(43)); m.set_decimal_45(decimal(44)); \
  m.set_decimal_46(decimal(45)); m.set_decimal_47(decimal(46)); m.set_decimal_48(decimal(47)); \
  m.set_decimal_49(decimal(48)); m.set_decimal_50(decimal(49))

Message make_int64() {
  Message message;
  message.set_symbol("BTC-USD");
  SET_INT64_FIELDS(message);
  return message;
}

DecimalMessage make_decimal() {
  DecimalMessage message;
  message.set_symbol("BTC-USD");
  SET_DECIMAL_FIELDS(message);
  return message;
}

WideMessage make_wide() {
  WideMessage message;
  SET_WIDE_SYMBOL_FIELDS(message);
  SET_WIDE_DECIMAL_FIELDS(message);
  return message;
}

struct Config {
  std::uint64_t iterations = 1'000'000;
  std::uint32_t repetitions = 10;
  std::uint64_t warmup_iterations = 10'000;
  std::uint32_t network_repetitions = 10;
  std::string registry_url = "http://schema-registry:8081";
  std::string failure_url = "http://schema-registry:65530";
  std::string csv_path = "/work/results/schema_registry_raw.csv";
  std::string metadata_path = "/work/results/schema_registry_metadata.txt";
};

struct Csv {
  explicit Csv(const std::string& path) : file(path) {
    if (!file) throw std::runtime_error("cannot open CSV output: " + path);
    file << "kind,library,codegen,api,test_case,repetition,iterations,bytes,elapsed_ns,ns_per_encode,encodes_per_second,checksum\n";
  }
  std::ofstream file;
};

template <typename Encode>
void run_measure(Csv& csv, const std::string& api, Case test_case, const Config& config,
                 std::size_t bytes_hint, Encode&& encode) {
  for (std::uint64_t i = 0; i < config.warmup_iterations; ++i) {
    const auto bytes = encode();
    do_not_optimize(bytes);
  }
  for (std::uint32_t repetition = 1; repetition <= config.repetitions; ++repetition) {
    std::uint64_t checksum = 0;
    std::size_t bytes = bytes_hint;
    const auto start = Clock::now();
    for (std::uint64_t i = 0; i < config.iterations; ++i) {
      bytes = encode();
      checksum += bytes;
    }
    const auto elapsed = std::chrono::duration_cast<Nanoseconds>(Clock::now() - start).count();
    do_not_optimize(checksum);
    const double ns_per_encode = elapsed / static_cast<double>(config.iterations);
    csv.file << "registry,confluent_schema_registry,google_speed," << api << ',' << case_name(test_case)
             << ',' << repetition << ',' << config.iterations << ',' << bytes << ','
             << std::fixed << std::setprecision(3) << elapsed << ',' << ns_per_encode << ','
             << (1'000'000'000.0 / ns_per_encode) << ',' << checksum << '\n';
  }
}

std::array<std::uint8_t, 6> prefix(std::uint32_t schema_id) {
  // Confluent Protobuf framing for a single top-level message:
  // magic byte 0, big-endian schema ID, and the one-byte message-index [0].
  return {0, static_cast<std::uint8_t>(schema_id >> 24), static_cast<std::uint8_t>(schema_id >> 16),
          static_cast<std::uint8_t>(schema_id >> 8), static_cast<std::uint8_t>(schema_id), 0};
}

template <typename MessageType>
void run_framing_for_message(Csv& csv, const Config& config, Case test_case, MessageType message, std::uint32_t schema_id) {
  const std::size_t payload_bytes = message.ByteSizeLong();
  const auto header = prefix(schema_id);
  std::vector<std::uint8_t> framed(payload_bytes + header.size());
  std::memcpy(framed.data(), header.data(), header.size());

  run_measure(csv, "cached_id_framed_in_place", test_case, config, framed.size(), [&] {
    std::memcpy(framed.data(), header.data(), header.size());
    return message.SerializeToArray(framed.data() + header.size(), static_cast<int>(payload_bytes)) ? framed.size() : 0;
  });

  std::vector<std::uint8_t> payload(payload_bytes);
  run_measure(csv, "cached_id_framed_with_copy", test_case, config, framed.size(), [&] {
    const bool ok = message.SerializeToArray(payload.data(), static_cast<int>(payload_bytes));
    if (ok) {
      std::memcpy(framed.data(), header.data(), header.size());
      std::memcpy(framed.data() + header.size(), payload.data(), payload_bytes);
    }
    return ok ? framed.size() : 0;
  });

  std::string serialized;
  serialized.reserve(payload_bytes);
  std::string framed_string;
  framed_string.reserve(payload_bytes + header.size());
  run_measure(csv, "cached_serializer_string", test_case, config, framed_string.capacity(), [&] {
    if (!message.SerializeToString(&serialized)) return std::size_t{0};
    framed_string.clear();
    framed_string.append(reinterpret_cast<const char*>(header.data()), header.size());
    framed_string.append(serialized);
    return framed_string.size();
  });
}

struct HttpResult {
  long status = 0;
  std::string body;
};

size_t write_body(char* data, size_t size, size_t count, void* userdata) {
  auto* body = static_cast<std::string*>(userdata);
  body->append(data, size * count);
  return size * count;
}

HttpResult http_request(CURL* curl, const std::string& url, const char* method, const std::string& body,
                        struct curl_slist* headers, long timeout_ms = 30'000L) {
  HttpResult result;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_body);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result.body);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout_ms);
  curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
  if (std::strcmp(method, "POST") == 0) {
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 0L);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(body.size()));
  } else {
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, nullptr);
    curl_easy_setopt(curl, CURLOPT_POST, 0L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, nullptr);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
  }
  const auto code = curl_easy_perform(curl);
  if (code != CURLE_OK) throw std::runtime_error(curl_easy_strerror(code));
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &result.status);
  return result;
}

std::string json_escape(std::string_view value) {
  std::string escaped;
  for (const char character : value) {
    switch (character) {
      case '"': escaped += "\\\""; break;
      case '\\': escaped += "\\\\"; break;
      case '\n': escaped += "\\n"; break;
      case '\r': escaped += "\\r"; break;
      case '\t': escaped += "\\t"; break;
      default: escaped += character; break;
    }
  }
  return escaped;
}

std::uint32_t response_id(const HttpResult& result) {
  if (result.status < 200 || result.status >= 300) {
    throw std::runtime_error("Schema Registry HTTP status " + std::to_string(result.status) + ": " + result.body);
  }
  const auto id_position = result.body.find("\"id\"");
  if (id_position == std::string::npos) throw std::runtime_error("Schema Registry response has no id: " + result.body);
  const auto colon = result.body.find(':', id_position);
  return static_cast<std::uint32_t>(std::stoul(result.body.substr(colon + 1)));
}

std::string registration_body(const std::string& schema) {
  return "{\"schemaType\":\"PROTOBUF\",\"schema\":\"" + json_escape(schema) + "\"}";
}

std::uint32_t register_schema(CURL* curl, struct curl_slist* headers, const std::string& url,
                              const std::string& subject, const std::string& schema) {
  const auto result = http_request(curl, url + "/subjects/" + subject + "/versions", "POST", registration_body(schema), headers);
  return response_id(result);
}

void run_network_rows(Csv& csv, const Config& config, const std::string& registry_url, const std::string& subject,
                      const std::string& schema, bool cold_connection, bool registration) {
  struct curl_slist* headers = curl_slist_append(nullptr, "Content-Type: application/vnd.schemaregistry.v1+json");
  CURL* reusable = cold_connection ? nullptr : curl_easy_init();
  if (!cold_connection && reusable == nullptr) throw std::runtime_error("curl_easy_init failed");

  for (std::uint32_t repetition = 1; repetition <= config.network_repetitions; ++repetition) {
    CURL* curl = cold_connection ? curl_easy_init() : reusable;
    const std::string current_subject = registration ? subject + "_" + std::to_string(repetition) : subject;
    const std::string url = registration
      ? registry_url + "/subjects/" + current_subject + "/versions"
      : registry_url + "/subjects/" + current_subject + "/versions/latest";
    const char* method = registration ? "POST" : "GET";
    const auto start = Clock::now();
    const auto result = http_request(curl, url, method, registration ? registration_body(schema) : "", headers);
    const auto elapsed = std::chrono::duration_cast<Nanoseconds>(Clock::now() - start).count();
    if (result.status < 200 || result.status >= 300) {
      throw std::runtime_error(std::string("Schema Registry ") + method + " " + url + " returned HTTP " +
                               std::to_string(result.status) + ": " + result.body);
    }
    const auto id = response_id(result);
    do_not_optimize(id);
    if (cold_connection) curl_easy_cleanup(curl);
    const double ns_per_request = elapsed;
    csv.file << "registry,confluent_schema_registry,http," << (registration ? "cold_register" : (cold_connection ? "cold_lookup_new_connection" : "registry_lookup_keepalive"))
             << ',' << (registration ? "schema_registry_registration" : "schema_registry_lookup") << ',' << repetition
             << ",1," << result.body.size() << ',' << std::fixed << std::setprecision(3) << elapsed << ','
             << ns_per_request << ',' << (1'000'000'000.0 / ns_per_request) << ',' << id << '\n';
  }
  if (reusable != nullptr) curl_easy_cleanup(reusable);
  curl_slist_free_all(headers);
}

void write_control_row(Csv& csv, const char* api, const char* test_case, std::uint32_t repetition,
                       double elapsed, std::size_t bytes, long status) {
  csv.file << "registry,confluent_schema_registry,http," << api << ',' << test_case << ',' << repetition
           << ",1," << bytes << ',' << std::fixed << std::setprecision(3) << elapsed << ',' << elapsed << ','
           << (1'000'000'000.0 / elapsed) << ',' << status << '\n';
}

void run_exceptional_paths(Csv& csv, const Config& config, const std::string& registry_url,
                           const std::string& failure_url, const std::string& subject) {
  struct curl_slist* headers = curl_slist_append(nullptr, "Content-Type: application/vnd.schemaregistry.v1+json");
  auto run_request_path = [&](const char* api, const std::string& url, const char* expected_case, long timeout_ms) {
    for (std::uint32_t repetition = 1; repetition <= config.network_repetitions; ++repetition) {
      CURL* curl = curl_easy_init();
      if (curl == nullptr) throw std::runtime_error("curl_easy_init failed");
      const auto start = Clock::now();
      long status = 0;
      std::size_t bytes = 0;
      try {
        const auto result = http_request(curl, url, "GET", "", headers, timeout_ms);
        status = result.status;
        bytes = result.body.size();
      } catch (...) {
        status = 0;
      }
      const auto elapsed = std::chrono::duration_cast<Nanoseconds>(Clock::now() - start).count();
      write_control_row(csv, api, expected_case, repetition, elapsed, bytes, status);
      curl_easy_cleanup(curl);
    }
  };

  run_request_path("cache_miss_404", registry_url + "/subjects/" + subject + "_missing/versions/latest",
                   "schema_registry_cache_miss", 5'000L);
  run_request_path("registry_unavailable", failure_url + "/subjects/unavailable/versions/latest",
                   "schema_registry_unavailable", 500L);

  for (std::uint32_t repetition = 1; repetition <= config.network_repetitions; ++repetition) {
    CURL* failed = curl_easy_init();
    CURL* successful = curl_easy_init();
    if (failed == nullptr || successful == nullptr) throw std::runtime_error("curl_easy_init failed");
    const auto start = Clock::now();
    long status = 0;
    std::size_t bytes = 0;
    try {
      http_request(failed, failure_url + "/subjects/retry/versions/latest", "GET", "", headers, 500L);
    } catch (...) {
      // The first failure is intentional; continue to exercise recovery.
    }
    try {
      const auto result = http_request(successful, registry_url + "/subjects/" + subject + "/versions/latest", "GET", "", headers);
      status = result.status;
      bytes = result.body.size();
    } catch (...) {
      status = 0;
    }
    const auto elapsed = std::chrono::duration_cast<Nanoseconds>(Clock::now() - start).count();
    write_control_row(csv, "retry_failure_then_success", "schema_registry_retry", repetition, elapsed, bytes, status);
    curl_easy_cleanup(failed);
    curl_easy_cleanup(successful);
  }
  curl_slist_free_all(headers);
}

void run_concurrent_registration(Csv& csv, const Config& config, const std::string& registry_url,
                                 const std::string& subject, const std::string& schema) {
  std::mutex csv_mutex;
  std::barrier start_barrier(static_cast<std::ptrdiff_t>(config.network_repetitions + 1));
  std::vector<std::thread> workers;
  workers.reserve(config.network_repetitions);
  for (std::uint32_t repetition = 1; repetition <= config.network_repetitions; ++repetition) {
    workers.emplace_back([&, repetition] {
      struct curl_slist* headers = curl_slist_append(nullptr, "Content-Type: application/vnd.schemaregistry.v1+json");
      CURL* curl = curl_easy_init();
      if (curl == nullptr) throw std::runtime_error("curl_easy_init failed");
      const auto current_subject = subject + "_concurrent_" + std::to_string(repetition);
      start_barrier.arrive_and_wait();
      const auto start = Clock::now();
      long status = 0;
      std::size_t bytes = 0;
      try {
        const auto result = http_request(curl, registry_url + "/subjects/" + current_subject + "/versions", "POST",
                                         registration_body(schema), headers);
        status = result.status;
        bytes = result.body.size();
      } catch (...) {
        status = 0;
      }
      const auto elapsed = std::chrono::duration_cast<Nanoseconds>(Clock::now() - start).count();
      {
        std::lock_guard lock(csv_mutex);
        write_control_row(csv, "concurrent_registration", "schema_registry_concurrent_register", repetition, elapsed, bytes, status);
      }
      curl_easy_cleanup(curl);
      curl_slist_free_all(headers);
    });
  }
  start_barrier.arrive_and_wait();
  for (auto& worker : workers) worker.join();
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
    else if (arg == "--network-repetitions") config.network_repetitions = static_cast<std::uint32_t>(std::stoul(next()));
    else if (arg == "--registry-url") config.registry_url = next();
    else if (arg == "--failure-url") config.failure_url = next();
    else if (arg == "--csv") config.csv_path = next();
    else if (arg == "--metadata") config.metadata_path = next();
    else throw std::runtime_error("unknown argument: " + arg);
  }
  if (config.iterations < 1'000'000 || config.repetitions < 10 || config.network_repetitions < 10) {
    throw std::runtime_error("steady-state iterations/repetitions and network repetitions are undersized");
  }
  return config;
}

std::string read_file(const std::string& path) {
  std::ifstream file(path);
  if (!file) throw std::runtime_error("cannot read " + path);
  std::ostringstream contents;
  contents << file.rdbuf();
  return contents.str();
}

void write_metadata(const Config& config) {
  std::ofstream metadata(config.metadata_path);
  metadata << "steady_state_iterations=" << config.iterations << '\n';
  metadata << "steady_state_repetitions=" << config.repetitions << '\n';
  metadata << "steady_state_warmup_iterations=" << config.warmup_iterations << '\n';
  metadata << "network_repetitions=" << config.network_repetitions << '\n';
  metadata << "network_iterations_per_repetition=1\n";
  metadata << "registry_url=" << config.registry_url << '\n';
  metadata << "failure_url=" << config.failure_url << '\n';
  metadata << "confluent_protobuf_prefix_bytes=6\n";
  metadata << "cold_network_paths_are_not_1m_by_design=true\n";
}

}  // namespace

int main(int argc, char** argv) {
  try {
    const Config config = parse_args(argc, argv);
    write_metadata(config);
    Csv csv(config.csv_path);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    const auto schema = read_file("proto/google_speed/market.proto");
    const std::string run_id = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now().time_since_epoch()).count());
    const std::string base_subject = "protobuf_encoding_benchmark_" + run_id;
    CURL* bootstrap_curl = curl_easy_init();
    if (bootstrap_curl == nullptr) throw std::runtime_error("curl_easy_init failed");
    struct curl_slist* bootstrap_headers = curl_slist_append(nullptr, "Content-Type: application/vnd.schemaregistry.v1+json");
    const auto schema_id = register_schema(bootstrap_curl, bootstrap_headers, config.registry_url, base_subject, schema);
    curl_slist_free_all(bootstrap_headers);
    curl_easy_cleanup(bootstrap_curl);

    run_framing_for_message(csv, config, Case::Int64, make_int64(), schema_id);
    run_framing_for_message(csv, config, Case::Decimal, make_decimal(), schema_id);
    run_framing_for_message(csv, config, Case::Wide, make_wide(), schema_id);

    run_network_rows(csv, config, config.registry_url, base_subject, schema, false, false);
    run_network_rows(csv, config, config.registry_url, base_subject, schema, true, false);
    run_network_rows(csv, config, config.registry_url, base_subject, schema, false, true);
    run_exceptional_paths(csv, config, config.registry_url, config.failure_url, base_subject);
    run_concurrent_registration(csv, config, config.registry_url, base_subject, schema);

    curl_global_cleanup();
    std::cerr << "Schema Registry benchmark complete: " << config.csv_path << '\n';
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "ERROR: " << error.what() << '\n';
    return 1;
  }
}

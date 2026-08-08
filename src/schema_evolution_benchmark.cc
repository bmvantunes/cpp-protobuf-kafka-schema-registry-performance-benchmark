#include <curl/curl.h>

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

using Clock = std::chrono::steady_clock;
using Nanoseconds = std::chrono::duration<double, std::nano>;

struct Config {
  std::uint32_t repetitions = 10;
  std::string registry_url = "http://schema-registry:8081";
  std::string csv_path = "/work/results/schema_evolution_raw.csv";
  std::string metadata_path = "/work/results/schema_evolution_metadata.txt";
};

struct Result { long status = 0; std::string body; };

size_t write_body(char* data, size_t size, size_t count, void* userdata) {
  auto* body = static_cast<std::string*>(userdata);
  body->append(data, size * count);
  return size * count;
}

Result request(CURL* curl, const std::string& url, const char* method, const std::string& body,
               struct curl_slist* headers) {
  Result result;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_body);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result.body);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 30'000L);
  curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
  if (std::string(method) == "POST") {
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

std::string read_file(const std::string& path) {
  std::ifstream file(path);
  if (!file) throw std::runtime_error("cannot read " + path);
  std::ostringstream contents;
  contents << file.rdbuf();
  return contents.str();
}

std::string escape_json(const std::string& value) {
  std::string escaped;
  for (const char character : value) {
    if (character == '\\') escaped += "\\\\";
    else if (character == '"') escaped += "\\\"";
    else if (character == '\n') escaped += "\\n";
    else escaped += character;
  }
  return escaped;
}

std::string registration_body(const std::string& schema) {
  return "{\"schemaType\":\"PROTOBUF\",\"schema\":\"" + escape_json(schema) + "\"}";
}

void write_row(std::ofstream& csv, const char* phase, std::uint32_t repetition, double elapsed,
               const Result& result) {
  csv << phase << ',' << repetition << ',' << result.status << ',' << result.body.size() << ','
      << std::fixed << std::setprecision(3) << elapsed << '\n';
  if (result.status >= 300 || result.status == 0) {
    std::cerr << phase << " repetition " << repetition << " returned " << result.status << ": " << result.body << '\n';
  }
}

Config parse_args(int argc, char** argv) {
  Config config;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    auto next = [&] { if (i + 1 >= argc) throw std::runtime_error("missing value for " + arg); return std::string(argv[++i]); };
    if (arg == "--repetitions") config.repetitions = static_cast<std::uint32_t>(std::stoul(next()));
    else if (arg == "--registry-url") config.registry_url = next();
    else if (arg == "--csv") config.csv_path = next();
    else if (arg == "--metadata") config.metadata_path = next();
    else throw std::runtime_error("unknown argument: " + arg);
  }
  if (config.repetitions < 10) throw std::runtime_error("repetitions must be >= 10");
  return config;
}

}  // namespace

int main(int argc, char** argv) {
  try {
    const Config config = parse_args(argc, argv);
    const auto v1 = read_file("proto/google_speed/market.proto");
    const auto v2 = read_file("schemas/schema_evolution/market_v2.proto");
    std::ofstream metadata(config.metadata_path);
    metadata << "repetitions=" << config.repetitions << '\n' << "v1_schema=proto/google_speed/market.proto\n"
             << "v2_schema=schemas/schema_evolution/market_v2.proto\n";
    std::ofstream csv(config.csv_path);
    csv << "phase,repetition,status,bytes,elapsed_ns\n";
    curl_global_init(CURL_GLOBAL_DEFAULT);
    struct curl_slist* headers = curl_slist_append(nullptr, "Content-Type: application/vnd.schemaregistry.v1+json");
    for (std::uint32_t repetition = 1; repetition <= config.repetitions; ++repetition) {
      CURL* curl = curl_easy_init();
      if (curl == nullptr) throw std::runtime_error("curl_easy_init failed");
      const std::string subject = "schema_evolution_benchmark_" + std::to_string(repetition);
      for (const auto& phase : {std::pair{"register_v1", v1}, std::pair{"register_v2", v2}}) {
        const auto start = Clock::now();
        const auto result = request(curl, config.registry_url + "/subjects/" + subject + "/versions", "POST", registration_body(phase.second), headers);
        const auto elapsed = std::chrono::duration_cast<Nanoseconds>(Clock::now() - start).count();
        write_row(csv, phase.first, repetition, elapsed, result);
      }
      const auto start = Clock::now();
      const auto result = request(curl, config.registry_url + "/subjects/" + subject + "/versions", "GET", "", headers);
      const auto elapsed = std::chrono::duration_cast<Nanoseconds>(Clock::now() - start).count();
      write_row(csv, "lookup_versions", repetition, elapsed, result);
      curl_easy_cleanup(curl);
    }
    curl_slist_free_all(headers);
    curl_global_cleanup();
    std::cerr << "Schema evolution benchmark complete: " << config.csv_path << '\n';
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "ERROR: " << error.what() << '\n';
    return 1;
  }
}

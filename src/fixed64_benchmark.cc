#include "generated/google_speed/google_speed/market.pb.h"
#include "generated/representations/representations/decimal.pb.h"

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;
using Nanoseconds = std::chrono::duration<double, std::nano>;
using IntMessage = benchmark::google_speed::OneStringTenInt64;
using FixedMessage = benchmark::representations::OneStringTenDecimalFixed64;

template <typename T>
inline void do_not_optimize(const T& value) {
#if defined(__GNUC__) || defined(__clang__)
  asm volatile("" : : "m"(value) : "memory");
#else
  (void)value;
#endif
}

struct Config {
  std::uint64_t iterations = 1'000'000;
  std::uint32_t repetitions = 10;
  std::uint64_t warmup_iterations = 10'000;
  std::string csv_path = "/work/results/fixed64_raw.csv";
  std::string metadata_path = "/work/results/fixed64_metadata.txt";
};

struct Csv {
  explicit Csv(const std::string& path) : file(path) {
    if (!file) throw std::runtime_error("cannot open CSV output: " + path);
    file << "test_case,representation,bytes,repetition,iterations,elapsed_ns,ns_per_encode,encodes_per_second,checksum\n";
  }
  std::ofstream file;
};

std::uint64_t value_for(const char* test_case, int index) {
  if (std::string(test_case) == "positive_small") return 1'000'001ULL + static_cast<std::uint64_t>(index);
  if (std::string(test_case) == "positive_scaled_price") return 1'234'500'000'000ULL + static_cast<std::uint64_t>(index);
  if (std::string(test_case) == "positive_near_max") return 9'000'000'000'000'000'000ULL + static_cast<std::uint64_t>(index);
  throw std::runtime_error("unknown test case");
}

IntMessage make_int_message(const char* test_case) {
  IntMessage message;
  message.set_symbol("BTC-USD");
  message.set_value_01(static_cast<std::int64_t>(value_for(test_case, 0)));
  message.set_value_02(static_cast<std::int64_t>(value_for(test_case, 1)));
  message.set_value_03(static_cast<std::int64_t>(value_for(test_case, 2)));
  message.set_value_04(static_cast<std::int64_t>(value_for(test_case, 3)));
  message.set_value_05(static_cast<std::int64_t>(value_for(test_case, 4)));
  message.set_value_06(static_cast<std::int64_t>(value_for(test_case, 5)));
  message.set_value_07(static_cast<std::int64_t>(value_for(test_case, 6)));
  message.set_value_08(static_cast<std::int64_t>(value_for(test_case, 7)));
  message.set_value_09(static_cast<std::int64_t>(value_for(test_case, 8)));
  message.set_value_10(static_cast<std::int64_t>(value_for(test_case, 9)));
  return message;
}

FixedMessage make_fixed_message(const char* test_case) {
  FixedMessage message;
  message.set_symbol("BTC-USD");
  message.set_decimal_01(value_for(test_case, 0)); message.set_decimal_02(value_for(test_case, 1));
  message.set_decimal_03(value_for(test_case, 2)); message.set_decimal_04(value_for(test_case, 3));
  message.set_decimal_05(value_for(test_case, 4)); message.set_decimal_06(value_for(test_case, 5));
  message.set_decimal_07(value_for(test_case, 6)); message.set_decimal_08(value_for(test_case, 7));
  message.set_decimal_09(value_for(test_case, 8)); message.set_decimal_10(value_for(test_case, 9));
  return message;
}

template <typename Message>
void run_message(Csv& csv, const Config& config, const char* test_case,
                 const char* representation, Message message) {
  const auto bytes = message.ByteSizeLong();
  std::vector<std::uint8_t> buffer(bytes);
  for (std::uint64_t i = 0; i < config.warmup_iterations; ++i) {
    do_not_optimize(message.SerializeToArray(buffer.data(), static_cast<int>(buffer.size())));
  }
  for (std::uint32_t repetition = 1; repetition <= config.repetitions; ++repetition) {
    std::uint64_t checksum = 0;
    const auto start = Clock::now();
    for (std::uint64_t i = 0; i < config.iterations; ++i) {
      const bool ok = message.SerializeToArray(buffer.data(), static_cast<int>(buffer.size()));
      checksum += ok ? bytes : 0;
    }
    const auto elapsed = std::chrono::duration_cast<Nanoseconds>(Clock::now() - start).count();
    do_not_optimize(checksum);
    const auto ns_per_encode = elapsed / static_cast<double>(config.iterations);
    csv.file << test_case << ',' << representation << ',' << bytes << ',' << repetition << ',' << config.iterations << ','
             << std::fixed << std::setprecision(3) << elapsed << ',' << ns_per_encode << ','
             << (1'000'000'000.0 / ns_per_encode) << ',' << checksum << '\n';
  }
}

Config parse_args(int argc, char** argv) {
  Config config;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    auto next = [&] { if (i + 1 >= argc) throw std::runtime_error("missing value for " + arg); return std::string(argv[++i]); };
    if (arg == "--iterations") config.iterations = std::stoull(next());
    else if (arg == "--repetitions") config.repetitions = static_cast<std::uint32_t>(std::stoul(next()));
    else if (arg == "--warmup-iterations") config.warmup_iterations = std::stoull(next());
    else if (arg == "--csv") config.csv_path = next();
    else if (arg == "--metadata") config.metadata_path = next();
    else throw std::runtime_error("unknown argument: " + arg);
  }
  if (config.iterations < 1'000'000 || config.repetitions < 10) {
    throw std::runtime_error("iterations must be >= 1000000 and repetitions must be >= 10");
  }
  return config;
}

}  // namespace

int main(int argc, char** argv) {
  try {
    const Config config = parse_args(argc, argv);
    std::ofstream metadata(config.metadata_path);
    metadata << "iterations=" << config.iterations << '\n'
             << "repetitions=" << config.repetitions << '\n'
             << "warmup_iterations=" << config.warmup_iterations << '\n'
             << "encoding_only=true\n"
             << "comparison=identical_positive_values_int64_vs_fixed64\n";
    Csv csv(config.csv_path);
    for (const char* test_case : {"positive_small", "positive_scaled_price", "positive_near_max"}) {
      run_message(csv, config, test_case, "int64", make_int_message(test_case));
      run_message(csv, config, test_case, "fixed64", make_fixed_message(test_case));
    }
    std::cerr << "fixed64 benchmark complete: " << config.csv_path << '\n';
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "ERROR: " << error.what() << '\n';
    return 1;
  }
}

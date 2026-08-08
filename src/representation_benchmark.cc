#include "generated/google_speed/google_speed/market.pb.h"
#include "generated/representations/representations/decimal.pb.h"

#include <algorithm>
#include <array>
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

template <typename T>
inline void do_not_optimize(const T& value) {
#if defined(__GNUC__) || defined(__clang__)
  asm volatile("" : : "m"(value) : "memory");
#else
  (void)value;
#endif
}

std::string decimal_text(std::size_t index) {
  return "12345." + std::string(6, '0') + std::to_string(index + 17);
}

std::uint64_t scaled_value(std::size_t index) {
  return 1'234'500'000'000ULL + static_cast<std::uint64_t>(index + 17);
}

std::string decimal128_bytes(std::size_t index) {
  const std::uint64_t value = scaled_value(index);
  std::string bytes(16, '\0');
  for (std::size_t byte = 0; byte < sizeof(value); ++byte) {
    bytes[byte] = static_cast<char>(value >> (byte * 8));
  }
  return bytes;
}

enum class Representation { String, ScaledSint64, Fixed64, Bytes128 };

const char* representation_name(Representation representation) {
  switch (representation) {
    case Representation::String: return "decimal_string";
    case Representation::ScaledSint64: return "scaled_sint64_scale_8";
    case Representation::Fixed64: return "fixed64_scale_8";
    case Representation::Bytes128: return "bytes_128bit_coefficient_scale_8";
  }
  return "unknown";
}

struct Config {
  std::uint64_t iterations = 1'000'000;
  std::uint32_t repetitions = 10;
  std::uint64_t warmup_iterations = 10'000;
  std::string csv_path = "/work/results/representation_raw.csv";
  std::string metadata_path = "/work/results/representation_metadata.txt";
};

struct Csv {
  explicit Csv(const std::string& path) : file(path) {
    if (!file) throw std::runtime_error("cannot open CSV output: " + path);
    file << "kind,representation,api,test_case,repetition,iterations,bytes,elapsed_ns,ns_per_encode,encodes_per_second,checksum\n";
  }
  std::ofstream file;
};

template <typename Encode>
void run_measure(Csv& csv, Representation representation, const Config& config,
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
    csv.file << "protobuf," << representation_name(representation) << ",SerializeToArray_preallocated,one_string_ten_decimal," << repetition
             << ',' << config.iterations << ',' << bytes << ',' << std::fixed << std::setprecision(3) << elapsed << ','
             << ns_per_encode << ',' << (1'000'000'000.0 / ns_per_encode) << ',' << checksum << '\n';
  }
}

template <typename Message>
std::size_t serialize_message(Message& message, std::vector<std::uint8_t>& buffer) {
  return message.SerializeToArray(buffer.data(), static_cast<int>(buffer.size())) ? buffer.size() : 0;
}

benchmark::google_speed::OneStringTenDecimal make_string_message() {
  benchmark::google_speed::OneStringTenDecimal message;
  message.set_symbol("BTC-USD");
  message.set_decimal_01(decimal_text(0)); message.set_decimal_02(decimal_text(1));
  message.set_decimal_03(decimal_text(2)); message.set_decimal_04(decimal_text(3));
  message.set_decimal_05(decimal_text(4)); message.set_decimal_06(decimal_text(5));
  message.set_decimal_07(decimal_text(6)); message.set_decimal_08(decimal_text(7));
  message.set_decimal_09(decimal_text(8)); message.set_decimal_10(decimal_text(9));
  return message;
}

benchmark::representations::OneStringTenDecimalScaledSint64 make_sint_message() {
  benchmark::representations::OneStringTenDecimalScaledSint64 message;
  message.set_symbol("BTC-USD");
  message.set_decimal_01(static_cast<std::int64_t>(scaled_value(0))); message.set_decimal_02(static_cast<std::int64_t>(scaled_value(1)));
  message.set_decimal_03(static_cast<std::int64_t>(scaled_value(2))); message.set_decimal_04(static_cast<std::int64_t>(scaled_value(3)));
  message.set_decimal_05(static_cast<std::int64_t>(scaled_value(4))); message.set_decimal_06(static_cast<std::int64_t>(scaled_value(5)));
  message.set_decimal_07(static_cast<std::int64_t>(scaled_value(6))); message.set_decimal_08(static_cast<std::int64_t>(scaled_value(7)));
  message.set_decimal_09(static_cast<std::int64_t>(scaled_value(8))); message.set_decimal_10(static_cast<std::int64_t>(scaled_value(9)));
  return message;
}

benchmark::representations::OneStringTenDecimalFixed64 make_fixed_message() {
  benchmark::representations::OneStringTenDecimalFixed64 message;
  message.set_symbol("BTC-USD");
  message.set_decimal_01(scaled_value(0)); message.set_decimal_02(scaled_value(1));
  message.set_decimal_03(scaled_value(2)); message.set_decimal_04(scaled_value(3));
  message.set_decimal_05(scaled_value(4)); message.set_decimal_06(scaled_value(5));
  message.set_decimal_07(scaled_value(6)); message.set_decimal_08(scaled_value(7));
  message.set_decimal_09(scaled_value(8)); message.set_decimal_10(scaled_value(9));
  return message;
}

benchmark::representations::OneStringTenDecimalBytes128 make_bytes_message() {
  benchmark::representations::OneStringTenDecimalBytes128 message;
  message.set_symbol("BTC-USD");
  message.set_decimal_01(decimal128_bytes(0)); message.set_decimal_02(decimal128_bytes(1));
  message.set_decimal_03(decimal128_bytes(2)); message.set_decimal_04(decimal128_bytes(3));
  message.set_decimal_05(decimal128_bytes(4)); message.set_decimal_06(decimal128_bytes(5));
  message.set_decimal_07(decimal128_bytes(6)); message.set_decimal_08(decimal128_bytes(7));
  message.set_decimal_09(decimal128_bytes(8)); message.set_decimal_10(decimal128_bytes(9));
  return message;
}

template <typename Message>
void run_message(Csv& csv, Representation representation, const Config& config, Message message) {
  const std::size_t bytes = message.ByteSizeLong();
  std::vector<std::uint8_t> buffer(bytes);
  run_measure(csv, representation, config, bytes, [&] { return serialize_message(message, buffer); });
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
  metadata << "decimal_scale=8\n";
  metadata << "encoding_only=true\n";
  metadata << "decode_benchmark=false\n";
  metadata << "compiler=" << __VERSION__ << '\n';
}

}  // namespace

int main(int argc, char** argv) {
  try {
    const Config config = parse_args(argc, argv);
    write_metadata(config);
    Csv csv(config.csv_path);
    run_message(csv, Representation::String, config, make_string_message());
    run_message(csv, Representation::ScaledSint64, config, make_sint_message());
    run_message(csv, Representation::Fixed64, config, make_fixed_message());
    run_message(csv, Representation::Bytes128, config, make_bytes_message());
    std::cerr << "Representation benchmark complete: " << config.csv_path << '\n';
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "ERROR: " << error.what() << '\n';
    return 1;
  }
}

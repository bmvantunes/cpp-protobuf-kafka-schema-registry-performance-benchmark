#include "generated/google_speed/google_speed/market.pb.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;
using Nanoseconds = std::chrono::duration<double, std::nano>;
using Message = benchmark::google_speed::OneStringTenInt64;

template <typename T>
inline void do_not_optimize(const T& value) {
#if defined(__GNUC__) || defined(__clang__)
  asm volatile("" : : "m"(value) : "memory");
#else
  (void)value;
#endif
}

enum class Distribution { SmallInt64, LargeInt64, NegativeInt64, MixedInt64, ShortString, LongString, Utf8String };

const char* distribution_name(Distribution distribution) {
  switch (distribution) {
    case Distribution::SmallInt64: return "int64_small_varints";
    case Distribution::LargeInt64: return "int64_large_varints";
    case Distribution::NegativeInt64: return "int64_negative_standard_varint";
    case Distribution::MixedInt64: return "int64_mixed_distribution";
    case Distribution::ShortString: return "string_short_ascii";
    case Distribution::LongString: return "string_long_ascii_256";
    case Distribution::Utf8String: return "string_multibyte_utf8";
  }
  return "unknown";
}

Message make_message(Distribution distribution, std::size_t corpus_index = 0) {
  Message message;
  if (distribution == Distribution::ShortString) {
    message.set_symbol("BTC-USD");
  } else if (distribution == Distribution::LongString) {
    message.set_symbol(std::string(256, static_cast<char>('A' + corpus_index % 26)));
  } else if (distribution == Distribution::Utf8String) {
    message.set_symbol("交易对-🚀-BTC/USD-" + std::to_string(corpus_index));
  } else {
    message.set_symbol("BTC-USD");
  }

  for (int field = 1; field <= 10; ++field) {
    std::int64_t value = field;
    switch (distribution) {
      case Distribution::SmallInt64: value = field; break;
      case Distribution::LargeInt64: value = std::numeric_limits<std::int64_t>::max() - field; break;
      case Distribution::NegativeInt64: value = -field; break;
      case Distribution::MixedInt64:
        value = (corpus_index + field) % 4 == 0 ? -field
              : ((corpus_index + field) % 4 == 1 ? field
              : ((corpus_index + field) % 4 == 2 ? 1'000'000 + field
                                                  : std::numeric_limits<std::int64_t>::max() - field));
        break;
      case Distribution::ShortString: case Distribution::LongString: case Distribution::Utf8String:
        value = 1'000'000 + field;
        break;
    }
    switch (field) {
      case 1: message.set_value_01(value); break; case 2: message.set_value_02(value); break;
      case 3: message.set_value_03(value); break; case 4: message.set_value_04(value); break;
      case 5: message.set_value_05(value); break; case 6: message.set_value_06(value); break;
      case 7: message.set_value_07(value); break; case 8: message.set_value_08(value); break;
      case 9: message.set_value_09(value); break; case 10: message.set_value_10(value); break;
    }
  }
  return message;
}

struct Config {
  std::uint64_t iterations = 1'000'000;
  std::uint32_t repetitions = 10;
  std::uint64_t warmup_iterations = 10'000;
  std::string csv_path = "/work/results/distribution_raw.csv";
  std::string metadata_path = "/work/results/distribution_metadata.txt";
};

struct Csv {
  explicit Csv(const std::string& path) : file(path) {
    if (!file) throw std::runtime_error("cannot open CSV output: " + path);
    file << "distribution,repetition,iterations,mean_bytes,serialized_bytes_last,elapsed_ns,ns_per_encode,encodes_per_second,checksum\n";
  }
  std::ofstream file;
};

void run_distribution(Csv& csv, const Config& config, Distribution distribution) {
  constexpr std::size_t corpus_size = 1024;
  std::vector<Message> corpus;
  corpus.reserve(corpus_size);
  for (std::size_t i = 0; i < corpus_size; ++i) corpus.push_back(make_message(distribution, i));
  std::vector<std::vector<std::uint8_t>> buffers;
  buffers.reserve(corpus_size);
  for (const auto& message : corpus) buffers.emplace_back(message.ByteSizeLong());

  for (std::uint64_t i = 0; i < config.warmup_iterations; ++i) {
    const auto index = i % corpus_size;
    corpus[index].SerializeToArray(buffers[index].data(), static_cast<int>(buffers[index].size()));
  }
  for (std::uint32_t repetition = 1; repetition <= config.repetitions; ++repetition) {
    std::uint64_t checksum = 0;
    std::uint64_t total_bytes = 0;
    std::size_t last_bytes = 0;
    const auto start = Clock::now();
    for (std::uint64_t i = 0; i < config.iterations; ++i) {
      const auto index = i % corpus_size;
      last_bytes = corpus[index].ByteSizeLong();
      corpus[index].SerializeToArray(buffers[index].data(), static_cast<int>(buffers[index].size()));
      total_bytes += last_bytes;
      checksum += buffers[index][0];
    }
    const auto elapsed = std::chrono::duration_cast<Nanoseconds>(Clock::now() - start).count();
    do_not_optimize(checksum);
    const double ns_per_encode = elapsed / static_cast<double>(config.iterations);
    csv.file << distribution_name(distribution) << ',' << repetition << ',' << config.iterations << ','
             << (total_bytes / static_cast<double>(config.iterations)) << ',' << last_bytes << ','
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
  if (config.iterations < 1'000'000 || config.repetitions < 10) throw std::runtime_error("iterations must be >= 1000000 and repetitions must be >= 10");
  return config;
}

}  // namespace

int main(int argc, char** argv) {
  try {
    const Config config = parse_args(argc, argv);
    std::ofstream metadata(config.metadata_path);
    metadata << "iterations=" << config.iterations << '\n' << "repetitions=" << config.repetitions << '\n'
             << "warmup_iterations=" << config.warmup_iterations << '\n' << "corpus_size=1024\n";
    Csv csv(config.csv_path);
    for (const auto distribution : {Distribution::SmallInt64, Distribution::LargeInt64, Distribution::NegativeInt64,
                                    Distribution::MixedInt64, Distribution::ShortString, Distribution::LongString,
                                    Distribution::Utf8String}) {
      run_distribution(csv, config, distribution);
    }
    std::cerr << "Distribution benchmark complete: " << config.csv_path << '\n';
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "ERROR: " << error.what() << '\n';
    return 1;
  }
}

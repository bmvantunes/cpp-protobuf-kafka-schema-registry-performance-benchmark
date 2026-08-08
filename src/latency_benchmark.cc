#include "generated/google_speed/google_speed/market.pb.h"

#include <google/protobuf/arena.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <new>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::atomic<std::uint64_t> allocation_count{0};
std::atomic<std::uint64_t> allocated_bytes{0};

std::uint64_t now_ns() {
  return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::steady_clock::now().time_since_epoch()).count());
}

void* allocate(std::size_t size) {
  void* pointer = std::malloc(size == 0 ? 1 : size);
  if (pointer == nullptr) throw std::bad_alloc();
  allocation_count.fetch_add(1, std::memory_order_relaxed);
  allocated_bytes.fetch_add(size, std::memory_order_relaxed);
  return pointer;
}

}  // namespace

void* operator new(std::size_t size) { return allocate(size); }
void* operator new[](std::size_t size) { return allocate(size); }
void operator delete(void* pointer) noexcept { std::free(pointer); }
void operator delete[](void* pointer) noexcept { std::free(pointer); }
void operator delete(void* pointer, std::size_t) noexcept { std::free(pointer); }
void operator delete[](void* pointer, std::size_t) noexcept { std::free(pointer); }

namespace {

using MessageInt64 = benchmark::google_speed::OneStringTenInt64;
using MessageDecimal = benchmark::google_speed::OneStringTenDecimal;
using MessageWide = benchmark::google_speed::TenStringFiftyDecimal;

template <typename T>
inline void do_not_optimize(const T& value) {
#if defined(__GNUC__) || defined(__clang__)
  asm volatile("" : : "m"(value) : "memory");
#else
  (void)value;
#endif
}

std::string symbol(std::size_t index) { return "SYMBOL_" + std::to_string(index + 1); }
std::string decimal(std::size_t index) {
  return "12345." + std::string(8 + (index % 7), '0') + std::to_string(index + 17);
}

MessageInt64 make_int64() {
  MessageInt64 message;
  message.set_symbol("BTC-USD");
  message.set_value_01(1000001); message.set_value_02(1000002); message.set_value_03(1000003);
  message.set_value_04(1000004); message.set_value_05(1000005); message.set_value_06(1000006);
  message.set_value_07(1000007); message.set_value_08(1000008); message.set_value_09(1000009); message.set_value_10(1000010);
  return message;
}

MessageDecimal make_decimal() {
  MessageDecimal message;
  message.set_symbol("BTC-USD");
  message.set_decimal_01(decimal(0)); message.set_decimal_02(decimal(1)); message.set_decimal_03(decimal(2));
  message.set_decimal_04(decimal(3)); message.set_decimal_05(decimal(4)); message.set_decimal_06(decimal(5));
  message.set_decimal_07(decimal(6)); message.set_decimal_08(decimal(7)); message.set_decimal_09(decimal(8)); message.set_decimal_10(decimal(9));
  return message;
}

MessageWide make_wide() {
  MessageWide message;
  for (int i = 0; i < 10; ++i) {
    const auto value = symbol(static_cast<std::size_t>(i));
    switch (i) {
      case 0: message.set_symbol_01(value); break; case 1: message.set_symbol_02(value); break;
      case 2: message.set_symbol_03(value); break; case 3: message.set_symbol_04(value); break;
      case 4: message.set_symbol_05(value); break; case 5: message.set_symbol_06(value); break;
      case 6: message.set_symbol_07(value); break; case 7: message.set_symbol_08(value); break;
      case 8: message.set_symbol_09(value); break; case 9: message.set_symbol_10(value); break;
    }
  }
  for (int i = 0; i < 50; ++i) {
    const auto value = decimal(static_cast<std::size_t>(i));
    switch (i) {
      case 0: message.set_decimal_01(value); break; case 1: message.set_decimal_02(value); break;
      case 2: message.set_decimal_03(value); break; case 3: message.set_decimal_04(value); break;
      case 4: message.set_decimal_05(value); break; case 5: message.set_decimal_06(value); break;
      case 6: message.set_decimal_07(value); break; case 7: message.set_decimal_08(value); break;
      case 8: message.set_decimal_09(value); break; case 9: message.set_decimal_10(value); break;
      case 10: message.set_decimal_11(value); break; case 11: message.set_decimal_12(value); break;
      case 12: message.set_decimal_13(value); break; case 13: message.set_decimal_14(value); break;
      case 14: message.set_decimal_15(value); break; case 15: message.set_decimal_16(value); break;
      case 16: message.set_decimal_17(value); break; case 17: message.set_decimal_18(value); break;
      case 18: message.set_decimal_19(value); break; case 19: message.set_decimal_20(value); break;
      case 20: message.set_decimal_21(value); break; case 21: message.set_decimal_22(value); break;
      case 22: message.set_decimal_23(value); break; case 23: message.set_decimal_24(value); break;
      case 24: message.set_decimal_25(value); break; case 25: message.set_decimal_26(value); break;
      case 26: message.set_decimal_27(value); break; case 27: message.set_decimal_28(value); break;
      case 28: message.set_decimal_29(value); break; case 29: message.set_decimal_30(value); break;
      case 30: message.set_decimal_31(value); break; case 31: message.set_decimal_32(value); break;
      case 32: message.set_decimal_33(value); break; case 33: message.set_decimal_34(value); break;
      case 34: message.set_decimal_35(value); break; case 35: message.set_decimal_36(value); break;
      case 36: message.set_decimal_37(value); break; case 37: message.set_decimal_38(value); break;
      case 38: message.set_decimal_39(value); break; case 39: message.set_decimal_40(value); break;
      case 40: message.set_decimal_41(value); break; case 41: message.set_decimal_42(value); break;
      case 42: message.set_decimal_43(value); break; case 43: message.set_decimal_44(value); break;
      case 44: message.set_decimal_45(value); break; case 45: message.set_decimal_46(value); break;
      case 46: message.set_decimal_47(value); break; case 47: message.set_decimal_48(value); break;
      case 48: message.set_decimal_49(value); break; case 49: message.set_decimal_50(value); break;
    }
  }
  return message;
}

struct Config {
  std::uint64_t iterations = 1'000'000;
  std::uint32_t repetitions = 10;
  std::uint64_t warmup_iterations = 10'000;
  std::string csv_path = "/work/results/latency_raw.csv";
  std::string metadata_path = "/work/results/latency_metadata.txt";
};

struct Csv {
  explicit Csv(const std::string& path) : file(path) {
    if (!file) throw std::runtime_error("cannot open CSV output: " + path);
    file << "kind,path,test_case,repetition,iterations,bytes,p50_ns,p90_ns,p99_ns,p99_9_ns,p99_99_ns,max_ns,mean_ns,allocations,allocated_bytes,checksum\n";
  }
  std::ofstream file;
};

std::uint64_t percentile(std::vector<std::uint64_t>& values, double fraction) {
  const std::size_t index = std::min(values.size() - 1, static_cast<std::size_t>(values.size() * fraction));
  std::nth_element(values.begin(), values.begin() + index, values.end());
  return values[index];
}

template <typename Message, typename Encode>
void run_path(Csv& csv, const Config& config, const char* test_case, const char* path,
              std::size_t bytes, Encode&& encode) {
  std::vector<std::uint64_t> samples(config.iterations);
  for (std::uint64_t i = 0; i < config.warmup_iterations; ++i) do_not_optimize(encode());
  for (std::uint32_t repetition = 1; repetition <= config.repetitions; ++repetition) {
    allocation_count.store(0, std::memory_order_relaxed);
    allocated_bytes.store(0, std::memory_order_relaxed);
    std::uint64_t checksum = 0;
    for (std::uint64_t i = 0; i < config.iterations; ++i) {
      const auto start = now_ns();
      const auto result = encode();
      samples[i] = now_ns() - start;
      checksum += result;
    }
    const auto sum = [&] { std::uint64_t total = 0; for (const auto value : samples) total += value; return total; }();
    const auto p50 = percentile(samples, .50); const auto p90 = percentile(samples, .90);
    const auto p99 = percentile(samples, .99); const auto p99_9 = percentile(samples, .999);
    const auto p99_99 = percentile(samples, .9999); const auto maximum = *std::max_element(samples.begin(), samples.end());
    csv.file << "protobuf," << path << ',' << test_case << ',' << repetition << ',' << config.iterations << ',' << bytes << ','
             << p50 << ',' << p90 << ',' << p99 << ',' << p99_9 << ',' << p99_99 << ',' << maximum << ','
             << (sum / static_cast<double>(config.iterations)) << ','
             << allocation_count.load(std::memory_order_relaxed) << ','
             << allocated_bytes.load(std::memory_order_relaxed) << ',' << checksum << '\n';
  }
}

template <typename Message>
void run_message(Csv& csv, const Config& config, const char* test_case, Message message) {
  const std::size_t bytes = message.ByteSizeLong();
  std::vector<std::uint8_t> reusable(bytes);
  run_path<Message>(csv, config, test_case, "serialize_array_reuse", bytes, [&] {
    return message.SerializeToArray(reusable.data(), static_cast<int>(bytes)) ? bytes : 0;
  });
  run_path<Message>(csv, config, test_case, "serialize_array_fresh_buffer", bytes, [&] {
    std::vector<std::uint8_t> fresh(bytes);
    const auto result = message.SerializeToArray(fresh.data(), static_cast<int>(bytes)) ? bytes : 0;
    do_not_optimize(fresh[0]);
    return result;
  });
  std::string serialized;
  serialized.reserve(bytes);
  run_path<Message>(csv, config, test_case, "serialize_string_reserved", bytes, [&] {
    return message.SerializeToString(&serialized) ? serialized.size() : 0;
  });
  google::protobuf::Arena arena;
  auto* arena_message = google::protobuf::Arena::Create<Message>(&arena);
  *arena_message = message;
  std::vector<std::uint8_t> arena_buffer(bytes);
  run_path<Message>(csv, config, test_case, "serialize_array_arena_message", bytes, [&] {
    return arena_message->SerializeToArray(arena_buffer.data(), static_cast<int>(bytes)) ? bytes : 0;
  });
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
             << "warmup_iterations=" << config.warmup_iterations << '\n' << "per_encode_timing=true\n";
    Csv csv(config.csv_path);
    run_message(csv, config, "one_string_ten_int64", make_int64());
    run_message(csv, config, "one_string_ten_decimal_strings", make_decimal());
    run_message(csv, config, "ten_strings_fifty_decimal_strings", make_wide());
    std::cerr << "Latency benchmark complete: " << config.csv_path << '\n';
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "ERROR: " << error.what() << '\n';
    return 1;
  }
}

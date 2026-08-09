#include "generated/google_speed/google_speed/market.pb.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <charconv>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <latch>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;
using IntMessage = benchmark::google_speed::OneStringTenInt64;
using DecimalMessage = benchmark::google_speed::OneStringTenDecimal;
using WideMessage = benchmark::google_speed::TenStringFiftyDecimal;

constexpr std::size_t kTextCapacity = 32;
constexpr std::size_t kPoolSize = 1024;
constexpr std::size_t kQueueCapacity = 1U << 20;
constexpr std::size_t kOutputCapacity = 8192;
constexpr std::size_t kLogCapacity = 4096;

struct IntEvent { char symbol[kTextCapacity]; std::int64_t values[10]; };
struct DecimalEvent { char symbol[kTextCapacity]; char decimals[10][kTextCapacity]; };
struct WideEvent { char fields[60][kTextCapacity]; };

template <typename T>
inline void do_not_optimize(const T& value) {
#if defined(__GNUC__) || defined(__clang__)
  asm volatile("" : : "m"(value) : "memory");
#else
  (void)value;
#endif
}

std::uint64_t now_ns() {
  return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
      Clock::now().time_since_epoch()).count());
}

void copy_text(char (&destination)[kTextCapacity], std::string_view value) {
  std::memset(destination, 0, kTextCapacity);
  const auto length = value.size() < kTextCapacity - 1 ? value.size() : kTextCapacity - 1;
  std::memcpy(destination, value.data(), length);
}

std::string decimal(std::size_t index) {
  return "12345." + std::string(8 + (index % 7), '0') + std::to_string(index + 17);
}

IntEvent make_int_event() {
  IntEvent event{};
  copy_text(event.symbol, "BTC-USD");
  for (int i = 0; i < 10; ++i) event.values[i] = 1'000'001 + i;
  return event;
}

DecimalEvent make_decimal_event() {
  DecimalEvent event{};
  copy_text(event.symbol, "BTC-USD");
  for (int i = 0; i < 10; ++i) copy_text(event.decimals[i], decimal(static_cast<std::size_t>(i)));
  return event;
}

WideEvent make_wide_event() {
  WideEvent event{};
  for (int i = 0; i < 10; ++i) copy_text(event.fields[i], "SYMBOL_" + std::to_string(i + 1));
  for (int i = 0; i < 50; ++i) copy_text(event.fields[10 + i], decimal(static_cast<std::size_t>(i)));
  return event;
}

template <typename Event>
struct Traits;

template <>
struct Traits<IntEvent> {
  using Message = IntMessage;
  static constexpr std::string_view name = "one_string_ten_int64";
  static IntEvent make() { return make_int_event(); }
  static void populate(const IntEvent& event, Message& message) {
    message.set_symbol(event.symbol);
    message.set_value_01(event.values[0]); message.set_value_02(event.values[1]);
    message.set_value_03(event.values[2]); message.set_value_04(event.values[3]);
    message.set_value_05(event.values[4]); message.set_value_06(event.values[5]);
    message.set_value_07(event.values[6]); message.set_value_08(event.values[7]);
    message.set_value_09(event.values[8]); message.set_value_10(event.values[9]);
  }
  static std::size_t format(const IntEvent& event, std::array<char, kLogCapacity>& output) {
    std::size_t position = 0;
    append(output, position, event.symbol);
    for (const auto value : event.values) append(output, position, value);
    output[position++] = '\n';
    return position;
  }
 private:
  static void append(std::array<char, kLogCapacity>& output, std::size_t& position, const char* value) {
    if (position != 0) output[position++] = '|';
    const auto length = std::strlen(value);
    std::memcpy(output.data() + position, value, length);
    position += length;
  }
  static void append(std::array<char, kLogCapacity>& output, std::size_t& position, std::int64_t value) {
    if (position != 0) output[position++] = '|';
    char text[32];
    const auto conversion = std::to_chars(text, text + sizeof(text), value);
    if (conversion.ec != std::errc{}) throw std::runtime_error("integer formatting failed");
    const auto length = static_cast<std::size_t>(conversion.ptr - text);
    std::memcpy(output.data() + position, text, length);
    position += length;
  }
};

template <>
struct Traits<DecimalEvent> {
  using Message = DecimalMessage;
  static constexpr std::string_view name = "one_string_ten_decimal_strings";
  static DecimalEvent make() { return make_decimal_event(); }
  static void populate(const DecimalEvent& event, Message& message) {
    message.set_symbol(event.symbol);
    message.set_decimal_01(event.decimals[0]); message.set_decimal_02(event.decimals[1]);
    message.set_decimal_03(event.decimals[2]); message.set_decimal_04(event.decimals[3]);
    message.set_decimal_05(event.decimals[4]); message.set_decimal_06(event.decimals[5]);
    message.set_decimal_07(event.decimals[6]); message.set_decimal_08(event.decimals[7]);
    message.set_decimal_09(event.decimals[8]); message.set_decimal_10(event.decimals[9]);
  }
  static std::size_t format(const DecimalEvent& event, std::array<char, kLogCapacity>& output) {
    std::size_t position = 0;
    append(output, position, event.symbol);
    for (const auto& value : event.decimals) append(output, position, value);
    output[position++] = '\n';
    return position;
  }
 private:
  static void append(std::array<char, kLogCapacity>& output, std::size_t& position, const char* value) {
    if (position != 0) output[position++] = '|';
    const auto length = std::strlen(value);
    std::memcpy(output.data() + position, value, length);
    position += length;
  }
};

template <>
struct Traits<WideEvent> {
  using Message = WideMessage;
  static constexpr std::string_view name = "ten_strings_fifty_decimal_strings";
  static WideEvent make() { return make_wide_event(); }
  static void populate(const WideEvent& event, Message& message) {
    message.set_symbol_01(event.fields[0]); message.set_symbol_02(event.fields[1]);
    message.set_symbol_03(event.fields[2]); message.set_symbol_04(event.fields[3]);
    message.set_symbol_05(event.fields[4]); message.set_symbol_06(event.fields[5]);
    message.set_symbol_07(event.fields[6]); message.set_symbol_08(event.fields[7]);
    message.set_symbol_09(event.fields[8]); message.set_symbol_10(event.fields[9]);
    message.set_decimal_01(event.fields[10]); message.set_decimal_02(event.fields[11]);
    message.set_decimal_03(event.fields[12]); message.set_decimal_04(event.fields[13]);
    message.set_decimal_05(event.fields[14]); message.set_decimal_06(event.fields[15]);
    message.set_decimal_07(event.fields[16]); message.set_decimal_08(event.fields[17]);
    message.set_decimal_09(event.fields[18]); message.set_decimal_10(event.fields[19]);
    message.set_decimal_11(event.fields[20]); message.set_decimal_12(event.fields[21]);
    message.set_decimal_13(event.fields[22]); message.set_decimal_14(event.fields[23]);
    message.set_decimal_15(event.fields[24]); message.set_decimal_16(event.fields[25]);
    message.set_decimal_17(event.fields[26]); message.set_decimal_18(event.fields[27]);
    message.set_decimal_19(event.fields[28]); message.set_decimal_20(event.fields[29]);
    message.set_decimal_21(event.fields[30]); message.set_decimal_22(event.fields[31]);
    message.set_decimal_23(event.fields[32]); message.set_decimal_24(event.fields[33]);
    message.set_decimal_25(event.fields[34]); message.set_decimal_26(event.fields[35]);
    message.set_decimal_27(event.fields[36]); message.set_decimal_28(event.fields[37]);
    message.set_decimal_29(event.fields[38]); message.set_decimal_30(event.fields[39]);
    message.set_decimal_31(event.fields[40]); message.set_decimal_32(event.fields[41]);
    message.set_decimal_33(event.fields[42]); message.set_decimal_34(event.fields[43]);
    message.set_decimal_35(event.fields[44]); message.set_decimal_36(event.fields[45]);
    message.set_decimal_37(event.fields[46]); message.set_decimal_38(event.fields[47]);
    message.set_decimal_39(event.fields[48]); message.set_decimal_40(event.fields[49]);
    message.set_decimal_41(event.fields[50]); message.set_decimal_42(event.fields[51]);
    message.set_decimal_43(event.fields[52]); message.set_decimal_44(event.fields[53]);
    message.set_decimal_45(event.fields[54]); message.set_decimal_46(event.fields[55]);
    message.set_decimal_47(event.fields[56]); message.set_decimal_48(event.fields[57]);
    message.set_decimal_49(event.fields[58]); message.set_decimal_50(event.fields[59]);
  }
  static std::size_t format(const WideEvent& event, std::array<char, kLogCapacity>& output) {
    std::size_t position = 0;
    for (const auto& value : event.fields) {
      if (position != 0) output[position++] = '|';
      const auto length = std::strlen(value);
      std::memcpy(output.data() + position, value, length);
      position += length;
    }
    output[position++] = '\n';
    return position;
  }
};

template <std::size_t Capacity>
class SpscPointerRing {
 public:
  SpscPointerRing() : entries_(Capacity) {}
  bool try_push(const void* value) {
    const auto head = head_.load(std::memory_order_relaxed);
    const auto tail = tail_.load(std::memory_order_acquire);
    if (head - tail == Capacity) return false;
    entries_[head & (Capacity - 1)] = value;
    head_.store(head + 1, std::memory_order_release);
    return true;
  }
  const void* pop() {
    const auto tail = tail_.load(std::memory_order_relaxed);
    const auto head = head_.load(std::memory_order_acquire);
    if (tail == head) return nullptr;
    const void* value = entries_[tail & (Capacity - 1)];
    tail_.store(tail + 1, std::memory_order_release);
    return value;
  }
 private:
  std::vector<const void*> entries_;
  alignas(64) std::atomic<std::size_t> head_{0};
  alignas(64) std::atomic<std::size_t> tail_{0};
};

struct Config {
  std::uint64_t iterations = 1'000'000;
  std::uint32_t repetitions = 10;
  std::uint64_t warmup_iterations = 10'000;
  std::string modes = "sync_both,async_handoff,async_protobuf,async_pipe,async_both";
  std::string csv_path = "/work/results/async_pipeline_raw.csv";
  std::string metadata_path = "/work/results/async_pipeline_metadata.txt";
};

struct Result {
  std::uint64_t completed = 0;
  std::uint64_t dropped = 0;
  std::uint64_t hot_elapsed_ns = 0;
  std::uint64_t worker_wall_ns = 0;
  std::uint64_t checksum = 0;
  std::vector<std::uint64_t> hot_samples;
  std::vector<std::uint64_t> worker_samples;
};

struct Csv {
  explicit Csv(const std::string& path) : file(path) {
    if (!file) throw std::runtime_error("cannot open CSV output: " + path);
    file << "mode,test_case,repetition,iterations,completed,dropped,payload_bytes,hot_elapsed_ns,worker_wall_ns,"
            "hot_mean_ns,hot_p50_ns,hot_p99_ns,worker_mean_ns,worker_p50_ns,worker_p99_ns,checksum\n";
  }
  std::ofstream file;
};

enum class Mode { SyncBoth, AsyncHandoff, AsyncProtobuf, AsyncPipe, AsyncBoth };

Mode parse_mode(const std::string& mode) {
  if (mode == "sync_both") return Mode::SyncBoth;
  if (mode == "async_handoff") return Mode::AsyncHandoff;
  if (mode == "async_protobuf") return Mode::AsyncProtobuf;
  if (mode == "async_pipe") return Mode::AsyncPipe;
  if (mode == "async_both") return Mode::AsyncBoth;
  throw std::runtime_error("invalid async pipeline mode: " + mode);
}

bool does_proto(Mode mode) { return mode == Mode::SyncBoth || mode == Mode::AsyncProtobuf || mode == Mode::AsyncBoth; }
bool does_pipe(Mode mode) { return mode == Mode::SyncBoth || mode == Mode::AsyncPipe || mode == Mode::AsyncBoth; }

template <typename Event>
std::uint64_t process_event(const Event& event, Mode mode,
                            typename Traits<Event>::Message& message,
                            std::array<std::uint8_t, kOutputCapacity>& output,
                            std::array<char, kLogCapacity>& line,
                            std::size_t payload_bytes) {
  std::uint64_t checksum = 0;
  if (does_proto(mode)) {
    Traits<Event>::populate(event, message);
    output[0] = 0; output[1] = 0; output[2] = 0; output[3] = 0; output[4] = 1; output[5] = 0;
    if (!message.SerializeToArray(output.data() + 6, static_cast<int>(payload_bytes))) return 0;
    checksum += payload_bytes + 6;
    do_not_optimize(output[6]);
  }
  if (does_pipe(mode)) {
    const auto line_bytes = Traits<Event>::format(event, line);
    checksum += line_bytes;
    do_not_optimize(line[0]);
  }
  return checksum;
}

template <typename Event>
std::size_t payload_size(const Event& event) {
  typename Traits<Event>::Message message;
  Traits<Event>::populate(event, message);
  return message.ByteSizeLong();
}

std::uint64_t percentile(std::vector<std::uint64_t> values, double fraction) {
  if (values.empty()) return 0;
  const auto index = std::min(values.size() - 1, static_cast<std::size_t>(values.size() * fraction));
  std::nth_element(values.begin(), values.begin() + index, values.end());
  return values[index];
}

double mean(const std::vector<std::uint64_t>& values) {
  if (values.empty()) return 0;
  long double total = 0;
  for (const auto value : values) total += value;
  return static_cast<double>(total / values.size());
}

template <typename Event>
Result run_sync(const Config& config, const Event& base, std::size_t payload_bytes) {
  typename Traits<Event>::Message message;
  Traits<Event>::populate(base, message);
  std::array<std::uint8_t, kOutputCapacity> output{};
  std::array<char, kLogCapacity> line{};
  Result result;
  result.hot_samples.reserve(config.iterations);
  for (std::uint64_t i = 0; i < config.warmup_iterations; ++i) {
    process_event(base, Mode::SyncBoth, message, output, line, payload_bytes);
  }
  const auto start = now_ns();
  for (std::uint64_t i = 0; i < config.iterations; ++i) {
    const auto item_start = now_ns();
    result.checksum += process_event(base, Mode::SyncBoth, message, output, line, payload_bytes);
    result.hot_samples.push_back(now_ns() - item_start);
  }
  result.hot_elapsed_ns = now_ns() - start;
  result.worker_samples = result.hot_samples;
  result.worker_wall_ns = result.hot_elapsed_ns;
  result.completed = config.iterations;
  return result;
}

template <typename Event>
Result run_async(const Config& config, Mode mode, const Event& base, std::size_t payload_bytes) {
  std::array<Event, kPoolSize> pool{};
  for (auto& event : pool) event = base;
  SpscPointerRing<kQueueCapacity> ring;
  std::latch ready(2);
  std::atomic<bool> start{false};
  std::atomic<bool> producer_done{false};
  Result result;
  result.hot_samples.reserve(config.iterations);
  result.worker_samples.reserve(config.iterations);
  std::uint64_t worker_wall_ns = 0;
  std::uint64_t worker_checksum = 0;
  std::uint64_t worker_completed = 0;

  std::thread worker([&] {
    typename Traits<Event>::Message message;
    Traits<Event>::populate(base, message);
    std::array<std::uint8_t, kOutputCapacity> output{};
    std::array<char, kLogCapacity> line{};
    ready.count_down();
    while (!start.load(std::memory_order_acquire)) std::this_thread::yield();
    const auto worker_start = now_ns();
    for (;;) {
      const auto value = ring.pop();
      if (value == nullptr) {
        if (producer_done.load(std::memory_order_acquire)) break;
        continue;
      }
      const auto* event = static_cast<const Event*>(value);
      const auto item_start = now_ns();
      if (mode == Mode::AsyncHandoff) {
        do_not_optimize(*event);
        result.worker_samples.push_back(now_ns() - item_start);
      } else {
        worker_checksum += process_event(*event, mode, message, output, line, payload_bytes);
        result.worker_samples.push_back(now_ns() - item_start);
      }
      ++worker_completed;
    }
    worker_wall_ns = now_ns() - worker_start;
  });

  std::thread producer([&] {
    ready.count_down();
    while (!start.load(std::memory_order_acquire)) std::this_thread::yield();
    const auto producer_start = now_ns();
    for (std::uint64_t i = 0; i < config.iterations; ++i) {
      const auto item_start = now_ns();
      const auto* event = &pool[i & (kPoolSize - 1)];
      if (!ring.try_push(event)) {
        ++result.dropped;
      } else {
        result.hot_samples.push_back(now_ns() - item_start);
      }
    }
    result.hot_elapsed_ns = now_ns() - producer_start;
    producer_done.store(true, std::memory_order_release);
  });

  ready.wait();
  start.store(true, std::memory_order_release);
  producer.join();
  worker.join();
  result.completed = worker_completed;
  result.worker_wall_ns = worker_wall_ns;
  result.checksum = worker_checksum;
  return result;
}

template <typename Event>
void run_case(const Config& config, Csv& csv, Mode mode, const std::string& mode_name,
              std::uint32_t repetition, const Event& event) {
  const auto bytes = payload_size(event);
  auto result = mode == Mode::SyncBoth
      ? run_sync(config, event, bytes)
      : run_async(config, mode, event, bytes);
  csv.file << mode_name << ',' << Traits<Event>::name << ',' << repetition << ',' << config.iterations << ','
           << result.completed << ',' << result.dropped << ',' << (bytes + 6) << ',' << result.hot_elapsed_ns << ','
           << result.worker_wall_ns << ',' << std::fixed << std::setprecision(3) << mean(result.hot_samples) << ','
           << percentile(result.hot_samples, .50) << ',' << percentile(result.hot_samples, .99) << ','
           << mean(result.worker_samples) << ',' << percentile(result.worker_samples, .50) << ','
           << percentile(result.worker_samples, .99) << ',' << result.checksum << '\n';
}

Config parse_args(int argc, char** argv) {
  Config config;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    auto next = [&] { if (i + 1 >= argc) throw std::runtime_error("missing value for " + arg); return std::string(argv[++i]); };
    if (arg == "--iterations") config.iterations = std::stoull(next());
    else if (arg == "--repetitions") config.repetitions = static_cast<std::uint32_t>(std::stoul(next()));
    else if (arg == "--warmup-iterations") config.warmup_iterations = std::stoull(next());
    else if (arg == "--modes") config.modes = next();
    else if (arg == "--csv") config.csv_path = next();
    else if (arg == "--metadata") config.metadata_path = next();
    else throw std::runtime_error("unknown argument: " + arg);
  }
  if (config.iterations < 1'000'000 || config.repetitions < 10) {
    throw std::runtime_error("iterations must be >= 1000000 and repetitions must be >= 10");
  }
  if (config.iterations > kQueueCapacity) throw std::runtime_error("iterations exceed preallocated queue capacity");
  return config;
}

std::vector<std::string> split(const std::string& value) {
  std::vector<std::string> result;
  std::size_t start = 0;
  while (start <= value.size()) {
    const auto comma = value.find(',', start);
    result.push_back(value.substr(start, comma == std::string::npos ? std::string::npos : comma - start));
    if (comma == std::string::npos) break;
    start = comma + 1;
  }
  return result;
}

}  // namespace

int main(int argc, char** argv) {
  try {
    const Config config = parse_args(argc, argv);
    std::ofstream metadata(config.metadata_path);
    metadata << "iterations=" << config.iterations << '\n'
             << "repetitions=" << config.repetitions << '\n'
             << "warmup_iterations=" << config.warmup_iterations << '\n'
             << "queue_capacity=" << kQueueCapacity << '\n'
             << "handoff_model=preallocated_immutable_event_pool_pointer\n"
             << "modes=" << config.modes << '\n';
    Csv csv(config.csv_path);
    for (const auto& mode_name : split(config.modes)) {
      const auto mode = parse_mode(mode_name);
      for (std::uint32_t repetition = 1; repetition <= config.repetitions; ++repetition) {
        run_case(config, csv, mode, mode_name, repetition, Traits<IntEvent>::make());
        run_case(config, csv, mode, mode_name, repetition, Traits<DecimalEvent>::make());
        run_case(config, csv, mode, mode_name, repetition, Traits<WideEvent>::make());
      }
      std::cerr << "Completed mode " << mode_name << '\n';
    }
    std::cerr << "Async pipeline benchmark complete: " << config.csv_path << '\n';
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "ERROR: " << error.what() << '\n';
    return 1;
  }
}

#include "generated/google_speed/google_speed/market.pb.h"

#include <barrier>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
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

Message make_message() {
  Message message;
  message.set_symbol("BTC-USD");
  message.set_value_01(1000001); message.set_value_02(1000002); message.set_value_03(1000003);
  message.set_value_04(1000004); message.set_value_05(1000005); message.set_value_06(1000006);
  message.set_value_07(1000007); message.set_value_08(1000008); message.set_value_09(1000009); message.set_value_10(1000010);
  return message;
}

struct Config {
  std::uint64_t iterations = 1'000'000;
  std::uint32_t repetitions = 10;
  std::uint64_t warmup_iterations = 10'000;
  std::uint32_t threads = 1;
  std::string mode = "thread_local";
  std::string csv_path = "/work/results/concurrency_raw.csv";
  std::string metadata_path = "/work/results/concurrency_metadata.txt";
};

struct Csv {
  explicit Csv(const std::string& path) : file(path) {
    if (!file) throw std::runtime_error("cannot open CSV output: " + path);
    file << "mode,threads,repetition,iterations,bytes,elapsed_ns,ns_per_encode,encodes_per_second,checksum\n";
  }
  std::ofstream file;
};

void run(const Config& config, Csv& csv) {
  if (config.threads == 0 || config.iterations % config.threads != 0) throw std::runtime_error("iterations must be divisible by threads");
  if (config.mode != "thread_local" && config.mode != "shared_readonly" && config.mode != "shared_buffer_mutex") throw std::runtime_error("invalid concurrency mode");
  const auto message = make_message();
  const auto bytes = message.ByteSizeLong();
  std::vector<Message> messages(config.threads, message);
  std::vector<std::vector<std::uint8_t>> buffers;
  buffers.reserve(config.threads);
  for (std::uint32_t i = 0; i < config.threads; ++i) buffers.emplace_back(bytes);
    std::vector<std::uint8_t> shared_buffer(bytes);
    std::mutex shared_mutex;

  for (std::uint64_t i = 0; i < config.warmup_iterations; ++i) message.SerializeToArray(buffers[0].data(), static_cast<int>(bytes));
  for (std::uint32_t repetition = 1; repetition <= config.repetitions; ++repetition) {
    std::uint64_t checksum = 0;
    std::mutex checksum_mutex;
    Clock::time_point start_time;
    std::barrier start_barrier(static_cast<std::ptrdiff_t>(config.threads + 1), [&] { start_time = Clock::now(); });
    std::vector<std::thread> workers;
    workers.reserve(config.threads);
    for (std::uint32_t thread_index = 0; thread_index < config.threads; ++thread_index) {
      workers.emplace_back([&, thread_index] {
        start_barrier.arrive_and_wait();
        std::uint64_t local_checksum = 0;
        for (std::uint64_t i = 0; i < config.iterations / config.threads; ++i) {
          bool ok = false;
          if (config.mode == "shared_buffer_mutex") {
            std::lock_guard lock(shared_mutex);
            ok = message.SerializeToArray(shared_buffer.data(), static_cast<int>(bytes));
          } else {
            const auto& source = config.mode == "shared_readonly" ? message : messages[thread_index];
            ok = source.SerializeToArray(buffers[thread_index].data(), static_cast<int>(bytes));
          }
          local_checksum += ok ? bytes : 0;
        }
        std::lock_guard lock(checksum_mutex);
        checksum += local_checksum;
      });
    }
    start_barrier.arrive_and_wait();
    for (auto& worker : workers) worker.join();
    const auto elapsed = std::chrono::duration_cast<Nanoseconds>(Clock::now() - start_time).count();
    do_not_optimize(checksum);
    const double ns_per_encode = elapsed / static_cast<double>(config.iterations);
    csv.file << config.mode << ',' << config.threads << ',' << repetition << ',' << config.iterations << ',' << bytes << ','
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
    else if (arg == "--threads") config.threads = static_cast<std::uint32_t>(std::stoul(next()));
    else if (arg == "--mode") config.mode = next();
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
             << "warmup_iterations=" << config.warmup_iterations << '\n' << "threads=" << config.threads << '\n' << "mode=" << config.mode << '\n';
    Csv csv(config.csv_path);
    run(config, csv);
    std::cerr << "Concurrency benchmark complete: " << config.csv_path << '\n';
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "ERROR: " << error.what() << '\n';
    return 1;
  }
}

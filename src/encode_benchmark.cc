#include "generated/google_codesize/google_codesize/market.pb.h"
#include "generated/google_lite/google_lite/market.pb.h"
#include "generated/google_speed/google_speed/market.pb.h"
#include "generated/protobuf_c/protobuf_c/market.pb-c.h"

#include <boost/json.hpp>
#include <json/json.h>
#include <nlohmann/json.hpp>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <yyjson.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>
#include <stdexcept>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;
using Nanoseconds = std::chrono::duration<double, std::nano>;

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

std::string numbered(std::string_view prefix, int index) {
  return std::string(prefix) + (index < 10 ? "0" : "") + std::to_string(index);
}

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
  std::string csv_path = "/work/results/raw.csv";
  std::string metadata_path = "/work/results/metadata.txt";
};

struct Csv {
  explicit Csv(const std::string& path) : file(path) {
    if (!file) throw std::runtime_error("cannot open CSV output: " + path);
    file << "kind,library,codegen,api,test_case,repetition,iterations,bytes,elapsed_ns,ns_per_encode,encodes_per_second,checksum\n";
  }
  std::ofstream file;
};

template <typename Encode>
void run_measure(Csv& csv, const std::string& kind, const std::string& library,
                 const std::string& codegen, const std::string& api, Case test_case,
                 const Config& config, std::size_t bytes_hint, Encode&& encode) {
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
    const double encodes_per_second = 1'000'000'000.0 / ns_per_encode;
    csv.file << kind << ',' << library << ',' << codegen << ',' << api << ',' << case_name(test_case)
             << ',' << repetition << ',' << config.iterations << ',' << bytes << ','
             << std::fixed << std::setprecision(3) << elapsed << ',' << ns_per_encode << ','
             << encodes_per_second << ',' << checksum << '\n';
  }
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

template <typename Message>
Message make_int64_message() {
  Message message;
  message.set_symbol("BTC-USD");
  SET_INT64_FIELDS(message);
  return message;
}

template <typename Message>
Message make_decimal_message() {
  Message message;
  message.set_symbol("BTC-USD");
  SET_DECIMAL_FIELDS(message);
  return message;
}

template <typename Message>
Message make_wide_message() {
  Message message;
  SET_WIDE_SYMBOL_FIELDS(message);
  SET_WIDE_DECIMAL_FIELDS(message);
  return message;
}

template <typename Message>
void run_google_variant(Csv& csv, const Config& config, const std::string& codegen, Case test_case, Message message) {
  const std::size_t bytes = message.ByteSizeLong();
  const std::string library = "google_protobuf";

  std::string output;
  output.reserve(bytes);
  run_measure(csv, "protobuf", library, codegen, "SerializeToString", test_case, config, bytes,
              [&] { return message.SerializeToString(&output) ? output.size() : 0; });

  std::vector<std::uint8_t> buffer(bytes);
  run_measure(csv, "protobuf", library, codegen, "SerializeToArray_preallocated", test_case, config, bytes,
              [&] { return message.SerializeToArray(buffer.data(), static_cast<int>(buffer.size())) ? bytes : 0; });
}

void rapidjson_write_wide(rapidjson::Writer<rapidjson::StringBuffer>& writer) {
  for (int i = 1; i <= 10; ++i) {
    const std::string key = numbered("symbol_", i);
    writer.Key(key.c_str());
    writer.String(symbol(static_cast<std::size_t>(i - 1)).c_str());
  }
  for (int i = 1; i <= 50; ++i) {
    const std::string key = numbered("decimal_", i);
    writer.Key(key.c_str());
    writer.String(decimal(static_cast<std::size_t>(i - 1)).c_str());
  }
}

template <typename WriterFn>
void run_json_writer(Csv& csv, const Config& config, const std::string& library, Case test_case, WriterFn&& write) {
  std::string output;
  output.reserve(test_case == Case::Wide ? 4500 : 500);
  run_measure(csv, "json", library, "n/a", "writer", test_case, config, 0,
              [&] { output.clear(); write(output, test_case); return output.size(); });
}

void run_rapidjson(Csv& csv, const Config& config, Case test_case) {
  run_json_writer(csv, config, "rapidjson", test_case, [](std::string& output, Case c) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();
    if (c == Case::Wide) {
      rapidjson_write_wide(writer);
    } else {
      writer.Key("symbol"); writer.String("BTC-USD");
      if (c == Case::Int64) {
        for (int i = 1; i <= 10; ++i) {
          const std::string key = numbered("value_", i);
          writer.Key(key.c_str()); writer.Int64(1'000'000 + i);
        }
      } else {
        for (int i = 1; i <= 10; ++i) {
          const std::string key = numbered("decimal_", i);
          writer.Key(key.c_str()); writer.String(decimal(static_cast<std::size_t>(i - 1)).c_str());
        }
      }
    }
    writer.EndObject();
    output.assign(buffer.GetString(), buffer.GetSize());
  });
}

nlohmann::json make_nlohmann(Case test_case) {
  nlohmann::json object;
  if (test_case == Case::Int64) {
    object["symbol"] = "BTC-USD";
    for (int i = 1; i <= 10; ++i) object[numbered("value_", i)] = 1'000'000 + i;
  } else if (test_case == Case::Decimal) {
    object["symbol"] = "BTC-USD";
    for (int i = 1; i <= 10; ++i) object[numbered("decimal_", i)] = decimal(static_cast<std::size_t>(i - 1));
  } else {
    for (int i = 1; i <= 10; ++i) object[numbered("symbol_", i)] = symbol(static_cast<std::size_t>(i - 1));
    for (int i = 1; i <= 50; ++i) object[numbered("decimal_", i)] = decimal(static_cast<std::size_t>(i - 1));
  }
  return object;
}

void run_nlohmann(Csv& csv, const Config& config, Case test_case) {
  const auto object = make_nlohmann(test_case);
  run_measure(csv, "json", "nlohmann_json", "n/a", "dump", test_case, config, 0,
              [&] { const auto output = object.dump(); do_not_optimize(output); return output.size(); });
}

Json::Value make_jsoncpp(Case test_case) {
  Json::Value object(Json::objectValue);
  if (test_case == Case::Int64) {
    object["symbol"] = "BTC-USD";
    for (int i = 1; i <= 10; ++i) object[numbered("value_", i)] = Json::Int64(1'000'000 + i);
  } else if (test_case == Case::Decimal) {
    object["symbol"] = "BTC-USD";
    for (int i = 1; i <= 10; ++i) object[numbered("decimal_", i)] = decimal(static_cast<std::size_t>(i - 1));
  } else {
    for (int i = 1; i <= 10; ++i) object[numbered("symbol_", i)] = symbol(static_cast<std::size_t>(i - 1));
    for (int i = 1; i <= 50; ++i) object[numbered("decimal_", i)] = decimal(static_cast<std::size_t>(i - 1));
  }
  return object;
}

void run_jsoncpp(Csv& csv, const Config& config, Case test_case) {
  const auto object = make_jsoncpp(test_case);
  Json::StreamWriterBuilder builder;
  builder["indentation"] = "";
  builder["commentStyle"] = "None";
  run_measure(csv, "json", "jsoncpp", "n/a", "writeString", test_case, config, 0,
              [&] { const auto output = Json::writeString(builder, object); do_not_optimize(output); return output.size(); });
}

boost::json::object make_boost_json(Case test_case) {
  boost::json::object object;
  if (test_case == Case::Int64) {
    object["symbol"] = "BTC-USD";
    for (int i = 1; i <= 10; ++i) object[numbered("value_", i)] = 1'000'000 + i;
  } else if (test_case == Case::Decimal) {
    object["symbol"] = "BTC-USD";
    for (int i = 1; i <= 10; ++i) object[numbered("decimal_", i)] = decimal(static_cast<std::size_t>(i - 1));
  } else {
    for (int i = 1; i <= 10; ++i) object[numbered("symbol_", i)] = symbol(static_cast<std::size_t>(i - 1));
    for (int i = 1; i <= 50; ++i) object[numbered("decimal_", i)] = decimal(static_cast<std::size_t>(i - 1));
  }
  return object;
}

void run_boost_json(Csv& csv, const Config& config, Case test_case) {
  const auto object = make_boost_json(test_case);
  run_measure(csv, "json", "boost_json", "n/a", "serialize", test_case, config, 0,
              [&] { const auto output = boost::json::serialize(object); do_not_optimize(output); return output.size(); });
}

struct YyjsonData {
  yyjson_mut_doc* doc = nullptr;
  yyjson_mut_val* root = nullptr;
};

YyjsonData make_yyjson(Case test_case) {
  YyjsonData data{yyjson_mut_doc_new(nullptr), nullptr};
  data.root = yyjson_mut_obj(data.doc);
  yyjson_mut_doc_set_root(data.doc, data.root);
  auto add_string = [&](const std::string& key, const std::string& value) {
    yyjson_mut_obj_add_strcpy(data.doc, data.root, key.c_str(), value.c_str());
  };
  auto add_int = [&](const std::string& key, std::int64_t value) {
    yyjson_mut_obj_add_sint(data.doc, data.root, key.c_str(), value);
  };
  if (test_case == Case::Int64) {
    add_string("symbol", "BTC-USD");
    for (int i = 1; i <= 10; ++i) add_int(numbered("value_", i), 1'000'000 + i);
  } else if (test_case == Case::Decimal) {
    add_string("symbol", "BTC-USD");
    for (int i = 1; i <= 10; ++i) add_string(numbered("decimal_", i), decimal(static_cast<std::size_t>(i - 1)));
  } else {
    for (int i = 1; i <= 10; ++i) add_string(numbered("symbol_", i), symbol(static_cast<std::size_t>(i - 1)));
    for (int i = 1; i <= 50; ++i) add_string(numbered("decimal_", i), decimal(static_cast<std::size_t>(i - 1)));
  }
  return data;
}

void run_yyjson(Csv& csv, const Config& config, Case test_case) {
  const auto data = make_yyjson(test_case);
  run_measure(csv, "json", "yyjson", "n/a", "mut_write", test_case, config, 0,
              [&] {
                std::size_t size = 0;
                char* encoded = yyjson_mut_write(data.doc, 0, &size);
                if (encoded != nullptr && size != 0) do_not_optimize(encoded[0]);
                std::free(encoded);
                return size;
              });
  yyjson_mut_doc_free(data.doc);
}

void run_protobuf_c(Csv& csv, const Config& config, Case test_case) {
  // protobuf-c is included as a C-generated comparison. The generated structs are
  // initialized and packed into a caller-owned buffer; no unpack/decode path is used.
  if (test_case == Case::Int64) {
    Benchmark__ProtobufC__OneStringTenInt64 message = BENCHMARK__PROTOBUF_C__ONE_STRING_TEN_INT64__INIT;
    message.symbol = const_cast<char*>("BTC-USD");
    message.value_01 = 1000001; message.value_02 = 1000002; message.value_03 = 1000003; message.value_04 = 1000004;
    message.value_05 = 1000005; message.value_06 = 1000006; message.value_07 = 1000007; message.value_08 = 1000008;
    message.value_09 = 1000009; message.value_10 = 1000010;
    const auto bytes = benchmark__protobuf_c__one_string_ten_int64__get_packed_size(&message);
    std::vector<std::uint8_t> buffer(bytes);
    run_measure(csv, "protobuf", "protobuf_c", "c_generated", "pack_preallocated", test_case, config, bytes,
                [&] { return benchmark__protobuf_c__one_string_ten_int64__pack(&message, buffer.data()); });
  } else if (test_case == Case::Decimal) {
    Benchmark__ProtobufC__OneStringTenDecimal message = BENCHMARK__PROTOBUF_C__ONE_STRING_TEN_DECIMAL__INIT;
    message.symbol = const_cast<char*>("BTC-USD");
    const std::array<std::string, 10> decimals = {decimal(0), decimal(1), decimal(2), decimal(3), decimal(4),
                                                  decimal(5), decimal(6), decimal(7), decimal(8), decimal(9)};
    message.decimal_01 = const_cast<char*>(decimals[0].c_str()); message.decimal_02 = const_cast<char*>(decimals[1].c_str());
    message.decimal_03 = const_cast<char*>(decimals[2].c_str()); message.decimal_04 = const_cast<char*>(decimals[3].c_str());
    message.decimal_05 = const_cast<char*>(decimals[4].c_str()); message.decimal_06 = const_cast<char*>(decimals[5].c_str());
    message.decimal_07 = const_cast<char*>(decimals[6].c_str()); message.decimal_08 = const_cast<char*>(decimals[7].c_str());
    message.decimal_09 = const_cast<char*>(decimals[8].c_str()); message.decimal_10 = const_cast<char*>(decimals[9].c_str());
    const auto bytes = benchmark__protobuf_c__one_string_ten_decimal__get_packed_size(&message);
    std::vector<std::uint8_t> buffer(bytes);
    run_measure(csv, "protobuf", "protobuf_c", "c_generated", "pack_preallocated", test_case, config, bytes,
                [&] { return benchmark__protobuf_c__one_string_ten_decimal__pack(&message, buffer.data()); });
  } else {
    Benchmark__ProtobufC__TenStringFiftyDecimal message = BENCHMARK__PROTOBUF_C__TEN_STRING_FIFTY_DECIMAL__INIT;
    const std::array<std::string, 10> symbols = {symbol(0), symbol(1), symbol(2), symbol(3), symbol(4),
                                                 symbol(5), symbol(6), symbol(7), symbol(8), symbol(9)};
    const std::array<std::string, 50> decimals = {
      decimal(0), decimal(1), decimal(2), decimal(3), decimal(4), decimal(5), decimal(6), decimal(7), decimal(8), decimal(9),
      decimal(10), decimal(11), decimal(12), decimal(13), decimal(14), decimal(15), decimal(16), decimal(17), decimal(18), decimal(19),
      decimal(20), decimal(21), decimal(22), decimal(23), decimal(24), decimal(25), decimal(26), decimal(27), decimal(28), decimal(29),
      decimal(30), decimal(31), decimal(32), decimal(33), decimal(34), decimal(35), decimal(36), decimal(37), decimal(38), decimal(39),
      decimal(40), decimal(41), decimal(42), decimal(43), decimal(44), decimal(45), decimal(46), decimal(47), decimal(48), decimal(49)};
    message.symbol_01 = const_cast<char*>(symbols[0].c_str()); message.symbol_02 = const_cast<char*>(symbols[1].c_str());
    message.symbol_03 = const_cast<char*>(symbols[2].c_str()); message.symbol_04 = const_cast<char*>(symbols[3].c_str());
    message.symbol_05 = const_cast<char*>(symbols[4].c_str()); message.symbol_06 = const_cast<char*>(symbols[5].c_str());
    message.symbol_07 = const_cast<char*>(symbols[6].c_str()); message.symbol_08 = const_cast<char*>(symbols[7].c_str());
    message.symbol_09 = const_cast<char*>(symbols[8].c_str()); message.symbol_10 = const_cast<char*>(symbols[9].c_str());
    char** decimal_fields[] = {&message.decimal_01, &message.decimal_02, &message.decimal_03, &message.decimal_04, &message.decimal_05,
                                &message.decimal_06, &message.decimal_07, &message.decimal_08, &message.decimal_09, &message.decimal_10,
                                &message.decimal_11, &message.decimal_12, &message.decimal_13, &message.decimal_14, &message.decimal_15,
                                &message.decimal_16, &message.decimal_17, &message.decimal_18, &message.decimal_19, &message.decimal_20,
                                &message.decimal_21, &message.decimal_22, &message.decimal_23, &message.decimal_24, &message.decimal_25,
                                &message.decimal_26, &message.decimal_27, &message.decimal_28, &message.decimal_29, &message.decimal_30,
                                &message.decimal_31, &message.decimal_32, &message.decimal_33, &message.decimal_34, &message.decimal_35,
                                &message.decimal_36, &message.decimal_37, &message.decimal_38, &message.decimal_39, &message.decimal_40,
                                &message.decimal_41, &message.decimal_42, &message.decimal_43, &message.decimal_44, &message.decimal_45,
                                &message.decimal_46, &message.decimal_47, &message.decimal_48, &message.decimal_49, &message.decimal_50};
    for (std::size_t i = 0; i < decimals.size(); ++i) *decimal_fields[i] = const_cast<char*>(decimals[i].c_str());
    const auto bytes = benchmark__protobuf_c__ten_string_fifty_decimal__get_packed_size(&message);
    std::vector<std::uint8_t> buffer(bytes);
    run_measure(csv, "protobuf", "protobuf_c", "c_generated", "pack_preallocated_empty_wide", test_case, config, bytes,
                [&] { return benchmark__protobuf_c__ten_string_fifty_decimal__pack(&message, buffer.data()); });
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
  metadata << "encoding_only=true\n";
  metadata << "protobuf_decimal_representation=string\n";
  metadata << "json_decimal_representation=string\n";
  metadata << "compiler=" << __VERSION__ << '\n';
}

}  // namespace

int main(int argc, char** argv) {
  try {
    const Config config = parse_args(argc, argv);
    write_metadata(config);
    Csv csv(config.csv_path);

    run_google_variant(csv, config, "speed", Case::Int64, make_int64_message<benchmark::google_speed::OneStringTenInt64>());
    run_google_variant(csv, config, "speed", Case::Decimal, make_decimal_message<benchmark::google_speed::OneStringTenDecimal>());
    run_google_variant(csv, config, "speed", Case::Wide, make_wide_message<benchmark::google_speed::TenStringFiftyDecimal>());
    for (const Case test_case : {Case::Int64, Case::Decimal, Case::Wide}) {
      if (test_case == Case::Int64) run_google_variant(csv, config, "code_size", test_case, make_int64_message<benchmark::google_codesize::OneStringTenInt64>());
      if (test_case == Case::Decimal) run_google_variant(csv, config, "code_size", test_case, make_decimal_message<benchmark::google_codesize::OneStringTenDecimal>());
      if (test_case == Case::Wide) run_google_variant(csv, config, "code_size", test_case, make_wide_message<benchmark::google_codesize::TenStringFiftyDecimal>());
    }
    for (const Case test_case : {Case::Int64, Case::Decimal, Case::Wide}) {
      if (test_case == Case::Int64) run_google_variant(csv, config, "lite_runtime", test_case, make_int64_message<benchmark::google_lite::OneStringTenInt64>());
      if (test_case == Case::Decimal) run_google_variant(csv, config, "lite_runtime", test_case, make_decimal_message<benchmark::google_lite::OneStringTenDecimal>());
      if (test_case == Case::Wide) run_google_variant(csv, config, "lite_runtime", test_case, make_wide_message<benchmark::google_lite::TenStringFiftyDecimal>());
    }

    for (const Case test_case : {Case::Int64, Case::Decimal, Case::Wide}) run_protobuf_c(csv, config, test_case);
    for (const Case test_case : {Case::Int64, Case::Decimal, Case::Wide}) {
      run_rapidjson(csv, config, test_case);
      run_nlohmann(csv, config, test_case);
      run_jsoncpp(csv, config, test_case);
      run_boost_json(csv, config, test_case);
      run_yyjson(csv, config, test_case);
    }
    std::cerr << "Benchmark complete: " << config.csv_path << '\n';
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "ERROR: " << error.what() << '\n';
    return 1;
  }
}

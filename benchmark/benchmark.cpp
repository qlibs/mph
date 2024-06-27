//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#define ANKERL_NANOBENCH_IMPLEMENT

#include <mph>
#include <string_view>
#include <algorithm>
#include <array>
#include <boost/container/flat_map.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/unordered_map.hpp>
#include <map>
#include <random>
#include <string_view>
#include <unordered_map>

#include "data.hpp"
#include "nanobench.h"

namespace mph {
[[nodiscard]] constexpr auto operator<(const fixed_string& lhs, const std::string_view& rhs) -> bool {
  return std::string_view{lhs.data(), lhs.size()} < rhs;
}

[[nodiscard]] constexpr auto operator<(const fixed_string& lhs, const fixed_string& rhs) -> bool {
  return std::string_view{lhs.data(), lhs.size()} < std::string_view{rhs.data(), rhs.size()};
}
} // namespace mph

template <>
struct boost::hash<mph::fixed_string> {
  std::size_t operator()(const mph::fixed_string key) const {
    return boost::hash<std::string_view>{}(std::string_view{key.data(), key.size()});
  }
};

int main() {
  using namespace ankerl::nanobench;

  static constexpr auto iterations = 1'000'000;

  const auto bench_std_map = [](const auto name, const auto &keys, auto fn) {
    using key_type = std::remove_cvref_t<decltype(keys[0].first)>;
    using value_type = std::remove_cvref_t<decltype(keys[0].second)>;
    std::map<key_type, value_type> map{};
    for (const auto &[id, value] : keys) {
      map[id] = value;
    }
    const auto hash = [&](const auto &key) {
      if (const auto index = map.find(key); index != std::cend(map)) {
        return index->second;
      }
      return typename decltype(map)::value_type::second_type{};
    };
    Bench()
        .performanceCounters(true)
        .minEpochIterations(iterations)
        .run(std::string(name) + ".std.map",
             [&] { doNotOptimizeAway(hash(fn(keys))); });
  };

  const auto bench_std_unordered_map = [](const auto name, const auto &keys,
                                          auto fn) {
    using key_type = std::remove_cvref_t<decltype(keys[0].first)>;
    using value_type = std::remove_cvref_t<decltype(keys[0].second)>;
    std::unordered_map<key_type, value_type, boost::hash<key_type>> hash_map{};
    for (const auto &[id, value] : keys) {
      hash_map[id] = value;
    }
    const auto hash = [&](const auto &key) {
      if (const auto index = hash_map.find(key); index != std::cend(hash_map)) {
        return index->second;
      }
      return typename decltype(hash_map)::value_type::second_type{};
    };
    Bench()
        .performanceCounters(true)
        .minEpochIterations(iterations)
        .run(std::string(name) + ".std.unordered_map",
             [&] { doNotOptimizeAway(hash(fn(keys))); });
  };

  const auto bench_boost_unordered_map = [](const auto name, const auto &keys,
                                            auto fn) {
    using key_type = std::remove_cvref_t<decltype(keys[0].first)>;
    using value_type = std::remove_cvref_t<decltype(keys[0].second)>;
    boost::unordered_map<key_type, value_type> hash_map{};
    for (const auto &[id, value] : keys) {
      hash_map[id] = value;
    }
    const auto hash = [&](const auto &key) {
      if (const auto index = hash_map.find(key); index != std::cend(hash_map)) {
        return index->second;
      }
      return typename decltype(hash_map)::value_type::second_type{};
    };
    Bench()
        .performanceCounters(true)
        .minEpochIterations(iterations)
        .run(std::string(name) + ".boost.unordered_map",
             [&] { doNotOptimizeAway(hash(fn(keys))); });
  };

  const auto bench_boost_flat_map = [](const auto name, const auto &keys,
                                       auto fn) {
    using key_type = std::remove_cvref_t<decltype(keys[0].first)>;
    using value_type = std::remove_cvref_t<decltype(keys[0].second)>;
    boost::container::flat_map<key_type, value_type> hash_map{};
    for (const auto &[id, value] : keys) {
      hash_map[id] = value;
    }
    const auto hash = [&](const auto &key) {
      if (const auto index = hash_map.find(key); index != std::cend(hash_map)) {
        return index->second;
      }
      return typename decltype(hash_map)::value_type::second_type{};
    };
    Bench()
        .performanceCounters(true)
        .minEpochIterations(iterations)
        .run(std::string(name) + ".boost.flat_map",
             [&] { doNotOptimizeAway(hash(fn(keys))); });
  };

  const auto bench_gperf = [](const auto &hash, const auto name,
                              const auto &keys, auto fn) {
    Bench()
        .performanceCounters(true)
        .minEpochIterations(iterations)
        .run(std::string(name) + ".gperf", [&] {
          const auto key = fn(keys);
          doNotOptimizeAway(hash(std::data(key), std::size(key)));
        });
  };

  const auto bench_mph = []<auto keys>(const auto& name, auto fn) {
    Bench()
      .performanceCounters(true)
      .minEpochIterations(iterations).run(std::string(name) + ".mph", [&] {
        doNotOptimizeAway(mph::hash<keys>(fn(keys)));
    });
  };

  const auto bench_mph_loop = []<auto keys>(const auto& name) {
    auto key = keys[0].first;
    Bench()
      .performanceCounters(true)
      .minEpochIterations(iterations).run(std::string(name) + ".mph", [&] {
        key = mph::hash<keys>(key);
        doNotOptimizeAway(key);
    });
  };

  {
    std::random_device rd{};
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(
        0, std::size(data::random_strings_5_len_4) - 1);
    std::vector<int> ids{};
    ids.reserve(iterations);
    for (auto i = 0; i < iterations; ++i) {
      ids.push_back(distribution(gen));
    }

    auto next = [&, i = 0](const auto &keys) mutable {
      return keys[ids[i++ % iterations]].first;
    };

    bench_std_map("random_strings_5_len_4", data::random_strings_5_len_4, next);
    bench_std_unordered_map("random_strings_5_len_4",
                            data::random_strings_5_len_4, next);
    bench_boost_unordered_map("random_strings_5_len_4",
                              data::random_strings_5_len_4, next);
    bench_boost_flat_map("random_strings_5_len_4", data::random_strings_5_len_4,
                         next);
    bench_gperf(
        [](const char *str, std::size_t len) {
          static constexpr auto MIN_WORD_LENGTH = 4;
          static constexpr auto MAX_WORD_LENGTH = 4;
          static constexpr auto MAX_HASH_VALUE = 7;

          static constexpr const unsigned char asso_values[] = {
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 4, 7, 8, 2, 8, 8, 8, 8, 8, 8, 5, 8, 8, 8, 8, 8, 0, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};

          static constexpr const char *wordlist[] = {
              "SPCB", "", "FXB ", "", "CODI", "MOHO", "", "DDMX"};

          static constexpr const std::uint8_t index[] = {
              1, 0, 2, 0, 3, 4, 0, 5,
          };

          const auto hash = [&] {
            return asso_values[static_cast<unsigned char>(str[0])];
          };

          if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
            unsigned int key = hash();

            if (key <= MAX_HASH_VALUE) {
              const char *s = wordlist[key];

              if (*str == *s && !strcmp(str + 1, s + 1)) {
                return index[key];
              }
            }
          }
          return decltype(index[0]){};
        },
        "random_strings_5_len_4", data::random_strings_5_len_4, next);
    bench_mph.template operator()<data::random_strings_5_len_4>("random_strings_5_len_4", next);
  }

  {
    std::random_device rd{};
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(
        0, std::size(data::random_strings_5_len_8) - 1);
    std::vector<int> ids{};
    ids.reserve(iterations);
    for (auto i = 0; i < iterations; ++i) {
      ids.push_back(distribution(gen));
    }

    auto next = [&, i = 0](const auto &keys) mutable {
      return keys[ids[i++ % iterations]].first;
    };

    bench_std_map("random_strings_5_len_8", data::random_strings_5_len_8, next);
    bench_std_unordered_map("random_strings_5_len_8",
                            data::random_strings_5_len_8, next);
    bench_boost_unordered_map("random_strings_5_len_8",
                              data::random_strings_5_len_8, next);
    bench_boost_flat_map("random_strings_5_len_8", data::random_strings_5_len_8,
                         next);
    bench_gperf(
        [](const char *str, std::size_t len) {
          static constexpr auto MIN_WORD_LENGTH = 8;
          static constexpr auto MAX_WORD_LENGTH = 8;
          static constexpr auto MAX_HASH_VALUE = 7;

          static constexpr const unsigned char asso_values[] = {
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 4, 7, 8, 2, 8, 8, 8, 8, 8, 8, 5, 8, 8, 8, 8, 8, 0, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};

          static constexpr const char *wordlist[] = {
              "SPCB    ", "",         "FXB     ", "",
              "CODI    ", "MOHO    ", "",         "DDMX    "};

          static constexpr const std::uint8_t index[] = {
              1, 0, 2, 0, 3, 4, 0, 5,
          };

          const auto hash = [&] {
            return asso_values[static_cast<unsigned char>(str[0])];
          };

          if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
            unsigned int key = hash();

            if (key <= MAX_HASH_VALUE) {
              const char *s = wordlist[key];

              if (*str == *s && !strcmp(str + 1, s + 1)) {
                return index[key];
              }
            }
          }
          return decltype(index[0]){};
        },
        "random_strings_5_len_8", data::random_strings_5_len_8, next);
    bench_mph.template operator()<data::random_strings_5_len_8>("random_strings_5_len_8", next);
  }

  {
    std::random_device rd{};
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(
        0, std::size(data::random_strings_6_len_2_5) - 1);
    std::vector<int> ids{};
    ids.reserve(iterations);
    for (auto i = 0; i < iterations; ++i) {
      ids.push_back(distribution(gen));
    }

    auto next = [&, i = 0](const auto &keys) mutable {
      return keys[ids[i++ % iterations]].first;
    };

    bench_std_map("random_strings_6_len_2_5", data::random_strings_6_len_2_5,
                  next);
    bench_std_unordered_map("random_strings_6_len_2_5",
                            data::random_strings_6_len_2_5, next);
    bench_boost_unordered_map("random_strings_6_len_2_5",
                              data::random_strings_6_len_2_5, next);
    bench_boost_flat_map("random_strings_6_len_2_5",
                         data::random_strings_6_len_2_5, next);
    bench_gperf(
        [](const char *str, std::size_t len) {
          static constexpr auto MIN_WORD_LENGTH = 2;
          static constexpr auto MAX_WORD_LENGTH = 5;
          static constexpr auto MAX_HASH_VALUE = 9;

          static constexpr const unsigned char asso_values[] = {
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 5,  10, 0,  10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 0,  10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};

          static constexpr const char *wordlist[] = {
              "", "", "ws", "wss", "http", "https", "", "", "ftp", "file"};

          static constexpr const std::uint8_t index[] = {
              0, 0, 1, 2, 3, 4, 0, 0, 5, 6,
          };

          const auto hash = [&] {
            return len + asso_values[static_cast<unsigned char>(str[0])];
          };

          if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
            unsigned int key = hash();

            if (key <= MAX_HASH_VALUE) {
              const char *s = wordlist[key];

              if (*str == *s && !strcmp(str + 1, s + 1)) return index[key];
            }
          }
          return decltype(index[0]){};
        },
        "random_strings_6_len_2_5", data::random_strings_6_len_2_5, next);
    bench_mph.template operator()<data::random_strings_6_len_2_5>("random_strings_6_len_2_5", next);
  }

  {
    std::random_device rd{};
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(
        0, std::size(data::random_strings_100_len_8) - 1);
    std::vector<int> ids{};
    ids.reserve(iterations);
    for (auto i = 0; i < iterations; ++i) {
      ids.push_back(distribution(gen));
    }

    auto next = [&, i = 0](const auto &keys) mutable {
      return keys[ids[i++ % iterations]].first;
    };

    bench_std_map("random_strings_100_len_8", data::random_strings_100_len_8,
                  next);
    bench_std_unordered_map("random_strings_100_len_8",
                            data::random_strings_100_len_8, next);
    bench_boost_unordered_map("random_strings_100_len_8",
                              data::random_strings_100_len_8, next);
    bench_boost_flat_map("random_strings_100_len_8",
                         data::random_strings_100_len_8, next);
    bench_gperf(
        [](const char *str, std::size_t len) {
          static constexpr auto MIN_WORD_LENGTH = 8;
          static constexpr auto MAX_WORD_LENGTH = 8;
          static constexpr auto MAX_HASH_VALUE = 302;

          static constexpr const unsigned short asso_values[] = {
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 25,  303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 0,   303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              80,  125, 5,   40,  25,  10,  45,  52,  10,  115, 117, 82,  55,
              95,  7,   70,  4,   105, 27,  0,   2,   70,  0,   9,   127, 5,
              10,  70,  303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303};

          static constexpr const char *wordlist[] = {
              "",         "",         "",         "", "QTRX    ",
              "",         "",         "TWM     ", "", "TCO     ",
              "WIRE    ", "",         "",         "", "",
              "IWX     ", "",         "",         "", "FQAL    ",
              "FFR     ", "",         "",         "", "",
              "ZIG     ", "",         "",         "", "USRT    ",
              "WEAT    ", "",         "",         "", "EXR     ",
              "EWD     ", "",         "",         "", "SFST    ",
              "IEA     ", "",         "FEMS    ", "", "TDOC    ",
              "TDAC    ", "",         "CUBB    ", "", "",
              "IWB     ", "",         "CDMO    ", "", "",
              "FTEK    ", "",         "UMRX    ", "", "",
              "MCRO    ", "",         "HTGM    ", "", "GXGXW   ",
              "EDUC    ", "",         "STBA    ", "", "",
              "DEACW   ", "",         "IHD     ", "", "",
              "CPRT    ", "",         "UCTT    ", "", "XTN     ",
              "IVR-A   ", "",         "SIEB    ", "", "ZXZZT   ",
              "VIACA   ", "",         "GDS     ", "", "",
              "IPG     ", "",         "",         "", "",
              "PWC     ", "",         "NURE    ", "", "GHSI    ",
              "",         "",         "NCSM    ", "", "",
              "TAC     ", "",         "ETJ     ", "", "",
              "MIE     ", "",         "DSE     ", "", "INVA    ",
              "JW.A    ", "",         "WTID    ", "", "",
              "",         "",         "FRME    ", "", "",
              "DGBP    ", "",         "",         "", "",
              "BWA     ", "QFIN    ", "AGM-C   ", "", "BCOR    ",
              "FMNB    ", "",         "III     ", "", "",
              "GENY    ", "",         "CYD     ", "", "SWIR    ",
              "FDL     ", "",         "CEIX    ", "", "",
              "DCP-B   ", "",         "LVUS    ", "", "",
              "ZAZZT   ", "",         "INFN    ", "", "BSMX    ",
              "FBC     ", "",         "VLYPO   ", "", "",
              "NEE-I   ", "",         "GLBS    ", "", "",
              "GPK     ", "",         "CYBE    ", "", "",
              "",         "",         "SB      ", "", "",
              "",         "",         "",         "", "",
              "IRTC    ", "",         "",         "", "",
              "",         "",         "JHCS    ", "", "LOPE    ",
              "MNE     ", "",         "BVSN    ", "", "",
              "RETL    ", "",         "HMLP-A  ", "", "VYMI    ",
              "",         "",         "",         "", "KLDO    ",
              "TRPX    ", "",         "LRCX    ", "", "CLWT    ",
              "NJAN    ", "",         "",         "", "",
              "",         "",         "",         "", "",
              "",         "",         "KRA     ", "", "",
              "BAC-A   ", "",         "BHK     ", "", "",
              "PNNTG   ", "",         "BRMK    ", "", "",
              "ARKG    ", "",         "",         "", "",
              "",         "",         "",         "", "",
              "",         "",         "LMHB    ", "", "",
              "RBCAA   ", "",         "",         "", "",
              "",         "",         "",         "", "",
              "",         "",         "LVHD    ", "", "",
              "NRZ     ", "",         "",         "", "",
              "GJH     ", "",         "",         "", "",
              "",         "",         "",         "", "",
              "",         "",         "",         "", "",
              "NBTB    ", "",         "",         "", "",
              "",         "",         "",         "", "",
              "",         "",         "RYN     "};
          static constexpr const std::uint8_t index[] = {
              0,  0,  0,  0,  1,  0,  0,  2,  0,  3,  4,  0,  0,  0,  0,   5,
              0,  0,  0,  6,  7,  0,  0,  0,  0,  8,  0,  0,  0,  9,  10,  0,
              0,  0,  11, 12, 0,  0,  0,  13, 14, 0,  15, 0,  16, 17, 0,   18,
              0,  0,  19, 0,  20, 0,  0,  21, 0,  22, 0,  0,  23, 0,  24,  0,
              25, 26, 0,  27, 0,  0,  28, 0,  29, 0,  0,  30, 0,  31, 0,   32,
              33, 0,  34, 0,  35, 36, 0,  37, 0,  0,  38, 0,  0,  0,  0,   39,
              0,  40, 0,  41, 0,  0,  42, 0,  0,  43, 0,  44, 0,  0,  45,  0,
              46, 0,  47, 48, 0,  49, 0,  0,  0,  0,  50, 0,  0,  51, 0,   0,
              0,  0,  52, 53, 54, 0,  55, 56, 0,  57, 0,  0,  58, 0,  59,  0,
              60, 61, 0,  62, 0,  0,  63, 0,  64, 0,  0,  65, 0,  66, 0,   67,
              68, 0,  69, 0,  0,  70, 0,  71, 0,  0,  72, 0,  73, 0,  0,   0,
              0,  74, 0,  0,  0,  0,  0,  0,  0,  75, 0,  0,  0,  0,  0,   0,
              76, 0,  77, 78, 0,  79, 0,  0,  80, 0,  81, 0,  82, 0,  0,   0,
              0,  83, 84, 0,  85, 0,  86, 87, 0,  0,  0,  0,  0,  0,  0,   0,
              0,  0,  0,  88, 0,  0,  89, 0,  90, 0,  0,  91, 0,  92, 0,   0,
              93, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  94, 0,  0,   95,
              0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  96, 0,  0,  97,  0,
              0,  0,  0,  98, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,
              0,  0,  99, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  100,
          };

          const auto hash = [&] {
            return asso_values[static_cast<unsigned char>(str[2] + 2)] +
                   asso_values[static_cast<unsigned char>(str[1])] +
                   asso_values[static_cast<unsigned char>(str[0])];
          };

          if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
            unsigned int key = hash();

            if (key <= MAX_HASH_VALUE) {
              const char *s = wordlist[key];

              if (*str == *s && !strcmp(str + 1, s + 1)) {
                return index[key];
              }
            }
          }
          return decltype(index[0]){};
        },
        "random_strings_100_len_8", data::random_strings_100_len_8, next);
    bench_mph.template operator()<data::random_strings_100_len_8>("random_strings_100_len_8", next);
  }

  {
    std::random_device rd{};
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(
        0, std::size(data::random_strings_100_len_8) - 1);
    std::vector<int> ids{};
    ids.reserve(iterations);
    for (auto i = 0; i < iterations; ++i) {
      ids.push_back(distribution(gen));
    }

    auto next = [&, i = 0](const auto &keys) mutable {
      return keys[ids[i++ % iterations]].first;
    };

    bench_std_map("random_strings_100_len_1_8",
                  data::random_strings_100_len_1_8, next);
    bench_std_unordered_map("random_strings_100_len_1_8",
                            data::random_strings_100_len_1_8, next);
    bench_boost_unordered_map("random_strings_100_len_1_8",
                              data::random_strings_100_len_1_8, next);
    bench_boost_flat_map("random_strings_100_len_1_8",
                         data::random_strings_100_len_1_8, next);
    bench_gperf(
        [](const char *str, std::size_t len) {
          static constexpr auto MIN_WORD_LENGTH = 1;
          static constexpr auto MAX_WORD_LENGTH = 8;
          static constexpr auto MAX_HASH_VALUE = 186;

          static constexpr const unsigned char asso_values[] = {
              187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187,
              187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187,
              187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187,
              187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 0,   0,   187,
              0,   187, 5,   187, 5,   0,   187, 187, 187, 187, 187, 187, 187,
              2,   35,  20,  120, 85,  80,  35,  2,   187, 0,   187, 55,  35,
              187, 125, 100, 7,   80,  10,  0,   187, 85,  60,  187, 187, 110,
              187, 187, 187, 187, 187, 187, 0,   20,  10,  10,  0,   115, 65,
              50,  10,  187, 30,  5,   70,  15,  5,   5,   187, 5,   0,   20,
              15,  70,  40,  187, 110, 0,   187, 187, 187, 187, 187, 187, 187,
              187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187,
              187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187,
              187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187,
              187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187,
              187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187,
              187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187,
              187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187,
              187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187,
              187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187,
              187, 187, 187, 187, 187, 187, 187, 187, 187};

          static constexpr const char *wordlist[] = {
              "",      "1",      "",        "",         "1234",
              "",      "",       "zzzzzzz", "11111111", "",
              "sleep", "",       "Happy",   "12345678", "",
              "apple", "Serene", "Journey", "98765432", "",
              "Solar", "Sphere", "",        "Cat",      "",
              "Spark", "",       "Quiet",   "Sun",      "",
              "Comet", "Spiral", "Sparkle", "Quasar",   "",
              "Color", "Candle", "cascade", "",         "",
              "Stars", "Secret", "Sincere", "Tranquil", "",
              "Cloud", "Breeze", "Glisten", "",         "",
              "Beach", "Cuddle", "lullaby", "Autumn",   "",
              "Sweet", "Bridge", "",        "keyboard", "Moon",
              "Swirl", "Mirror", "",        "Mountain", "",
              "Music", "Beacon", "Glitter", "",         "",
              "Misty", "Silken", "",        "",         "Book",
              "phone", "Bubble", "",        "",         "",
              "Child", "Guitar", "",        "",         "",
              "Windy", "Travel", "",        "",         "",
              "Smile", "Basket", "",        "Luminous", "Harmony",
              "River", "Spring", "",        "",         "Rain",
              "Fruit", "Winter", "",        "",         "Echo",
              "Petal", "Summer", "Feather", "",         "",
              "Earth", "Forest", "Radiant", "Ethereal", "",
              "Peace", "Shadow", "Whisper", "Twilight", "",
              "Light", "Puzzle", "Whistle", "",         "",
              "Laugh", "Window", "",        "Dog",      "",
              "Dream", "Puddle", "",        "",         "",
              "Dance", "Flower", "",        "Symphony", "",
              "Ocean", "Rocket", "",        "",         "",
              "",      "Coffee", "",        "",         "",
              "",      "Galaxy", "",        "",         "",
              "",      "Wisdom", "",        "",         "",
              "",      "Pillow", "",        "",         "",
              "",      "Beauty", "Elusive", "",         "",
              "Swift", "Zephyr", "Enchant", "",         "",
              "",      "Flight", "",        "",         "",
              "",      "Velvet", "",        "",         "",
              "",      "Warmth"};

          static constexpr const std::uint8_t index[] = {
              0,  1,  0,  0,  2,  0,  0,  3,  4,  0,  5,   0,  6,  7,  0,  8,
              9,  10, 11, 0,  12, 13, 0,  14, 0,  15, 0,   16, 17, 0,  18, 19,
              20, 21, 0,  22, 23, 24, 0,  0,  25, 26, 27,  28, 0,  29, 30, 31,
              0,  0,  32, 33, 34, 35, 0,  36, 37, 0,  38,  39, 40, 41, 0,  42,
              0,  43, 44, 45, 0,  0,  46, 47, 0,  0,  48,  49, 50, 0,  0,  0,
              51, 52, 0,  0,  0,  53, 54, 0,  0,  0,  55,  56, 0,  57, 58, 59,
              60, 0,  0,  61, 62, 63, 0,  0,  64, 65, 66,  67, 0,  0,  68, 69,
              70, 71, 0,  72, 73, 74, 75, 0,  76, 77, 78,  0,  0,  79, 80, 0,
              81, 0,  82, 83, 0,  0,  0,  84, 85, 0,  86,  0,  87, 88, 0,  0,
              0,  0,  89, 0,  0,  0,  0,  90, 0,  0,  0,   0,  91, 0,  0,  0,
              0,  92, 0,  0,  0,  0,  93, 94, 0,  0,  95,  96, 97, 0,  0,  0,
              98, 0,  0,  0,  0,  99, 0,  0,  0,  0,  100,
          };

          const auto hash = [&] {
            unsigned int hval = len;

            switch (hval) {
              default:
                hval += asso_values[static_cast<unsigned char>(str[5])];
              /*FALLTHROUGH*/
              case 5:
              case 4:
                hval += asso_values[static_cast<unsigned char>(str[3])];
              /*FALLTHROUGH*/
              case 3:
              case 2:
                hval += asso_values[static_cast<unsigned char>(str[1])];
              /*FALLTHROUGH*/
              case 1:
                hval += asso_values[static_cast<unsigned char>(str[0])];
                break;
            }
            return hval;
          };

          if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
            unsigned int key = hash();

            if (key <= MAX_HASH_VALUE) {
              const char *s = wordlist[key];

              if (*str == *s && !strcmp(str + 1, s + 1)) {
                return index[key];
              }
            }
          }
          return decltype(index[0]){};
        },
        "random_strings_100_len_1_8", data::random_strings_100_len_1_8, next);
    bench_mph.template operator()<data::random_strings_100_len_1_8>("random_strings_100_len_1_8", next);
    bench_mph_loop.template operator()<data::random_uints_5>("random_uints_5");
  }
}

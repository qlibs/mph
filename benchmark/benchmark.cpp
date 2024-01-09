//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#define ANKERL_NANOBENCH_IMPLEMENT
#include <algorithm>
#include <array>
#include <boost/container/flat_map.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/unordered_map.hpp>
#include <map>
#include <mph>
#include <random>
#include <unordered_map>

#include "data.hpp"
#include "nanobench.h"

int main() {
  using namespace ankerl::nanobench;

  static constexpr auto iterations = 1'000'000;

  const auto bench_std_map = [](const auto name, const auto &symbols, auto fn) {
    std::map<std::remove_cvref_t<decltype(symbols[0])>, std::uint8_t> map{};
    for (auto index = 0u; const auto &symbol : symbols) {
      map[symbol] = ++index;
    }
    const auto hash = [&](const auto &symbol) {
      if (const auto index = map.find(symbol); index != std::cend(map)) {
        return index->second;
      }
      return typename decltype(map)::value_type::second_type{};
    };
    Bench().minEpochIterations(iterations).run(std::string(name) + ".std.map", [&] { doNotOptimizeAway(hash(fn(symbols))); });
  };

  const auto bench_std_unordered_map = [](const auto name, const auto &symbols, auto fn) {
    using key_type = std::remove_cvref_t<decltype(symbols[0])>;
    std::unordered_map<key_type, std::uint8_t, boost::hash<key_type>> hash_map{};
    for (auto index = 0u; const auto &symbol : symbols) {
      hash_map[symbol] = ++index;
    }
    const auto hash = [&](const auto &symbol) {
      if (const auto index = hash_map.find(symbol); index != std::cend(hash_map)) {
        return index->second;
      }
      return typename decltype(hash_map)::value_type::second_type{};
    };
    Bench().minEpochIterations(iterations).run(std::string(name) + ".std.unordered_map", [&] {
      doNotOptimizeAway(hash(fn(symbols)));
    });
  };

  const auto bench_boost_unordered_map = [](const auto name, const auto &symbols, auto fn) {
    using key_type = std::remove_cvref_t<decltype(symbols[0])>;
    boost::unordered_map<key_type, std::uint8_t> hash_map{};
    for (auto index = 0u; const auto &symbol : symbols) {
      hash_map[symbol] = ++index;
    }
    const auto hash = [&](const auto &symbol) {
      if (const auto index = hash_map.find(symbol); index != std::cend(hash_map)) {
        return index->second;
      }
      return typename decltype(hash_map)::value_type::second_type{};
    };
    Bench().minEpochIterations(iterations).run(std::string(name) + ".boost.unordered_map", [&] {
      doNotOptimizeAway(hash(fn(symbols)));
    });
  };

  const auto bench_std_bsearch = [](const auto name, const auto &symbols, auto fn) {
    using key_type = std::remove_cvref_t<decltype(symbols[0])>;
    std::vector<key_type> tokens(std::cbegin(symbols), std::cend(symbols));
    std::sort(std::begin(tokens), std::end(tokens));
    const auto bsearch = [&](const auto data) -> int {
      if (const auto found = std::binary_search(std::cbegin(tokens), std::cend(tokens), data)) {
        return 1u + std::distance(std::cbegin(tokens), std::lower_bound(std::cbegin(tokens), std::cend(tokens), data));
      }
      return 0u;
    };
    Bench().minEpochIterations(iterations).run(std::string(name) + ".std.bsearch", [&] {
      doNotOptimizeAway(bsearch(fn(symbols)));
    });
  };

  const auto bench_gperf = [](const auto &hash, const auto name, const auto &symbols, auto fn) {
    Bench().minEpochIterations(iterations).run(std::string(name) + ".gperf", [&] {
      const auto symbol = fn(symbols);
      doNotOptimizeAway(hash(std::data(symbol), std::size(symbol)));
    });
  };

  const auto bench_mph = [](const auto &hash, const auto name, const auto &symbols, auto fn) {
    Bench().minEpochIterations(iterations).run(std::string(name) + ".mph", [&] { doNotOptimizeAway(hash(fn(symbols))); });
  };

  {
    std::random_device rd{};
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, std::size(data::random_5_len_4) - 1);
    std::vector<int> ids{};
    ids.reserve(iterations);
    for (auto i = 0; i < iterations; ++i) {
      ids.push_back(distribution(gen));
    }

    auto next = [&, i = 0](const auto &symbols) mutable { return symbols[ids[i++ % iterations]]; };

    bench_std_map("random_5_len_4", data::random_5_len_4, next);
    bench_std_unordered_map("random_5_len_4", data::random_5_len_4, next);
    bench_boost_unordered_map("random_5_len_4", data::random_5_len_4, next);
    bench_std_bsearch("random_5_len_4", data::random_5_len_4, next);
    bench_gperf(
        [](const char *str, std::size_t len) {
          static constexpr auto MIN_WORD_LENGTH = 4;
          static constexpr auto MAX_WORD_LENGTH = 4;
          static constexpr auto MAX_HASH_VALUE = 7;

          static constexpr const unsigned char asso_values[] = {
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 7, 8, 2, 8, 8, 8,
              8, 8, 8, 5, 8, 8, 8, 8, 8, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};

          static constexpr const char *wordlist[] = {"SPCB", "", "FXB ", "", "CODI", "MOHO", "", "DDMX"};

          static constexpr const std::uint8_t index[] = {
              1, 0, 2, 0, 3, 4, 0, 5,
          };

          const auto hash = [&] { return asso_values[static_cast<unsigned char>(str[0])]; };

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
        "random_5_len_4", data::random_5_len_4, next);
    bench_mph(mph::hash{[] { return data::random_5_len_4; }}, "random_5_len_4", data::random_5_len_4, next);
  }

  {
    std::random_device rd{};
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, std::size(data::random_5_len_8) - 1);
    std::vector<int> ids{};
    ids.reserve(iterations);
    for (auto i = 0; i < iterations; ++i) {
      ids.push_back(distribution(gen));
    }

    auto next = [&, i = 0](const auto &symbols) mutable { return symbols[ids[i++ % iterations]]; };

    bench_std_map("random_5_len_8", data::random_5_len_8, next);
    bench_std_unordered_map("random_5_len_8", data::random_5_len_8, next);
    bench_boost_unordered_map("random_5_len_8", data::random_5_len_8, next);
    bench_std_bsearch("random_5_len_8", data::random_5_len_8, next);
    bench_gperf(
        [](const char *str, std::size_t len) {
          static constexpr auto MIN_WORD_LENGTH = 8;
          static constexpr auto MAX_WORD_LENGTH = 8;
          static constexpr auto MAX_HASH_VALUE = 7;

          static constexpr const unsigned char asso_values[] = {
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 7, 8, 2, 8, 8, 8,
              8, 8, 8, 5, 8, 8, 8, 8, 8, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};

          static constexpr const char *wordlist[] = {"SPCB    ", "", "FXB     ", "", "CODI    ", "MOHO    ", "", "DDMX    "};

          static constexpr const std::uint8_t index[] = {
              1, 0, 2, 0, 3, 4, 0, 5,
          };

          const auto hash = [&] { return asso_values[static_cast<unsigned char>(str[0])]; };

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
        "random_5_len_8", data::random_5_len_8, next);
    bench_mph(mph::hash{[] { return data::random_5_len_8; }}, "random_5_len_8", data::random_5_len_8, next);
  }

  {
    std::random_device rd{};
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, std::size(data::random_6_len_3_5) - 1);
    std::vector<int> ids{};
    ids.reserve(iterations);
    for (auto i = 0; i < iterations; ++i) {
      ids.push_back(distribution(gen));
    }

    auto next = [&, i = 0](const auto &symbols) mutable { return symbols[ids[i++ % iterations]]; };

    bench_std_map("random_6_len_3_5", data::random_6_len_3_5, next);
    bench_std_unordered_map("random_6_len_3_5", data::random_6_len_3_5, next);
    bench_boost_unordered_map("random_6_len_3_5", data::random_6_len_3_5, next);
    bench_std_bsearch("random_6_len_3_5", data::random_6_len_3_5, next);
    bench_gperf(
        [](const char *str, std::size_t len) {
          static constexpr auto MIN_WORD_LENGTH = 2;
          static constexpr auto MAX_WORD_LENGTH = 5;
          static constexpr auto MAX_HASH_VALUE = 9;

          static constexpr const unsigned char asso_values[] = {
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 5,  10,
              0,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 0,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
              10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};

          static constexpr const char *wordlist[] = {"", "", "ws", "wss", "http", "https", "", "", "ftp", "file"};

          static constexpr const std::uint8_t index[] = {
              0, 0, 1, 2, 3, 4, 0, 0, 5, 6,
          };

          const auto hash = [&] { return len + asso_values[static_cast<unsigned char>(str[0])]; };

          if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
            unsigned int key = hash();

            if (key <= MAX_HASH_VALUE) {
              const char *s = wordlist[key];

              if (*str == *s && !strcmp(str + 1, s + 1)) return index[key];
            }
          }
          return decltype(index[0]){};
        },
        "random_6_len_3_5", data::random_6_len_3_5, next);
    bench_mph(mph::hash{[] { return data::random_6_len_3_5; }}, "random_6_len_3_5", data::random_6_len_3_5, next);
  }

  {
    std::random_device rd{};
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, std::size(data::random_100_len_8) - 1);
    std::vector<int> ids{};
    ids.reserve(iterations);
    for (auto i = 0; i < iterations; ++i) {
      ids.push_back(distribution(gen));
    }

    auto next = [&, i = 0](const auto &symbols) mutable { return symbols[ids[i++ % iterations]]; };

    bench_std_map("random_100_len_8", data::random_100_len_8, next);
    bench_std_unordered_map("random_100_len_8", data::random_100_len_8, next);
    bench_boost_unordered_map("random_100_len_8", data::random_100_len_8, next);
    bench_std_bsearch("random_100_len_8", data::random_100_len_8, next);
    bench_gperf(
        [](const char *str, std::size_t len) {
          static constexpr auto MIN_WORD_LENGTH = 8;
          static constexpr auto MAX_WORD_LENGTH = 8;
          static constexpr auto MAX_HASH_VALUE = 302;

          static constexpr const unsigned short asso_values[] = {
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 25,  303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 0,   303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 80,
              125, 5,   40,  25,  10,  45,  52,  10,  115, 117, 82,  55,  95,  7,   70,  4,   105, 27,  0,   2,   70,  0,
              9,   127, 5,   10,  70,  303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303,
              303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303, 303};

          static constexpr const char *wordlist[] = {
              "",         "",         "",         "", "QTRX    ", "",         "", "TWM     ", "", "TCO     ",
              "WIRE    ", "",         "",         "", "",         "IWX     ", "", "",         "", "FQAL    ",
              "FFR     ", "",         "",         "", "",         "ZIG     ", "", "",         "", "USRT    ",
              "WEAT    ", "",         "",         "", "EXR     ", "EWD     ", "", "",         "", "SFST    ",
              "IEA     ", "",         "FEMS    ", "", "TDOC    ", "TDAC    ", "", "CUBB    ", "", "",
              "IWB     ", "",         "CDMO    ", "", "",         "FTEK    ", "", "UMRX    ", "", "",
              "MCRO    ", "",         "HTGM    ", "", "GXGXW   ", "EDUC    ", "", "STBA    ", "", "",
              "DEACW   ", "",         "IHD     ", "", "",         "CPRT    ", "", "UCTT    ", "", "XTN     ",
              "IVR-A   ", "",         "SIEB    ", "", "ZXZZT   ", "VIACA   ", "", "GDS     ", "", "",
              "IPG     ", "",         "",         "", "",         "PWC     ", "", "NURE    ", "", "GHSI    ",
              "",         "",         "NCSM    ", "", "",         "TAC     ", "", "ETJ     ", "", "",
              "MIE     ", "",         "DSE     ", "", "INVA    ", "JW.A    ", "", "WTID    ", "", "",
              "",         "",         "FRME    ", "", "",         "DGBP    ", "", "",         "", "",
              "BWA     ", "QFIN    ", "AGM-C   ", "", "BCOR    ", "FMNB    ", "", "III     ", "", "",
              "GENY    ", "",         "CYD     ", "", "SWIR    ", "FDL     ", "", "CEIX    ", "", "",
              "DCP-B   ", "",         "LVUS    ", "", "",         "ZAZZT   ", "", "INFN    ", "", "BSMX    ",
              "FBC     ", "",         "VLYPO   ", "", "",         "NEE-I   ", "", "GLBS    ", "", "",
              "GPK     ", "",         "CYBE    ", "", "",         "",         "", "SB      ", "", "",
              "",         "",         "",         "", "",         "IRTC    ", "", "",         "", "",
              "",         "",         "JHCS    ", "", "LOPE    ", "MNE     ", "", "BVSN    ", "", "",
              "RETL    ", "",         "HMLP-A  ", "", "VYMI    ", "",         "", "",         "", "KLDO    ",
              "TRPX    ", "",         "LRCX    ", "", "CLWT    ", "NJAN    ", "", "",         "", "",
              "",         "",         "",         "", "",         "",         "", "KRA     ", "", "",
              "BAC-A   ", "",         "BHK     ", "", "",         "PNNTG   ", "", "BRMK    ", "", "",
              "ARKG    ", "",         "",         "", "",         "",         "", "",         "", "",
              "",         "",         "LMHB    ", "", "",         "RBCAA   ", "", "",         "", "",
              "",         "",         "",         "", "",         "",         "", "LVHD    ", "", "",
              "NRZ     ", "",         "",         "", "",         "GJH     ", "", "",         "", "",
              "",         "",         "",         "", "",         "",         "", "",         "", "",
              "NBTB    ", "",         "",         "", "",         "",         "", "",         "", "",
              "",         "",         "RYN     "};
          static constexpr const std::uint8_t index[] = {
              0,  0,  0,  0,  1,  0,  0,  2,  0,  3,  4,  0,  0,  0,  0,  5,  0,  0,  0,  6,  7,  0,  0,   0,  0,  8,  0,  0,
              0,  9,  10, 0,  0,  0,  11, 12, 0,  0,  0,  13, 14, 0,  15, 0,  16, 17, 0,  18, 0,  0,  19,  0,  20, 0,  0,  21,
              0,  22, 0,  0,  23, 0,  24, 0,  25, 26, 0,  27, 0,  0,  28, 0,  29, 0,  0,  30, 0,  31, 0,   32, 33, 0,  34, 0,
              35, 36, 0,  37, 0,  0,  38, 0,  0,  0,  0,  39, 0,  40, 0,  41, 0,  0,  42, 0,  0,  43, 0,   44, 0,  0,  45, 0,
              46, 0,  47, 48, 0,  49, 0,  0,  0,  0,  50, 0,  0,  51, 0,  0,  0,  0,  52, 53, 54, 0,  55,  56, 0,  57, 0,  0,
              58, 0,  59, 0,  60, 61, 0,  62, 0,  0,  63, 0,  64, 0,  0,  65, 0,  66, 0,  67, 68, 0,  69,  0,  0,  70, 0,  71,
              0,  0,  72, 0,  73, 0,  0,  0,  0,  74, 0,  0,  0,  0,  0,  0,  0,  75, 0,  0,  0,  0,  0,   0,  76, 0,  77, 78,
              0,  79, 0,  0,  80, 0,  81, 0,  82, 0,  0,  0,  0,  83, 84, 0,  85, 0,  86, 87, 0,  0,  0,   0,  0,  0,  0,  0,
              0,  0,  0,  88, 0,  0,  89, 0,  90, 0,  0,  91, 0,  92, 0,  0,  93, 0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,
              94, 0,  0,  95, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  96, 0,  0,  97, 0,  0,  0,  0,   98, 0,  0,  0,  0,
              0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  99, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  100,
          };

          const auto hash = [&] {
            return asso_values[static_cast<unsigned char>(str[2] + 2)] + asso_values[static_cast<unsigned char>(str[1])] +
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
        "random_100_len_8", data::random_100_len_8, next);
    bench_mph(mph::hash{[] { return data::random_100_len_8; }}, "random_100_len_8", data::random_100_len_8, next);
  }
}

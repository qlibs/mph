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
#include <map>
#include <mph>
#include <random>
#include <unordered_map>

#include "data.hpp"
#include "nanobench.h"

int main() {
  using namespace ankerl::nanobench;

  static constexpr auto iterations = 1'000'000;

  const auto bench_map = [](const auto name, const auto &symbols, auto fn) {
    std::map<std::string_view, int> map{};
    for (auto index = 0; const auto &symbol : symbols) {
      map[symbol] = index++;
    }
    Bench().minEpochIterations(iterations).run(std::string(name) + ".map", [&] { doNotOptimizeAway(map[fn(symbols)]); });
  };

  const auto bench_unordered_map = [](const auto name, const auto &symbols, auto fn) {
    std::unordered_map<std::string_view, int> hash_map{};
    for (auto index = 0; const auto &symbol : symbols) {
      hash_map[symbol] = index++;
    }
    Bench().minEpochIterations(iterations).run(std::string(name) + ".unordered_map", [&] {
      doNotOptimizeAway(hash_map[fn(symbols)]);
    });
  };

  const auto bench_bsearch = [](const auto name, const auto &symbols, auto fn) {
    std::vector<std::string_view> tokens(std::cbegin(symbols), std::cend(symbols));
    std::sort(std::begin(tokens), std::end(tokens));
    const auto bsearch = [&](const auto data) -> int {
      if (const bool found = std::binary_search(std::cbegin(tokens), std::cend(tokens), data)) {
        return std::distance(std::cbegin(tokens), std::lower_bound(std::cbegin(tokens), std::cend(tokens), data));
      }
      return -1;
    };
    Bench().minEpochIterations(iterations).run(std::string(name) + ".bsearch", [&] {
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
    constexpr auto symbol_size = hash.symbols()[0].size();
    Bench().minEpochIterations(iterations).run(std::string(name) + ".mph", [&] {
      doNotOptimizeAway(hash(mph::utility::array_view<const char, symbol_size>(std::data(fn(symbols)))));
    });
  };

  const auto bench_mph2 = [](const auto &hash, const auto name, const auto &symbols, auto fn) {
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

    bench_map("random_5_len_4", data::random_5_len_4, next);
    bench_unordered_map("random_5_len_4", data::random_5_len_4, next);
    bench_bsearch("random_5_len_4", data::random_5_len_4, next);
    bench_gperf(
        [](const char *str, [[maybe_unused]] std::size_t len) {
          static constexpr unsigned char asso_values[] = {
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 7, 8, 2, 8, 8, 8,
              8, 8, 8, 5, 8, 8, 8, 8, 8, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};
          return asso_values[static_cast<unsigned char>(str[0])];
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

    bench_map("random_5_len_8", data::random_5_len_8, next);
    bench_unordered_map("random_5_len_8", data::random_5_len_8, next);
    bench_bsearch("random_5_len_8", data::random_5_len_8, next);
    bench_gperf(
        [](const char *str, [[maybe_unused]] std::size_t len) {
          static constexpr unsigned char asso_values[] = {
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 7, 8, 2, 8, 8, 8,
              8, 8, 8, 5, 8, 8, 8, 8, 8, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
              8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};
          return asso_values[static_cast<unsigned char>(str[0])];
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

    bench_map("random_6_len_3_5", data::random_6_len_3_5, next);
    bench_unordered_map("random_6_len_3_5", data::random_6_len_3_5, next);
    bench_bsearch("random_6_len_3_5", data::random_6_len_3_5, next);
    bench_gperf(
        [](const char *str, std::size_t len) {
          static constexpr unsigned char asso_values[] = {
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
          return len + asso_values[static_cast<unsigned char>(str[0])];
        },
        "random_6_len_3_5", data::random_6_len_3_5, next);
    bench_mph2(mph::hash{[] { return data::random_6_len_3_5; }}, "random_6_len_3_5", data::random_6_len_3_5, next);
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

    bench_map("random_100_len_8", data::random_100_len_8, next);
    bench_unordered_map("random_100_len_8", data::random_100_len_8, next);
    bench_bsearch("random_100_len_8", data::random_100_len_8, next);
    bench_gperf(
        [](const char *str, [[maybe_unused]] std::size_t len) {
          static constexpr unsigned short asso_values[] = {
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
          return asso_values[static_cast<unsigned char>(str[2] + 2)] + asso_values[static_cast<unsigned char>(str[1])] +
                 asso_values[static_cast<unsigned char>(str[0])];
        },
        "random_100_len_8", data::random_100_len_8, next);
    bench_mph(mph::hash{[] { return data::random_100_len_8; }}, "random_100_len_8", data::random_100_len_8, next);
  }
}

//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <mph>
#include <chrono>
#include <vector>
#include <iostream>
#include <utility>
#include <random>

using i32 = __INT32_TYPE__;
using u32 = __UINT32_TYPE__;
using i64 = __INT64_TYPE__;
using u64 = __UINT64_TYPE__;
using std::literals::operator""sv;

inline constexpr auto DATA = mph::utility::array{
  #include "/dev/shm/data.ipp"
};

void timeit(const auto& fn) {
  const auto start = std::chrono::high_resolution_clock::now();
  const auto checksum = fn();
  const auto stop = std::chrono::high_resolution_clock::now();
  std::cout << checksum << ":" << std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count() / double(1e9) << std::endl;
}

template<u32 Size, double Probability, u64 Seed, template<class, class, const auto&> class Benchmark, auto N = 100'000, const auto& input = DATA>
[[nodiscard]] auto int_to_int(auto fn) {
  using key_type = decltype(input[0].first);
  using mapped_type = decltype(input[0].second);
  std::mt19937_64 gen{Seed};
  std::uniform_int_distribution<key_type> dis{};
  mph::utility::array<std::vector<key_type>, 2u> data{};
  for (auto i = 0u; i < Size * N; ++i) {
    data[0].push_back(dis(gen));
    data[1].push_back(input[dis(gen) % Size].first);
  }
  std::vector<key_type> lookups(Size * N);
  for (auto i = 0u; i < lookups.size(); ++i) lookups[i] = data[(i % 100) < (Probability * 100)][i];

  Benchmark<key_type, mapped_type, input> benchmark{};
  timeit([&]{
    u64 checksum{};
    for (const auto& lookup: lookups) {
      checksum += (benchmark(lookup) != mapped_type{});
    }
    return checksum;
  });
}

template<u32 Size, double Probability, u64 Seed, template<class, class, const auto&> class Benchmark, auto N = 100'000, const auto& input = DATA>
[[nodiscard]] auto str_to_int() {
  using key_type = decltype(input[0].first);
  using mapped_type = decltype(input[0].second);
  std::mt19937_64 gen{Seed};
  std::uniform_int_distribution<u32> dis{};
  mph::utility::array<std::vector<std::string_view>, 2u> data{};
  for (auto i = 0u; i < Size * N; ++i) {
    data[0].push_back(&"..........."[i%10]);
    data[1].push_back(input[dis(gen) % Size].first);
  }
  std::vector<std::string_view> lookups(Size * N);
  for (auto i = 0u; i < lookups.size(); ++i) lookups[i] = data[(i % 100) < (Probability * 100)][i];

  Benchmark<key_type, mapped_type, input> benchmark{};
  timeit([&]{
    u64 checksum{};
    for (const auto& lookup: lookups) {
      checksum += benchmark(lookup);
    }
    return checksum;
  });
}

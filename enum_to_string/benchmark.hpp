//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <chrono>
#include <random>
#include <vector>
#include <iostream>
#include <algorithm>
#include <limits>
#include <array>
#include <climits>
#include <array>
#include <utility>

using i32 = __INT32_TYPE__;
using u32 = __UINT32_TYPE__;
using i64 = __INT64_TYPE__;
using u64 = __UINT64_TYPE__;

void benchmark(const auto& fn) {
  const auto start = std::chrono::high_resolution_clock::now();
  const auto checksum = fn();
  const auto stop = std::chrono::high_resolution_clock::now();
  std::cout << checksum << ":" << std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count() / double(1e9) << std::endl;
}

struct pcg {
  [[nodiscard]] constexpr auto operator()() noexcept -> u32 {
    u64 old_state = state;
    state = old_state * 6364136223846793005u + increment;
    u32 xor_shifted = ((old_state >> 18u) ^ old_state) >> 27u;
    u32 rot = u32(old_state >> 59u);
    return (xor_shifted >> rot) | (xor_shifted << ((-rot) & 31u));
  }
  u64 increment{1442695040888963407u};
  u64 state{5573589319906701683u + increment};
};

template<auto V> inline constexpr auto constant_v = V;
template<class T, auto N> inline constexpr auto entries = [] {
  constexpr auto data = []<auto... Ns>(std::index_sequence<Ns...>) {
    return std::array{constant_v<std::array<char, Ns>{}>.data()...};
  }(std::make_index_sequence<N>{});
  pcg random{};
  std::array<std::pair<T, const char*>, N> a{};
  for (auto i = 0u; i < N; ++i) {
    T r{};
    do { r = random(); } while (std::find_if(a.begin(), a.end(), [&](const auto& p){ return p.first == r; }) != a.end());
    a[i] = {r, data[i]};
  }
  return a;
}();

template<u32 Size, double Probability, class T, u64 Seed, auto N = 100'000>
[[nodiscard]] auto enum_to_string(auto fn) {
  static constexpr auto input = entries<T, Size>;
  std::mt19937_64 gen{Seed};
  std::uniform_int_distribution<T> dis{};
  std::array<std::vector<T>, 2u> data{};
  for (auto i = 0u; i < Size * N; ++i) {
    data[0].push_back(Size + dis(gen));
    data[1].push_back(input[dis(gen) % Size].first);
  }
  std::vector<T> lookups(Size * N);
  for (auto i = 0u; i < lookups.size(); ++i) lookups[i] = data[(i % 100) < (Probability * 100)][i];

  benchmark([&]{
    u64 checksum{};
    for (const auto& lookup: lookups) {
      checksum += (fn.template operator()<input>(lookup) != "");
    }
    return checksum;
  });
}

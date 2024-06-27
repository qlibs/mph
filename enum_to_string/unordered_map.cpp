//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <benchmark.hpp>
#include <unordered_map>

template <const auto& entries, auto I = 0>
auto find(auto value) -> const char* {
  static const auto values = []<auto... Ns>(std::index_sequence<Ns...>) {
    return std::unordered_map<UNDERLYING_TYPE, const char*>{entries[Ns]...};
  }(std::make_index_sequence<entries.size()>{});
  const auto it = values.find(value);
  return it != values.end() ? it->second : "";
}

int main() {
  BENCHMARK<SIZE, PROBABILITY, UNDERLYING_TYPE, SEED>([]<const auto& entries>(auto value) { return find<entries>(value); });
}

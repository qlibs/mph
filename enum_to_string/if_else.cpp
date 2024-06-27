//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <benchmark.hpp>

template <const auto& entries, auto I = 0>
constexpr auto find(auto value) -> const char* {
  if constexpr (I == entries.size()) {
    return "";
  } else if (value == entries[I].first) {
    return entries[I].second;
  } else {
    return find<entries, I+1>(value);
  }
}

int main() {
  BENCHMARK<SIZE, PROBABILITY, UNDERLYING_TYPE, SEED>([]<const auto& entries>(auto value) { return find<entries>(value); });
}

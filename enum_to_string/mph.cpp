//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <benchmark.hpp>
#include <mph>

template <const auto& entries>
constexpr auto find(auto value) -> const char* {
  const auto r = mph::find<entries>(value);
  return r ? *r : "";
}

int main() {
  BENCHMARK<SIZE, PROBABILITY, UNDERLYING_TYPE, SEED>([]<const auto& entries>(auto value) { return find<entries>(value); });
}

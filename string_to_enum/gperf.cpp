//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <benchmark.hpp>
#include "/dev/shm/gperf.hpp"

template <const auto& entries>
constexpr auto find(auto value) -> u32 {
  if (auto ptr = Perfect_Hash::find(value.data(), value.size())) {
   return ptr->value;
  } else {
    return {};
  }
}

int main() {
  BENCHMARK<SIZE, PROBABILITY, SEED>([]<const auto& entries>(auto value) { return find<entries>(value); });
}

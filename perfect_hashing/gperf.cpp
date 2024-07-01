//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <benchmark.hpp>
#include "/dev/shm/gperf.hpp"

template<class T, class TMapped, const auto& entries>
struct find {
  auto operator()(auto value) const -> TMapped {
    if (auto ptr = Perfect_Hash::find(value.data(), value.size())) {
      return ptr->value;
    } else {
      return {};
    }
  }
};

int main() {
  BENCHMARK<SIZE, PROBABILITY, SEED, find>();
}

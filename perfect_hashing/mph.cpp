//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <benchmark.hpp>
#ifndef NTEST
#define NTEST
#endif
#include <mph>

template<const auto& entries>
auto find(auto value) {
  return *mph::find<entries>(value);
}

int main() {
  BENCHMARK<SIZE, PROBABILITY, SEED>([]<const auto& entries>(auto value) { return find<entries>(value); });
}

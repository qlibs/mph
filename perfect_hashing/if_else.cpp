//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <benchmark.hpp>

template<class T, class TMapped, const auto& entries>
struct find {
  template<auto I = 0> auto operator()(auto value) const -> TMapped {
    if constexpr (I == entries.size()) {
      return {};
    } else if (value == entries[I].first) {
      return entries[I].second;
    } else {
      return operator()<I + 1>(value);
    }
  }
};

int main() {
  BENCHMARK<SIZE, PROBABILITY, SEED, find>();
}

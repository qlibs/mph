//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <benchmark.hpp>
#include <unordered_map>

template<class T, class TMapped, const auto& entries>
struct find {
  auto operator()(auto value) const {
    const auto it = map.find(value);
    return it != map.end() ? it->second : decltype(it->second){};
  }
 private:
  std::unordered_map<T, TMapped> map{entries.begin(), entries.end()};
};

int main() {
  BENCHMARK<SIZE, PROBABILITY, SEED, find>();
}

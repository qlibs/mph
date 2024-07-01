//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <benchmark.hpp>
#include <utility>
#include <frozen/unordered_map.h>
#include <frozen/string.h>

template<class T, class TMapped, const auto& entries>
struct find {
  auto operator()(auto value) const {
    const auto it = map.find(value);
    return it != map.end() ? it->second : decltype(it->second){};
  }

 private:
  static constexpr auto map = []<auto... Ns>(std::index_sequence<Ns...>) {
    return frozen::unordered_map<
      std::conditional_t<std::is_same_v<T, std::string_view>, frozen::string, T>, TMapped,
      entries.size()
    >{entries[Ns]...};
  }(std::make_index_sequence<entries.size()>{});
};

int main() {
  BENCHMARK<SIZE, PROBABILITY, SEED, find>();
}

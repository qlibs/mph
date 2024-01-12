//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <array>
#include <iostream>
#include <mph>
#include <string_view>

using std::literals::operator""sv;

class dispatch {
  static constexpr std::array symbols{
      std::pair{"APPL    "sv, 1},
      std::pair{"GOOGL   "sv, 2},
      std::pair{"MSFT    "sv, 3},
  };

  static constexpr auto hash = mph::hash<0, [] { return symbols; }>;

 public:
  constexpr ~dispatch() {
    for (const auto& e : v) {
      std::clog << e << '\n';
    }
  }

  constexpr auto on(const auto data) { ++v[hash(data)]; }

 private:
  std::array<std::size_t, std::size(symbols) + 1>
      v{};  // 0 is special for branchless code continuation
};

int main() {
  dispatch d{};

  const auto on = [&](const auto symbol) {
    d.on(std::span<const char, std::size(symbol) - 1>(
        std::data(symbol), std::data(symbol) + std::size(symbol) - 1));
  };

  on(std::to_array("APPL    "));
  on(std::to_array("GOOGL   "));
  on(std::to_array("MSFT    "));
}

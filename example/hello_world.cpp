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

int main() {
  using std::literals::operator""sv;

  static constexpr std::array symbols{
      "FBC"sv,
      "SPY"sv,
      "CDC"sv,
  };

  auto hash = mph::hash{[] { return symbols; }};

  std::cout << int(hash("F"));    // 0
  std::cout << int(hash("FO"));   // 0
  std::cout << int(hash("FOO"));  // 0

  std::cout << int(hash("FBC"));  // 1
  std::cout << int(hash("SPY"));  // 2
  std::cout << int(hash("CDC"));  // 3
}

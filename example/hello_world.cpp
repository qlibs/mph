//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <mph>
#include <string_view>
#include <array>
#include <iostream>

int main() {
  using std::literals::operator""sv;

  static constexpr std::array symbols{
    "FBC"sv,
    "SPY"sv,
    "CDC"sv,
  };

  auto hash = mph::hash{[] { return symbols; }};

  std::cout << hash("F"sv);   // 0
  std::cout << hash("FO"sv);  // 0
  std::cout << hash("FOO"sv); // 0

  std::cout << hash("FBC"sv); // 1
  std::cout << hash("SPY"sv); // 2
  std::cout << hash("CDC"sv); // 3
}

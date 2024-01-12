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

  enum class color {
    unknown = 0,
    red = 1,
    green = 2,
    blue = 3,
  };

  static constexpr auto colors = std::array{
      std::pair{"red"sv, color::red},
      std::pair{"green"sv, color::green},
      std::pair{"blue"sv, color::blue},
  };

  std::cout << int(
      mph::hash<color::unknown, [] { return colors; }>("green"sv));  // prints 2
}

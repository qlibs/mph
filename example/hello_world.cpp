//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <cassert>
#include <iostream>
#include <mph>

#if (defined(__GNUC__) and not defined(__clang__)) or \
    (defined(__clang__) and (__clang_major__ >= 18))
int main() {
  enum class color { red, green, blue };

  // clang-format off
  auto colors = mph::map<
    {"red",   color::red},
    {"green", color::green},
    {"blue",  color::blue}
  >;
  // clang-format on

  static_assert(color::green == *colors["green"]);
  static_assert(color::red == *colors["red"]);
  static_assert(color::blue == *colors["blue"]);

  assert(colors["green"]);
  std::cout << int(*colors["green"]);  // prints 1
}
#else
int main() {
  enum class color { red, green, blue };

  constexpr auto colors = std::array{
      std::pair{mph::fixed_string{"red"}, color::red},
      std::pair{mph::fixed_string{"green"}, color::green},
      std::pair{mph::fixed_string{"blue"}, color::blue},
  };

  std::cout << int(*mph::hash<colors>("green"));  // prints 1
}
#endif

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

int main() {
  enum class color { red, green, blue };

  constexpr auto colors = std::array{
      mph::pair{"red", color::red},
      mph::pair{"green", color::green},
      mph::pair{"blue", color::blue},
  };

  static_assert(color::green == *mph::hash<colors>("green"));
  static_assert(color::red == *mph::hash<colors>("red"));
  static_assert(color::blue == *mph::hash<colors>("blue"));

  assert(mph::hash<colors>("green"));
  std::cout << int(*mph::hash<colors>("green"));  // prints 1
}

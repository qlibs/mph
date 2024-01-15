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

template <class T>
struct optional {
  constexpr optional() = default;
  constexpr explicit(false) optional(const T v) : value{v}, is_set{true} {}
  [[nodiscard]] constexpr operator bool() const { return is_set; }
  [[nodiscard]] constexpr const auto& operator*() const { return value; }
  T value{};
  bool is_set{};
};

int main() {
  enum class color {
    red,
    green,
    blue,
  };

  constexpr auto colors = std::array{
      std::pair{mph::fixed_string{"red"}, color::red},
      std::pair{mph::fixed_string{"green"}, color::green},
      std::pair{mph::fixed_string{"blue"}, color::blue},
  };

  if (const auto result = mph::hash<optional<color>{}, colors>("foobar");
      result) {
    std::cout << int(*result);
  }
}

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

// clang-format off
constexpr auto policies = []<const auto... ts>(auto &&...args) {
  if constexpr (constexpr auto pext = mph::pext<{.max_bits_size=7u}>{}; requires { pext.template operator()<ts...>( std::forward<decltype(args)>(args)...); }) {
    return pext.template operator()<ts...>(std::forward<decltype(args)>(args)...);
  } else {
    static_assert([](auto &&...) { return false; }(ts...), "hash can't be created with given policies!");
  }
};
// clang-format on

int main() {
  constexpr std::array keys{
      std::pair{1, 0},
      std::pair{100, 1},
      std::pair{1000, 2},
  };

  constexpr auto hash = mph::hash<keys, policies>;

  std::cout << hash(0);   // false
  std::cout << hash(42);  // false

  std::cout << hash(1);   // true
  std::cout << *hash(1);  // 0

  std::cout << hash(100);   // true
  std::cout << *hash(100);  // 1

  std::cout << hash(1000);   // true
  std::cout << *hash(1000);  // 2
}

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

constexpr auto policies = []<const auto unknown, const auto keys, class T>(
    const T data, auto &&...args) {
  if constexpr (constexpr auto pext = mph::pext<2>{};
                requires {
                  pext.template operator()<unknown, keys, T>(
                      data, std::forward<decltype(args)>(args)...);
                }) {
    return pext.template operator()<unknown, keys, T>(
        data, std::forward<decltype(args)>(args)...);
  } else {
    static_assert([](auto &&) { return false; }(keys),
                  "hash can't be created with given policies!");
  }
};

int main() {
  constexpr std::array keys{
      std::pair{1, 0},
      std::pair{100, 1},
      std::pair{1000, 2},
  };

  constexpr auto hash = mph::hash<-1, keys, policies>;

  std::cout << hash(0);   // -1
  std::cout << hash(42);  // -1

  std::cout << hash(1);     // 0
  std::cout << hash(100);   // 1
  std::cout << hash(1000);  // 2
}

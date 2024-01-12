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

constexpr auto policies =
    []<const auto unknown, const auto symbols>(auto &&data, auto &&...args) {
  if (not std::size(data)) {
    return unknown;
  } else if constexpr (constexpr auto pext = mph::pext<2>{};
                       requires {
                         pext.template operator()<unknown, symbols>(
                             data, std::forward<decltype(args)>(args)...);
                       }) {
    return pext.template operator()<unknown, symbols>(
        data, std::forward<decltype(args)>(args)...);
  } else {
    static_assert([](auto &&) { return false; }(symbols),
                  "hash can't be created with given policies!");
  }
};

int main() {
  using std::literals::operator""sv;

  static constexpr std::array symbols{
      std::pair{"FBC"sv, 0},
      std::pair{"SPY"sv, 1},
      std::pair{"CDC"sv, 2},
  };

  constexpr auto hash = mph::hash<-1, [] { return symbols; }, policies>;

  std::cout << hash(""sv);     // -1
  std::cout << hash("FO"sv);   // -1
  std::cout << hash("FOO"sv);  // -1

  std::cout << hash("FBC"sv);  // 0
  std::cout << hash("SPY"sv);  // 1
  std::cout << hash("CDC"sv);  // 2
}

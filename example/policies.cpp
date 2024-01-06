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

constexpr auto dummy_policies = []([[maybe_unused]] const auto symbols, [[maybe_unused]] const auto *data, [[maybe_unused]] const auto size, [[maybe_unused]] auto &&...args) {
  if (not size) {
    return -1;
  } else if constexpr (constexpr auto pext_direct = mph::pext_direct<std::uint64_t, 2>{}; requires { pext_direct(symbols, data, size, std::forward<decltype(args)>(args)...); }) {
    return int(pext_direct(symbols, data, size, std::forward<decltype(args)>(args)...))-1;
  } else {
    static_assert([](auto&&) { return false; }(data), "No luck!");
  }
};

int main() {
  using std::literals::operator""sv;

  static constexpr std::array symbols{
    "FBC"sv,
    "SPY"sv,
    "CDC"sv,
  };

  const auto hash = mph::hash{[] { return symbols; }, dummy_policies};

  std::cout << hash(""sv);    // -1
  std::cout << hash("FO"sv);  // -1
  std::cout << hash("FOO"sv); // -1

  std::cout << hash("FBC"sv); // 0
  std::cout << hash("SPY"sv); // 1
  std::cout << hash("CDC"sv); // 2
}


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
#include <vector>

using std::literals::operator""sv;

class dispatch {
  static constexpr std::array symbols{
      "FBC"sv,
      "SPY"sv,
      "CDC"sv,
  };

  static constexpr auto hash = mph::hash{[] { return symbols; }};

 public:
  ~dispatch() {
    for (const auto& e : v) {
      std::clog << e << '\n';
    }
  }

  auto on(const std::string_view data) { ++v[hash(data)]; }

 private:
  std::vector<std::size_t> v =
      std::vector<std::size_t>(std::size(symbols) + 1);  // 0 is special for branchless code continuation
};

int main() {
  dispatch d{};
  d.on("FBC");  // 1: 1
  d.on("SPY");  // 2: 1
  d.on("CDC");  // 3: 1
  d.on("XXX");  // 0: 1
  d.on("Y");    // 0: 2
  d.on("SPY");  // 2: 2
}

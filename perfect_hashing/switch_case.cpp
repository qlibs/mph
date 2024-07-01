//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <benchmark.hpp>

template<class T, class TMapped, const auto& entries>
struct find {
  auto operator()(auto value) const {
    const auto switch_case = [value]<auto I = 0>(auto self) {
      if constexpr (I == entries.size()) {
        return decltype(entries[0].second){};
      } else {
        switch (value) {
          default: return self.template operator()<I + 1>(self);
          case entries[I].first: return entries[I].second;
        }
      }
    };
    return switch_case(switch_case);
  }
};

//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <mph>

#include "ut.hpp"

int main() {
  using namespace boost::ut;
  using std::literals::operator""sv;

  constexpr auto verify = [](const auto &symbols, const auto &hash) {
    for (auto expected = 1; const auto &[symbol, _] : symbols) {
      expect(_i(expected++) == hash(symbol));
    }
  };

  "[hash] enum"_test = [] {
    enum class color {
      unknown,
      red,
      green,
      blue,
    };

    static constexpr auto colors = std::array{
        std::pair{"red"sv, color::red},
        std::pair{"green"sv, color::green},
        std::pair{"blue"sv, color::blue},
    };

    constexpr auto hash = mph::hash<color::unknown, [] { return colors; }>;

    expect(color::red == hash("red"sv));
    expect(color::green == hash("green"sv));
    expect(color::blue == hash("blue"sv));
    expect(color::unknown == hash(""sv));
    expect(color::unknown == hash("D"sv));
    expect(color::unknown == hash("a"sv));
    expect(color::unknown == hash("b"sv));
  };

  "[hash] policies"_test = [verify] {
    static constexpr std::array symbols{
        std::pair{"A"sv, 1},
        std::pair{"B"sv, 2},
        std::pair{"C"sv, 3},
    };

    constexpr auto hash = mph::hash<0, [] { return symbols; }, mph::policies>;

    verify(symbols, hash);

    expect(0_i == hash(""sv));
    expect(0_i == hash("D"sv));
    expect(0_i == hash("a"sv));
    expect(0_i == hash("b"sv));
  };

  "[hash] custom policies - pext"_test = [verify] {
    static constexpr std::array symbols{
        std::pair{"A"sv, 1},
        std::pair{"B"sv, 2},
        std::pair{"C"sv, 3},
    };

    constexpr auto hash = mph::hash < 0, [] { return symbols; },
                   []<const auto... ts>(auto &&...args) {
      return mph::pext<5>{}.template operator()<ts...>(
          std::forward<decltype(args)>(args)...);
    }
    > ;

    verify(symbols, hash);

    expect(0_i == hash(""sv));
    expect(0_i == hash("D"sv));
    expect(0_i == hash("a"sv));
    expect(0_i == hash("b"sv));
  };

  "[hash] custom policies - pext_split"_test = [verify] {
    static constexpr std::array symbols{
        std::pair{"A"sv, 1},
        std::pair{"B"sv, 2},
        std::pair{"C"sv, 3},
    };

    constexpr auto hash = mph::hash < 0, [] { return symbols; },
                   []<const auto... ts>(auto &&...args) {
      return mph::pext_split<5, 0u>{}.template operator()<ts...>(
          std::forward<decltype(args)>(args)...);
    }
    > ;

    verify(symbols, hash);

    expect(0_i == hash("D"sv));
    expect(0_i == hash("a"sv));
    expect(0_i == hash("b"sv));
  };

  "[hash] custom policies - pext_split<N>"_test = [verify] {
    static constexpr std::array symbols{
        std::pair{"AAPL    "sv, 1}, std::pair{"AMZN    "sv, 2},
        std::pair{"GOOGL   "sv, 3}, std::pair{"MSFT    "sv, 4},
        std::pair{"NVDA    "sv, 5},
    };

    constexpr auto hash = mph::hash < 0, [] { return symbols; },
                   []<const auto... ts>(auto &&...args) {
      return mph::pext_split<7, 0u>{}.template operator()<ts...>(
          std::forward<decltype(args)>(args)...);
    }
    > ;

    verify(symbols, hash);
  };

  "[hash] custom policies - pext_split<len(N)-1>"_test = [verify] {
    static constexpr std::array symbols{
        std::pair{"    AAPL"sv, 1}, std::pair{"    AMZN"sv, 2},
        std::pair{"   GOOGL"sv, 3}, std::pair{"    MSFT"sv, 4},
        std::pair{"    NVDA"sv, 5},
    };

    constexpr auto hash = mph::hash < 0, [] { return symbols; },
                   []<const auto... ts>(auto &&...args) {
      return mph::pext_split<7, 7u>{}.template operator()<ts...>(
          std::forward<decltype(args)>(args)...);
    }
    > ;

    verify(symbols, hash);
  };

  "[hash] custom policies - swar32"_test = [verify] {
    static constexpr std::array symbols{
        std::pair{"A"sv, 1},
        std::pair{"B"sv, 2},
        std::pair{"C"sv, 3},
    };

    constexpr auto hash = mph::hash < 0, [] { return symbols; },
                   []<const auto... ts>(auto &&...args) {
      return mph::swar<std::uint32_t>{}.template operator()<ts...>(
          std::forward<decltype(args)>(args)...);
    }
    > ;

    verify(symbols, hash);

    expect(0_i == hash("D"sv));
    expect(0_i == hash("a"sv));
    expect(0_i == hash("b"sv));
  };

  "[hash] custom policies - swar64"_test = [verify] {
    static constexpr std::array symbols{
        std::pair{"foobar"sv, 1}, std::pair{"bar"sv, 2}, std::pair{"foo"sv, 3}};

    constexpr auto hash = mph::hash < 0, [] { return symbols; },
                   []<const auto... ts>(auto &&...args) {
      return mph::swar<std::uint64_t>{}.template operator()<ts...>(
          std::forward<decltype(args)>(args)...);
    }
    > ;

    verify(symbols, hash);

    expect(0_i == hash(""sv));
    expect(0_i == hash("xxx"sv));
    expect(0_i == hash("baz"sv));
  };

  "[hash] custom policies - swar64 / std::span"_test = [verify] {
    static constexpr std::array symbols{
        std::pair{"A       "sv, 1},
        std::pair{"B       "sv, 2},
        std::pair{"C       "sv, 3},
    };

    constexpr auto size = std::size(symbols[0].first);
    constexpr auto hash = mph::hash < 0, [] { return symbols; },
                   []<const auto... ts>(auto &&...args) {
      return mph::swar<std::uint64_t>{}.template operator()<ts...>(
          std::forward<decltype(args)>(args)...);
    }
    > ;

    verify(symbols, hash);

    expect(0_i == hash(""sv));
    expect(0_i == hash("D "sv));
    expect(0_i == hash(" D"sv));
    expect(0_i == hash(" D "sv));
    expect(0_i == hash("E"sv));
    expect(0_i == hash("F"sv));
    expect(0_i == hash(std::span<const char>("        ", size)));
    expect(0_i == hash(std::span<const char>("D       ", size)));
    expect(0_i == hash(std::span<const char>("E       ", size)));
    expect(0_i == hash(std::span<const char>("F       ", size)));
  };

  "[hash] std::span data"_test = [verify] {
    static constexpr std::array symbols{
        std::pair{"A       "sv, 1},
        std::pair{"B       "sv, 2},
        std::pair{"C       "sv, 3},
    };

    constexpr auto size = std::size(symbols[0].first);

    auto hash = mph::hash<0, [] { return symbols; }>;

    verify(symbols, hash);

    expect(0_i == hash(""sv));
    expect(0_i == hash("D "sv));
    expect(0_i == hash(" D"sv));
    expect(0_i == hash(" D "sv));
    expect(0_i == hash("E"sv));
    expect(0_i == hash("F"sv));
    expect(0_i == hash(std::span<const char>("        ", size)));
    expect(0_i == hash(std::span<const char>("D       ", size)));
    expect(0_i == hash(std::span<const char>("E       ", size)));
    expect(0_i == hash(std::span<const char>("F       ", size)));
  };

  "[hash] std::span variable length"_test = [verify] {
    static constexpr std::array symbols{
        std::pair{std::span<const char, 8>("enter  "), 1},
        std::pair{std::span<const char, 8>("delete "), 2},
        std::pair{std::span<const char, 8>("esc    "), 3},
    };

    const auto hash = mph::hash<0, [] { return symbols; }>;

    verify(symbols, hash);

    expect(0_i == hash(std::span("")));
    expect(0_i == hash(std::span("  ")));
    expect(0_i == hash(std::span("    ")));
    expect(0_i == hash(std::span("stop")));
    expect(0_i == hash(std::span("start")));
    expect(0_i == hash(std::span("foobar")));
    expect(0_i == hash(std::span("1234567")));
  };

  "[hash] std::array"_test = [verify] {
    static constexpr std::array symbols{
        std::pair{std::array{'A', 'a'}, 1},
        std::pair{std::array{'B', 'b'}, 2},
        std::pair{std::array{'C', 'c'}, 3},
    };

    const auto hash = mph::hash<0, [] { return symbols; }>;

    verify(symbols, hash);

    expect(0_i == hash(std::array{'A'}));
    expect(0_i == hash(std::array{'X'}));
    expect(0_i == hash(std::array{'A', 'A'}));
    expect(0_i == hash(std::array{'B', 'c'}));
    expect(0_i == hash(std::array{'C', 'b'}));
    expect(0_i == hash(std::array{'C', 'b', 'a'}));
  };

  "[hash] std::string_view"_test = [verify] {
    static constexpr std::array symbols{
        std::pair{"AA "sv, 1},
        std::pair{"BB "sv, 2},
        std::pair{"CC "sv, 3},
    };

    auto hash = mph::hash<0, [] { return symbols; }>;

    verify(symbols, hash);

    expect(0_i == hash(""sv));
    expect(0_i == hash("   "sv));
    expect(0_i == hash("aa "sv));
    expect(0_i == hash("aaa"sv));
    expect(0_i == hash("bb"sv));
    expect(0_i == hash("bb "sv));
    expect(0_i == hash(" cc"sv));
  };

  "[hash] fail case - variable length"_test = [verify] {
    static constexpr std::array symbols{
        std::pair{" AA "sv, 1},
        std::pair{" AB "sv, 2},
        std::pair{" AC "sv, 3},
    };

    auto hash = mph::hash<0, [] { return symbols; }>;

    verify(symbols, hash);

    expect(0_i == hash(""sv));
    expect(0_i == hash(" aa "sv));
    expect(0_i == hash("aaaa"sv));
    expect(0_i == hash(" AA"sv));
    expect(0_i == hash("AA "sv));
    expect(0_i == hash(" AA_"sv));
    expect(0_i == hash("_AA_"sv));
  };

  "[hash] variable length"_test = [verify] {
    static constexpr std::array<std::pair<std::string_view, int>, 6> symbols{
        {{"ftp", 1},
         {"file", 2},
         {"http", 3},
         {"https", 4},
         {"ws", 5},
         {"wss", 6}}};
    static constexpr auto hash = mph::hash<0, [] { return symbols; }>;

    verify(symbols, hash);

    expect(0_i == hash(""sv));
    expect(0_i == hash("udp"sv));
    expect(0_i == hash("HTTP"sv));
    expect(0_i == hash("http2"sv));
    expect(0_i == hash("https!"sv));
  };

  "[hash] multiple policies trigger"_test = [verify] {
    static constexpr std::array<std::pair<std::string_view, std::uint8_t>, 100>
        symbols{
            std::pair{"III     "sv, 1},  std::pair{"AGM-C   "sv, 2},
            std::pair{"LOPE    "sv, 3},  std::pair{"FEMS    "sv, 4},
            std::pair{"IEA     "sv, 5},  std::pair{"VYMI    "sv, 6},
            std::pair{"BHK     "sv, 7},  std::pair{"SIEB    "sv, 8},
            std::pair{"DGBP    "sv, 9},  std::pair{"INFN    "sv, 10},
            std::pair{"USRT    "sv, 11}, std::pair{"BCOR    "sv, 12},
            std::pair{"TWM     "sv, 13}, std::pair{"BVSN    "sv, 14},
            std::pair{"STBA    "sv, 15}, std::pair{"GPK     "sv, 16},
            std::pair{"LVHD    "sv, 17}, std::pair{"FTEK    "sv, 18},
            std::pair{"GLBS    "sv, 19}, std::pair{"CUBB    "sv, 20},
            std::pair{"LRCX    "sv, 21}, std::pair{"HTGM    "sv, 22},
            std::pair{"RYN     "sv, 23}, std::pair{"IPG     "sv, 24},
            std::pair{"PNNTG   "sv, 25}, std::pair{"ZIG     "sv, 26},
            std::pair{"IVR-A   "sv, 27}, std::pair{"INVA    "sv, 28},
            std::pair{"MNE     "sv, 29}, std::pair{"KRA     "sv, 30},
            std::pair{"BRMK    "sv, 31}, std::pair{"ARKG    "sv, 32},
            std::pair{"FFR     "sv, 33}, std::pair{"QTRX    "sv, 34},
            std::pair{"XTN     "sv, 35}, std::pair{"BAC-A   "sv, 36},
            std::pair{"CYBE    "sv, 37}, std::pair{"ETJ     "sv, 38},
            std::pair{"JHCS    "sv, 39}, std::pair{"RBCAA   "sv, 40},
            std::pair{"GDS     "sv, 41}, std::pair{"WTID    "sv, 42},
            std::pair{"TCO     "sv, 43}, std::pair{"BWA     "sv, 44},
            std::pair{"MIE     "sv, 45}, std::pair{"GENY    "sv, 46},
            std::pair{"TDOC    "sv, 47}, std::pair{"MCRO    "sv, 48},
            std::pair{"QFIN    "sv, 49}, std::pair{"NBTB    "sv, 50},
            std::pair{"PWC     "sv, 51}, std::pair{"FQAL    "sv, 52},
            std::pair{"NJAN    "sv, 53}, std::pair{"IWB     "sv, 54},
            std::pair{"GXGXW   "sv, 55}, std::pair{"EDUC    "sv, 56},
            std::pair{"RETL    "sv, 57}, std::pair{"VIACA   "sv, 58},
            std::pair{"KLDO    "sv, 59}, std::pair{"NEE-I   "sv, 60},
            std::pair{"FBC     "sv, 61}, std::pair{"JW.A    "sv, 62},
            std::pair{"BSMX    "sv, 63}, std::pair{"FMNB    "sv, 64},
            std::pair{"EXR     "sv, 65}, std::pair{"TAC     "sv, 66},
            std::pair{"FDL     "sv, 67}, std::pair{"SWIR    "sv, 68},
            std::pair{"CLWT    "sv, 69}, std::pair{"LMHB    "sv, 70},
            std::pair{"IRTC    "sv, 71}, std::pair{"CDMO    "sv, 72},
            std::pair{"HMLP-A  "sv, 73}, std::pair{"LVUS    "sv, 74},
            std::pair{"UMRX    "sv, 75}, std::pair{"GJH     "sv, 76},
            std::pair{"FRME    "sv, 77}, std::pair{"CEIX    "sv, 78},
            std::pair{"IHD     "sv, 79}, std::pair{"GHSI    "sv, 80},
            std::pair{"DCP-B   "sv, 81}, std::pair{"SB      "sv, 82},
            std::pair{"DSE     "sv, 83}, std::pair{"CPRT    "sv, 84},
            std::pair{"NRZ     "sv, 85}, std::pair{"VLYPO   "sv, 86},
            std::pair{"TDAC    "sv, 87}, std::pair{"ZXZZT   "sv, 88},
            std::pair{"IWX     "sv, 89}, std::pair{"NCSM    "sv, 90},
            std::pair{"WIRE    "sv, 91}, std::pair{"SFST    "sv, 92},
            std::pair{"EWD     "sv, 93}, std::pair{"DEACW   "sv, 94},
            std::pair{"TRPX    "sv, 95}, std::pair{"UCTT    "sv, 96},
            std::pair{"ZAZZT   "sv, 97}, std::pair{"CYD     "sv, 98},
            std::pair{"NURE    "sv, 99}, std::pair{"WEAT    "sv, 100},
        };

    const auto hash = mph::hash<std::uint8_t{}, [] { return symbols; }>;

    verify(symbols, hash);

    expect(0_u == hash("        "sv));
    expect(0_u == hash(" III    "sv));
    expect(0_u == hash("  III   "sv));
    expect(0_u == hash("   III  "sv));
    expect(0_u == hash("    III "sv));
    expect(0_u == hash("     III"sv));
  };
}

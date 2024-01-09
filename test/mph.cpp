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

  "[hash] default policies"_test = [] {
    static constexpr std::array symbols{
        "A"sv,
        "B"sv,
        "C"sv,
    };

    constexpr auto hash = mph::hash{[] { return symbols; }, mph::default_policies};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(symbol));
    }

    expect(0_u == hash(""sv));
    expect(0_u == hash("D"sv));
    expect(0_u == hash("a"sv));
    expect(0_u == hash("b"sv));
  };

  "[hash] custom policies - pext"_test = [] {
    static constexpr std::array symbols{
        "A"sv,
        "B"sv,
        "C"sv,
    };

    constexpr auto hash = mph::hash{[] { return symbols; }, [](auto &&...args) { return mph::pext<5>{}(args...); }};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(symbol));
    }

    expect(0_u == hash(""sv));
    expect(0_u == hash("D"sv));
    expect(0_u == hash("a"sv));
    expect(0_u == hash("b"sv));
  };

  "[hash] custom policies - pext_split"_test = [] {
    static constexpr std::array symbols{
        "A"sv,
        "B"sv,
        "C"sv,
    };

    constexpr auto hash = mph::hash{[] { return symbols; }, [](auto &&...args) { return mph::pext_split<5>{}(args...); }};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(symbol));
    }

    expect(0_u == hash("D"sv));
    expect(0_u == hash("a"sv));
    expect(0_u == hash("b"sv));
  };

  "[hash] custom policies - swar32"_test = [] {
    static constexpr std::array symbols{
        "A"sv,
        "B"sv,
        "C"sv,
    };

    constexpr auto hash = mph::hash{[] { return symbols; }, [](auto &&...args) { return mph::swar<std::uint32_t>{}(args...); }};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(symbol));
    }

    expect(0_u == hash("D"sv));
    expect(0_u == hash("a"sv));
    expect(0_u == hash("b"sv));
  };

  "[hash] custom policies - swar64"_test = [] {
    static constexpr std::array symbols{
        "foobar"sv,
        "bar"sv,
        "foo"sv,
    };

    constexpr auto hash = mph::hash{[] { return symbols; }, [](auto &&...args) { return mph::swar<std::uint64_t>{}(args...); }};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(symbol));
    }

    expect(0_u == hash(""sv));
    expect(0_u == hash("xxx"sv));
    expect(0_u == hash("baz"sv));
  };

  "[hash] custom policies - swar64 / std::span"_test = [] {
    static constexpr std::array symbols{
        "A       "sv,
        "B       "sv,
        "C       "sv,
    };

    constexpr auto size = std::size(symbols[0]);

    auto hash = mph::hash{[] { return symbols; }, [](auto &&...args) { return mph::swar<std::uint64_t>{}(args...); }};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(std::span<const char, size>(std::data(symbol), std::data(symbol) + size)));
    }

    expect(0_u == hash(""sv));
    expect(0_u == hash("D "sv));
    expect(0_u == hash(" D"sv));
    expect(0_u == hash(" D "sv));
    expect(0_u == hash("E"sv));
    expect(0_u == hash("F"sv));
    expect(0_u == hash(std::span<const char>("        ", size)));
    expect(0_u == hash(std::span<const char>("D       ", size)));
    expect(0_u == hash(std::span<const char>("E       ", size)));
    expect(0_u == hash(std::span<const char>("F       ", size)));
  };

  "[hash] std::span data"_test = [] {
    static constexpr std::array symbols{
        "A       "sv,
        "B       "sv,
        "C       "sv,
    };

    constexpr auto size = std::size(symbols[0]);

    auto hash = mph::hash{[] { return symbols; }};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(std::span<const char, size>(std::data(symbol), std::data(symbol) + size)));
    }

    expect(0_u == hash(""sv));
    expect(0_u == hash("D "sv));
    expect(0_u == hash(" D"sv));
    expect(0_u == hash(" D "sv));
    expect(0_u == hash("E"sv));
    expect(0_u == hash("F"sv));
    expect(0_u == hash(std::span<const char>("        ", size)));
    expect(0_u == hash(std::span<const char>("D       ", size)));
    expect(0_u == hash(std::span<const char>("E       ", size)));
    expect(0_u == hash(std::span<const char>("F       ", size)));
  };

  "[hash] std::span variable length"_test = [] {
    static constexpr std::array symbols{
        std::span<const char, 8>("enter  "),
        std::span<const char, 8>("delete "),
        std::span<const char, 8>("esc    "),
    };

    const auto hash = mph::hash{[] { return symbols; }};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(symbol));
    }

    expect(0_u == hash(std::span("")));
    expect(0_u == hash(std::span("  ")));
    expect(0_u == hash(std::span("    ")));
    expect(0_u == hash(std::span("stop")));
    expect(0_u == hash(std::span("start")));
    expect(0_u == hash(std::span("foobar")));
    expect(0_u == hash(std::span("1234567")));
  };

  "[hash] std::array"_test = [] {
    static constexpr std::array symbols{
        std::array{'A', 'a'},
        std::array{'B', 'b'},
        std::array{'C', 'c'},
    };

    const auto hash = mph::hash{[] { return symbols; }};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(symbol));
    }

    expect(0_u == hash(std::array{'A'}));
    expect(0_u == hash(std::array{'X'}));
    expect(0_u == hash(std::array{'A', 'A'}));
    expect(0_u == hash(std::array{'B', 'c'}));
    expect(0_u == hash(std::array{'C', 'b'}));
    expect(0_u == hash(std::array{'C', 'b', 'a'}));
  };

  "[hash] std::string_view"_test = [] {
    static constexpr std::array symbols{
        "AA "sv,
        "BB "sv,
        "CC "sv,
    };

    auto hash = mph::hash{[] { return symbols; }};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(symbol));
    }

    expect(0_u == hash(""sv));
    expect(0_u == hash("   "sv));
    expect(0_u == hash("aa "sv));
    expect(0_u == hash("aaa"sv));
    expect(0_u == hash("bb"sv));
    expect(0_u == hash("bb "sv));
    expect(0_u == hash(" cc"sv));
  };

  "[hash] fail case - different sizes"_test = [] {
    static constexpr std::array symbols{
        " AA "sv,
        " AB "sv,
        " AC "sv,
    };

    auto hash = mph::hash{[] { return symbols; }};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(symbol));
    }

    expect(0_u == hash(""sv));
    expect(0_u == hash(" aa "sv));
    expect(0_u == hash("aaaa"sv));
    expect(0_u == hash(" AA"sv));
    expect(0_u == hash("AA "sv));
    expect(0_u == hash(" AA_"sv));
    expect(0_u == hash("_AA_"sv));
  };

  "[hash] different sizes"_test = [] {
    static constexpr std::array<std::string_view, 6> symbols{"ftp", "file", "http", "https", "ws", "wss"};

    constexpr auto hash = mph::hash{[] { return symbols; }};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(symbol));
    }

    expect(0_u == hash(""sv));
    expect(0_u == hash("udp"sv));
    expect(0_u == hash("HTTP"sv));
    expect(0_u == hash("http2"sv));
    expect(0_u == hash("https!"sv));
  };

  "[hash] multiple policies trigger"_test = [] {
    static constexpr std::array<std::string_view, 100> symbols{
        "III     ", "AGM-C   ", "LOPE    ", "FEMS    ", "IEA     ", "VYMI    ", "BHK     ", "SIEB    ", "DGBP    ", "INFN    ",
        "USRT    ", "BCOR    ", "TWM     ", "BVSN    ", "STBA    ", "GPK     ", "LVHD    ", "FTEK    ", "GLBS    ", "CUBB    ",
        "LRCX    ", "HTGM    ", "RYN     ", "IPG     ", "PNNTG   ", "ZIG     ", "IVR-A   ", "INVA    ", "MNE     ", "KRA     ",
        "BRMK    ", "ARKG    ", "FFR     ", "QTRX    ", "XTN     ", "BAC-A   ", "CYBE    ", "ETJ     ", "JHCS    ", "RBCAA   ",
        "GDS     ", "WTID    ", "TCO     ", "BWA     ", "MIE     ", "GENY    ", "TDOC    ", "MCRO    ", "QFIN    ", "NBTB    ",
        "PWC     ", "FQAL    ", "NJAN    ", "IWB     ", "GXGXW   ", "EDUC    ", "RETL    ", "VIACA   ", "KLDO    ", "NEE-I   ",
        "FBC     ", "JW.A    ", "BSMX    ", "FMNB    ", "EXR     ", "TAC     ", "FDL     ", "SWIR    ", "CLWT    ", "LMHB    ",
        "IRTC    ", "CDMO    ", "HMLP-A  ", "LVUS    ", "UMRX    ", "GJH     ", "FRME    ", "CEIX    ", "IHD     ", "GHSI    ",
        "DCP-B   ", "SB      ", "DSE     ", "CPRT    ", "NRZ     ", "VLYPO   ", "TDAC    ", "ZXZZT   ", "IWX     ", "NCSM    ",
        "WIRE    ", "SFST    ", "EWD     ", "DEACW   ", "TRPX    ", "UCTT    ", "ZAZZT   ", "CYD     ", "NURE    ", "WEAT    ",
    };

    const auto hash = mph::hash{[] { return symbols; }};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(symbol));
    }

    expect(0_u == hash("        "sv));
    expect(0_u == hash(" III    "sv));
    expect(0_u == hash("  III   "sv));
    expect(0_u == hash("   III  "sv));
    expect(0_u == hash("    III "sv));
    expect(0_u == hash("     III"sv));
  };
}

//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <mph>
#include <ut.hpp>

int main() {
  using namespace boost::ut;
  using std::literals::operator""sv;

  ""_test = [] {
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

  ""_test = [] {
    static constexpr std::array symbols{
        "A"sv,
        "B"sv,
        "C"sv,
    };

    constexpr auto hash = mph::hash{[] { return symbols; }, [](auto&&... args) { return mph::pext_direct<5>{}(args...); }};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(symbol));
    }

    expect(0_u == hash(""sv));
    expect(0_u == hash("D"sv));
    expect(0_u == hash("a"sv));
    expect(0_u == hash("b"sv));
  };


  ""_test = [] {
    static constexpr std::array symbols{
        "A"sv,
        "B"sv,
        "C"sv,
    };

    constexpr auto hash = mph::hash{[] { return symbols; }, [](auto&&... args) { return mph::pext_split_on_first_char<5>{}(args...); }};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(symbol));
    }

    expect(0_u == hash("D"sv));
    expect(0_u == hash("a"sv));
    expect(0_u == hash("b"sv));
  };

  ""_test = [] {
    static constexpr std::array symbols{
        "A       "sv,
        "B       "sv,
        "C       "sv,
    };

    auto hash = mph::hash{[] { return symbols; }};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(symbol.data())); // no size information
    }

    expect(0_u == hash("        ")); // no size information
    expect(0_u == hash("D       ")); // no size information
    expect(0_u == hash("E       ")); // no size information
    expect(0_u == hash("F       ")); // no size information
  };

  ""_test = [] {
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

  ""_test = [] {
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

  ""_test = [] {
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
      expect(_u(expected++) == hash(symbol.data()));
    }

    expect(0_u == hash("        "sv));
    expect(0_u == hash(" III    "sv));
    expect(0_u == hash("  III   "sv));
    expect(0_u == hash("   III  "sv));
    expect(0_u == hash("    III "sv));
    expect(0_u == hash("     III"sv));
  };
}

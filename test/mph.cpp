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

    constexpr auto hash = mph::hash{[] { return symbols; }};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(symbol));
    }

    expect(0_u == hash(""));
    expect(0_u == hash("D"));
    expect(0_u == hash("a"));
    expect(0_u == hash("b"));
  };

  ""_test = [] {
    static constexpr std::array symbols{
        "A"sv,
        "B"sv,
        "C"sv,
    };

    auto hash = mph::hash{[] { return symbols; }};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(symbol.data()));
    }

    expect(0_u == hash(""));
    expect(0_u == hash("D"));
    expect(0_u == hash("a"));
    expect(0_u == hash("b"));
  };

  ""_test = [] {
    static constexpr std::array symbols{
        "AA "sv,
        "BB "sv,
        "CC "sv,
    };

    auto hash = mph::hash{[] { return symbols; }};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(symbol.data()));
    }

    expect(0_u == hash(""));
    expect(0_u == hash("   "));
    expect(0_u == hash("aa "));
    expect(0_u == hash("aaa"));
    expect(0_u == hash("bb"));
    expect(0_u == hash("bb "));
    expect(0_u == hash(" cc"));
  };

  ""_test = [] {
    static constexpr std::array symbols{
        " AA "sv,
        " AB "sv,
        " AC "sv,
    };

    auto hash = mph::hash{[] { return symbols; }};

    for (auto expected = 1u; const auto &symbol : symbols) {
      expect(_u(expected++) == hash(symbol.data()));
    }

    expect(0_u == hash(""));
    expect(0_u == hash(" aa "));
    expect(0_u == hash("aaaa"));
    expect(0_u == hash(" AA"));
    expect(0_u == hash("AA "));
    expect(0_u == hash(" AA_"));
    expect(0_u == hash("_AA_"));
  };

  //""_test = [] {
    //static constexpr std::array<std::string_view, 100> symbols{
      //"III     ", "AGM-C   ", "LOPE    ", "FEMS    ", "IEA     ", "VYMI    ", "BHK     ", "SIEB    ", "DGBP    ", "INFN    ",
      //"USRT    ", "BCOR    ", "TWM     ", "BVSN    ", "STBA    ", "GPK     ", "LVHD    ", "FTEK    ", "GLBS    ", "CUBB    ",
      //"LRCX    ", "HTGM    ", "RYN     ", "IPG     ", "PNNTG   ", "ZIG     ", "IVR-A   ", "INVA    ", "MNE     ", "KRA     ",
      //"BRMK    ", "ARKG    ", "FFR     ", "QTRX    ", "XTN     ", "BAC-A   ", "CYBE    ", "ETJ     ", "JHCS    ", "RBCAA   ",
      //"GDS     ", "WTID    ", "TCO     ", "BWA     ", "MIE     ", "GENY    ", "TDOC    ", "MCRO    ", "QFIN    ", "NBTB    ",
      //"PWC     ", "FQAL    ", "NJAN    ", "IWB     ", "GXGXW   ", "EDUC    ", "RETL    ", "VIACA   ", "KLDO    ", "NEE-I   ",
      //"FBC     ", "JW.A    ", "BSMX    ", "FMNB    ", "EXR     ", "TAC     ", "FDL     ", "SWIR    ", "CLWT    ", "LMHB    ",
      //"IRTC    ", "CDMO    ", "HMLP-A  ", "LVUS    ", "UMRX    ", "GJH     ", "FRME    ", "CEIX    ", "IHD     ", "GHSI    ",
      //"DCP-B   ", "SB      ", "DSE     ", "CPRT    ", "NRZ     ", "VLYPO   ", "TDAC    ", "ZXZZT   ", "IWX     ", "NCSM    ",
      //"WIRE    ", "SFST    ", "EWD     ", "DEACW   ", "TRPX    ", "UCTT    ", "ZAZZT   ", "CYD     ", "NURE    ", "WEAT    ",
    //};

    //auto hash = mph::hash{[] { return symbols; }};

    //auto expected = 1u;
    //for (const auto &symbol : symbols) {
      //expect(_u(expected++) == hash(symbol.data()));
    //}

    //expect(0_u == hash(""));
    //expect(0_u == hash("        "));
    //expect(0_u == hash("III "));
    //expect(0_u == hash("III  "));
    //expect(0_u == hash("III   "));
    //expect(0_u == hash("III    "));
    //expect(0_u == hash("III     "));
    //expect(0_u == hash(" III    "));
    //expect(0_u == hash("IIIx    "));
    //expect(0_u == hash("     III"));
  //};
}

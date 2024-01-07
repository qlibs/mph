//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <array>
#include <string_view>

namespace data::inline v_1_0_0 {
constexpr std::array<std::string_view, 5> random_5_len_4{"FXB ", "CODI", "DDMX", "MOHO", "SPCB"};
constexpr std::array<std::string_view, 5> random_5_len_8{"FXB     ", "CODI    ", "DDMX    ", "MOHO    ", "SPCB    "};
constexpr std::array<std::string_view, 6> random_6_len_3_5{"ftp", "file", "http", "https", "ws", "wss"};
constexpr std::array<std::string_view, 100> random_100_len_8{
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
}  // namespace data::inline v_1_0_0

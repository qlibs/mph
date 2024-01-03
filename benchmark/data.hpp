//
// Copyright (c) 2024 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#if __has_include(<frozen/unordered_map.h>) and __has_include(<frozen/string.h>)
#define FROZEN_LETITGO_HAS_STRING_VIEW
#include <frozen/string.h>
#include <frozen/unordered_map.h>
#endif

#include <array>
#include <string_view>

namespace data {
constexpr const std::array<std::string_view, 100> all{
    "III     ", "AGM-C   ", "LOPE    ", "FEMS    ", "IEA     ", "VYMI    ",
    "BHK     ", "SIEB    ", "DGBP    ", "INFN    ", "USRT    ", "BCOR    ",
    "TWM     ", "BVSN    ", "STBA    ", "GPK     ", "LVHD    ", "FTEK    ",
    "GLBS    ", "CUBB    ", "LRCX    ", "HTGM    ", "RYN     ", "IPG     ",
    "PNNTG   ", "ZIG     ", "IVR-A   ", "INVA    ", "MNE     ", "KRA     ",
    "BRMK    ", "ARKG    ", "FFR     ", "QTRX    ", "XTN     ", "BAC-A   ",
    "CYBE    ", "ETJ     ", "JHCS    ", "RBCAA   ", "GDS     ", "WTID    ",
    "TCO     ", "BWA     ", "MIE     ", "GENY    ", "TDOC    ", "MCRO    ",
    "QFIN    ", "NBTB    ", "PWC     ", "FQAL    ", "NJAN    ", "IWB     ",
    "GXGXW   ", "EDUC    ", "RETL    ", "VIACA   ", "KLDO    ", "NEE-I   ",
    "FBC     ", "JW.A    ", "BSMX    ", "FMNB    ", "EXR     ", "TAC     ",
    "FDL     ", "SWIR    ", "CLWT    ", "LMHB    ", "IRTC    ", "CDMO    ",
    "HMLP-A  ", "LVUS    ", "UMRX    ", "GJH     ", "FRME    ", "CEIX    ",
    "IHD     ", "GHSI    ", "DCP-B   ", "SB      ", "DSE     ", "CPRT    ",
    "NRZ     ", "VLYPO   ", "TDAC    ", "ZXZZT   ", "IWX     ", "NCSM    ",
    "WIRE    ", "SFST    ", "EWD     ", "DEACW   ", "TRPX    ", "UCTT    ",
    "ZAZZT   ", "CYD     ", "NURE    ", "WEAT    ",
};

constexpr const std::array<std::string_view, 100> random{
    "FXB     ", "CODI    ", "DDMX    ", "MOHO    ", "SPCB    ", "XXII    ",
    "MTC     ", "DWAT    ", "ETSY    ", "FUND    ", "SPXE    ", "PLXS    ",
    "IAT     ", "APT     ", "BANX    ", "MWK     ", "LBC     ", "HCCH    ",
    "FJNK    ", "SSNC    ", "EQWL    ", "PCF     ", "AIG     ", "SPOK    ",
    "CELC    ", "RESN    ", "TAXF    ", "VSEC    ", "PEN     ", "PSB-W   ",
    "GECCM   ", "QEP     ", "TRS     ", "CUK     ", "FAN     ", "AYR     ",
    "VEGN    ", "TOTAR   ", "AGO-F   ", "SPLV    ", "EFA     ", "MOR     ",
    "LPT     ", "RHE     ", "SFL     ", "TSLF    ", "PACK+   ", "VGLT    ",
    "GVP     ", "ONTO    ", "VRTV    ", "TZOO    ", "SLY     ", "XLF     ",
    "DIAL    ", "JHMF    ", "XYF     ", "EVFM    ", "LCUT    ", "MUH     ",
    "BFY     ", "FNGU    ", "ETI-    ", "IWD     ", "CRC     ", "WERN    ",
    "NZF     ", "CTEK    ", "FITE    ", "LTHM    ", "TESS    ", "ORSN    ",
    "CDR-B   ", "KEY-K   ", "WDAY    ", "WLH     ", "FORM    ", "KORP    ",
    "ROG     ", "SCHA    ", "JAZZ    ", "FLAX    ", "LTBR    ", "SOHU    ",
    "IBDD    ", "FBND    ", "OAC=    ", "AGFSW   ", "IGLB    ", "MOS     ",
    "PLAG    ", "NPTN    ", "VG      ", "DO      ", "SMCI    ", "PACQ    ",
    "CDC     ", "OPESW   ", "EXP     ", "NCTY    ",
};

constexpr const std::array<std::string_view, 6> small{
    "FXB     ", "CODI    ", "DDMX    ", "MOHO    ", "SPCB    ", "XXII    ",
};

constexpr const std::array<std::string_view, 1> single{
    "FXB     ",
};

#if __has_include(<frozen/unordered_map.h>) and __has_include(<frozen/string.h>)
constexpr frozen::unordered_map<frozen::string, std::int16_t, 100> frozen_all =
    {
        {"III     ", 0},  {"AGM-C   ", 1},  {"LOPE    ", 2},  {"FEMS    ", 3},
        {"IEA     ", 4},  {"VYMI    ", 5},  {"BHK     ", 6},  {"SIEB    ", 7},
        {"DGBP    ", 8},  {"INFN    ", 9},  {"USRT    ", 10}, {"BCOR    ", 11},
        {"TWM     ", 12}, {"BVSN    ", 13}, {"STBA    ", 14}, {"GPK     ", 15},
        {"LVHD    ", 16}, {"FTEK    ", 17}, {"GLBS    ", 18}, {"CUBB    ", 19},
        {"LRCX    ", 20}, {"HTGM    ", 21}, {"RYN     ", 22}, {"IPG     ", 23},
        {"PNNTG   ", 24}, {"ZIG     ", 25}, {"IVR-A   ", 26}, {"INVA    ", 27},
        {"MNE     ", 28}, {"KRA     ", 29}, {"BRMK    ", 30}, {"ARKG    ", 31},
        {"FFR     ", 32}, {"QTRX    ", 33}, {"XTN     ", 34}, {"BAC-A   ", 35},
        {"CYBE    ", 36}, {"ETJ     ", 37}, {"JHCS    ", 38}, {"RBCAA   ", 39},
        {"GDS     ", 40}, {"WTID    ", 41}, {"TCO     ", 42}, {"BWA     ", 43},
        {"MIE     ", 44}, {"GENY    ", 45}, {"TDOC    ", 46}, {"MCRO    ", 47},
        {"QFIN    ", 48}, {"NBTB    ", 49}, {"PWC     ", 50}, {"FQAL    ", 51},
        {"NJAN    ", 52}, {"IWB     ", 53}, {"GXGXW   ", 54}, {"EDUC    ", 55},
        {"RETL    ", 56}, {"VIACA   ", 57}, {"KLDO    ", 58}, {"NEE-I   ", 59},
        {"FBC     ", 60}, {"JW.A    ", 61}, {"BSMX    ", 62}, {"FMNB    ", 63},
        {"EXR     ", 64}, {"TAC     ", 65}, {"FDL     ", 66}, {"SWIR    ", 67},
        {"CLWT    ", 68}, {"LMHB    ", 69}, {"IRTC    ", 70}, {"CDMO    ", 71},
        {"HMLP-A  ", 72}, {"LVUS    ", 73}, {"UMRX    ", 74}, {"GJH     ", 75},
        {"FRME    ", 76}, {"CEIX    ", 77}, {"IHD     ", 78}, {"GHSI    ", 79},
        {"DCP-B   ", 80}, {"SB      ", 81}, {"DSE     ", 82}, {"CPRT    ", 83},
        {"NRZ     ", 84}, {"VLYPO   ", 85}, {"TDAC    ", 86}, {"ZXZZT   ", 87},
        {"IWX     ", 88}, {"NCSM    ", 89}, {"WIRE    ", 90}, {"SFST    ", 91},
        {"EWD     ", 92}, {"DEACW   ", 93}, {"TRPX    ", 94}, {"UCTT    ", 95},
        {"ZAZZT   ", 96}, {"CYD     ", 97}, {"NURE    ", 98}, {"WEAT    ", 99},
};

constexpr frozen::unordered_map<frozen::string, std::int16_t, 100>
    frozen_random = {
        {"FXB     ", 0},  {"CODI    ", 1},  {"DDMX    ", 2},  {"MOHO    ", 3},
        {"SPCB    ", 4},  {"XXII    ", 5},  {"MTC     ", 6},  {"DWAT    ", 7},
        {"ETSY    ", 8},  {"FUND    ", 9},  {"SPXE    ", 10}, {"PLXS    ", 11},
        {"IAT     ", 12}, {"APT     ", 13}, {"BANX    ", 14}, {"MWK     ", 15},
        {"LBC     ", 16}, {"HCCH    ", 17}, {"FJNK    ", 18}, {"SSNC    ", 19},
        {"EQWL    ", 20}, {"PCF     ", 21}, {"AIG     ", 22}, {"SPOK    ", 23},
        {"CELC    ", 24}, {"RESN    ", 25}, {"TAXF    ", 26}, {"VSEC    ", 27},
        {"PEN     ", 28}, {"PSB-W   ", 29}, {"GECCM   ", 30}, {"QEP     ", 31},
        {"TRS     ", 32}, {"CUK     ", 33}, {"FAN     ", 34}, {"AYR     ", 35},
        {"VEGN    ", 36}, {"TOTAR   ", 37}, {"AGO-F   ", 38}, {"SPLV    ", 39},
        {"EFA     ", 40}, {"MOR     ", 41}, {"LPT     ", 42}, {"RHE     ", 43},
        {"SFL     ", 44}, {"TSLF    ", 45}, {"PACK+   ", 46}, {"VGLT    ", 47},
        {"GVP     ", 48}, {"ONTO    ", 49}, {"VRTV    ", 50}, {"TZOO    ", 51},
        {"SLY     ", 52}, {"XLF     ", 53}, {"DIAL    ", 54}, {"JHMF    ", 55},
        {"XYF     ", 56}, {"EVFM    ", 57}, {"LCUT    ", 58}, {"MUH     ", 59},
        {"BFY     ", 60}, {"FNGU    ", 61}, {"ETI-    ", 62}, {"IWD     ", 63},
        {"CRC     ", 64}, {"WERN    ", 65}, {"NZF     ", 66}, {"CTEK    ", 67},
        {"FITE    ", 68}, {"LTHM    ", 69}, {"TESS    ", 70}, {"ORSN    ", 71},
        {"CDR-B   ", 72}, {"KEY-K   ", 73}, {"WDAY    ", 74}, {"WLH     ", 75},
        {"FORM    ", 76}, {"KORP    ", 77}, {"ROG     ", 78}, {"SCHA    ", 79},
        {"JAZZ    ", 80}, {"FLAX    ", 81}, {"LTBR    ", 82}, {"SOHU    ", 83},
        {"IBDD    ", 84}, {"FBND    ", 85}, {"OAC=    ", 86}, {"AGFSW   ", 87},
        {"IGLB    ", 88}, {"MOS     ", 89}, {"PLAG    ", 90}, {"NPTN    ", 91},
        {"VG      ", 92}, {"DO      ", 93}, {"SMCI    ", 94}, {"PACQ    ", 95},
        {"CDC     ", 96}, {"OPESW   ", 97}, {"EXP     ", 98}, {"NCTY    ", 99},
};

constexpr frozen::unordered_map<frozen::string, std::int16_t, 1> frozen_single =
    {{"FXB     ", 0}};
#endif
} // namespace data

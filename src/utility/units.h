#ifndef _OXYBELIS_UTILITY_UNITS_H
#define _OXYBELIS_UTILITY_UNITS_H

constexpr long double operator ""_Mm(long double val) {
    return val * 1'000'000.0;
}

constexpr long double operator ""_km(long double val) {
    return val * 1'000.0;
}

constexpr long double operator ""_m(long double val) {
    return val;
}

#endif

#ifndef _OXYBELIS_MATH_FIXED_H
#define _OXYBELIS_MATH_FIXED_H

#include <cstdint>
#include <type_traits>
#include <cmath>

template <size_t I, size_t F>
struct UFixed {
    static_assert(I + F == 64, "Only 64-bit unsigned fixed integer supported.");
    constexpr const static size_t IPART_SHIFT = F;
    constexpr const static size_t FPART_MASK = (1 << F) - 1;
    constexpr const static size_t FRAC_MAX = (1 << F) - 1;

    uint64_t raw;

    constexpr UFixed(uint64_t i, uint64_t f = 0):
        raw((i << IPART_SHIFT) | (f & FPART_MASK)) {
    }

    template <typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
    constexpr explicit UFixed(T floating) {
        const auto frac_max = static_cast<T>(FRAC_MAX);
        T i;
        auto f = std::modf(floating, &i);
        uint64_t ipart = static_cast<uint64_t>(i);
        uint64_t fpart = static_cast<uint64_t>(f * frac_max);
        this->raw = (ipart << IPART_SHIFT) | (fpart & FPART_MASK);
    }

    constexpr static UFixed<I, F> from_raw(uint64_t raw) {
        return UFixed<I, F>(raw >> IPART_SHIFT, raw & FPART_MASK);
    }

    constexpr uint64_t ipart() const {
        return this->raw >> IPART_SHIFT;
    }

    constexpr uint64_t fpart() const {
        return this->raw & FPART_MASK;
    }

    template <typename T = float, typename = std::enable_if_t<std::is_floating_point<T>::value>>
    constexpr float to_floating() const {
        return static_cast<T>(this->ipart()) + static_cast<T>(this->fpart()) / static_cast<T>(FRAC_MAX);
    }
};

template <size_t I, size_t F>
constexpr UFixed<I, F> operator+(UFixed<I, F> lhs, UFixed<I, F> rhs) {
    return UFixed<I, F>::from_raw(lhs.raw + rhs.raw);
}

template <size_t I, size_t F>
constexpr UFixed<I, F> operator-(UFixed<I, F> lhs, UFixed<I, F> rhs) {
    return UFixed<I, F>::from_raw(lhs.raw - rhs.raw);
}

template <size_t I, size_t F>
constexpr UFixed<I, F> operator*(UFixed<I, F> lhs, UFixed<I, F> rhs) {
    __uint128_t l = static_cast<__uint128_t>(lhs.raw);
    __uint128_t r = static_cast<__uint128_t>(rhs.raw);
    __uint128_t result = (l * r) >> F;
    return UFixed<I, F>::from_raw(static_cast<uint64_t>(result));
}

template <size_t I, size_t F>
constexpr UFixed<I, F> operator/(UFixed<I, F> lhs, UFixed<I, F> rhs) {
    __uint128_t l = static_cast<__uint128_t>(lhs.raw);
    __uint128_t r = static_cast<__uint128_t>(rhs.raw);
    __uint128_t result = (l << F) / r;
    return UFixed<I, F>::from_raw(static_cast<uint64_t>(result));
}

template <size_t I, size_t F>
constexpr UFixed<I, F> operator%(UFixed<I, F> lhs, UFixed<I, F> rhs) {
    return UFixed<I, F>::from_raw(lhs.raw % rhs.raw);
}

template <size_t I, size_t F>
constexpr bool operator<(UFixed<I, F> lhs, UFixed<I, F> rhs) {
    return lhs.raw < rhs.raw;
}

template <size_t I, size_t F>
constexpr bool operator>(UFixed<I, F> lhs, UFixed<I, F> rhs) {
    return lhs.raw > rhs.raw;
}

template <size_t I, size_t F>
constexpr bool operator<=(UFixed<I, F> lhs, UFixed<I, F> rhs) {
    return lhs.raw <= rhs.raw;
}

template <size_t I, size_t F>
constexpr bool operator>=(UFixed<I, F> lhs, UFixed<I, F> rhs) {
    return lhs.raw >= rhs.raw;
}

template <size_t I, size_t F>
constexpr bool operator==(UFixed<I, F> lhs, UFixed<I, F> rhs) {
    return lhs.raw == rhs.raw;
}

template <size_t I, size_t F>
constexpr bool operator!=(UFixed<I, F> lhs, UFixed<I, F> rhs) {
    return lhs.raw != rhs.raw;
}

template <typename Os, size_t I, size_t F>
Os& operator<<(Os& os, UFixed<I, F> uf) {
    return os << uf.to_floating();
}

using UFixed4816 = UFixed<48, 16>;
using UFixed568 = UFixed<56, 8>;
using UUnit = UFixed<0, 64>;

#endif

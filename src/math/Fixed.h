#ifndef _OXYBELIS_MATH_FIXED_H
#define _OXYBELIS_MATH_FIXED_H

#include <cstdint>
#include <type_traits>

template <size_t I, size_t F>
class UFixed {
    static_assert(I + F == 64, "Only 64-bit unsigned fixed integer supported.");
    constexpr const static size_t IPART_SHIFT = F;
    constexpr const static size_t FPART_MASK = (1 << F) - 1;

    uint64_t num;

public:
    constexpr UFixed(size_t i, size_t f = 0):
        num((i << IPART_SHIFT) | (f & FPART_MASK)) {
    }

    constexpr size_t ipart() const {
        return this->num >> IPART_SHIFT;
    }

    constexpr size_t fpart() const {
        return this->num & FPART_MASK;
    }
};

using UFixed4816 = UFixed<48, 16>;
using UFixed568 = UFixed<56, 8>;
using Unit = UFixed<0, 64>;

template <size_t I, size_t F>
constexpr UFixed<I, F> operator+(UFixed<I, F> lhs, UFixed<I, F> rhs) {
    return UFixed<I, F>(lhs.num + rhs.num);
}

template <size_t I, size_t F>
constexpr UFixed<I, F> operator-(UFixed<I, F> lhs, UFixed<I, F> rhs) {
    return UFixed<I, F>(lhs.num - rhs.num);
}

template <typename Os, size_t I, size_t F>
Os& operator<<(Os& os, UFixed<I, F> uf) {
    os << uf.ipart() << '.';
    return os;
}

#endif

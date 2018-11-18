#ifndef _OXYBELIS_UTILITY_UTILITY_H
#define _OXYBELIS_UTILITY_UTILITY_H

#include <utility>
#include <tuple>
#include <array>
#include <cstddef>
#include <cmath>
#include "glad/glad.h"

template <typename F>
struct Defer {
    F f;

    ~Defer() {
        f();
    }
};

template <typename Fn>
auto defer(Fn&& f) {
    return Defer<Fn>{std::forward<Fn>(f)};
}

template <typename Fn, typename... Fns>
struct Overload: Fn, Overload<Fns...> {
    Overload(Fn h, Fns... t):
        Fn(h), Overload<Fns...>(t...) {
    }

    using Fn::operator();
    using Overload<Fns...>::operator();
};

template <typename Fn>
struct Overload<Fn>: Fn {
    Overload(Fn h):
        Fn(h) {
    }

    using Fn::operator();
};

template <typename... Fns>
constexpr auto make_overload(Fns&&... overloads) {
    return Overload<Fns...>{overloads...};
}

template <typename F>
constexpr void pack_foreach(F&&) {}

template <typename F, typename T, typename... Ts>
constexpr void pack_foreach(F&& f, T&& h, Ts&&... tail) {
    f(h);
    pack_foreach(std::forward<F>(f), tail...);
}

template <class T>
inline size_t hash_combine(size_t seed, const T& v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
}

constexpr long double operator ""_deg(long double deg) {
    return deg * M_PI / 180.0;
}

// see https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2
template <typename T>
constexpr auto next_2pow(T x) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;
    return x + 1;
}

#endif

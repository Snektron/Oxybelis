#ifndef _OXYBELIS_UTILITY_UTILITY_H
#define _OXYBELIS_UTILITY_UTILITY_H

#include <memory>
#include <utility>
#include <tuple>
#include "glad/glad.h"

template <typename T>
using DestroyFn = void(T*);

template <typename T, DestroyFn<T> D>
struct Deleter {
    void operator()(T* ptr) {
        D(ptr);
    }
};

template <typename T, DestroyFn<T> D>
using CPtr = std::unique_ptr<T, Deleter<T, D>>;

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

#endif

#ifndef _OXYBELIS_UTILITY_H
#define _OXYBELIS_UTILITY_H

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

template <typename Fn>
struct Overload: Fn {
    Overload(Fn f):
        Fn(f) {
    }
};

template <typename... Fns>
struct Overloaded: Overload<Fns>... {
    Overloaded(Fns... f):
        Overload<Fns>(f)... {
    }
};

template <typename... Fns>
constexpr auto make_overload(Fns&&... overloads) {
    return Overloaded<Fns...>{overloads...};
}

#endif

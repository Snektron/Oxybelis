#ifndef _OXYBELIS_UTILITY_H
#define _OXYBELIS_UTILITY_H

#include <memory>
#include <utility>
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

template <typename F>
auto defer(F&& f) {
    return Defer<F>{std::forward<F>(f)};
}

#endif

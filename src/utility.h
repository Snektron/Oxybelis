#ifndef _OXYBELIS_UTILITY_H
#define _OXYBELIS_UTILITY_H

#include <memory>

template <typename T>
using DeleteFn = void(T*);

template <typename T, DeleteFn<T> D>
struct Deleter {
    void operator()(T* ptr) {
        D(ptr);
    }
};

template <typename T, DeleteFn<T> D>
using CPtr = std::unique_ptr<T, Deleter<T, D>>;

#endif

#ifndef _OXYBELIS_UTILITY_UTILITY_H
#define _OXYBELIS_UTILITY_UTILITY_H

#include <memory>
#include <utility>
#include <tuple>
#include <array>
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

namespace detail {
    template <size_t... Indices>
    struct ArrayIndices {
        template <typename F>
        constexpr static auto apply(F&& f) {
            using Result = std::result_of_t<F(size_t)>;
            return std::array<Result, sizeof...(Indices)>{f(Indices)...};
        }
    };

    template <size_t N, typename Indices>
    struct Prepend_{};

    template <size_t N, size_t... Indices>
    struct Prepend_<N, ArrayIndices<Indices...>> {
        using Type = ArrayIndices<N, Indices...>;
    };

    template <size_t N, typename Indices>
    using Prepend = typename Prepend_<N, Indices>::Type;

    template <size_t N>
    struct GenerateIndices_ {
        using Type = Prepend<N, typename GenerateIndices_<N - 1>::Type>;
    };

    template <>
    struct GenerateIndices_<0> {
        using Type = ArrayIndices<0>;
    };

    template <size_t N>
    using GenerateIndices = typename GenerateIndices_<N>::Type;
}

template <size_t N, typename F>
constexpr auto initialize_array(F&& f) {
    return detail::GenerateIndices<N - 1>::apply(std::forward<F>(f));
}

#endif

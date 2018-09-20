#ifndef _OXYBELIS_MATH_VEC_H
#define _OXYBELIS_MATH_VEC_H

#include <cstddef>
#include <type_traits>

namespace detail {
    template <typename T, size_t N>
    struct SimdVecHelper {
        constexpr const static bool valid = false;
    };

    #define DEFINE_SIMD_VEC(type, size) \
        template <> \
        struct SimdVecHelper<type, (size)> { \
            constexpr const static bool valid = true; \
            using Type [[gnu::vector_size((size) * sizeof(type))]] = type; \
        }

    #define DEFINE_SIMD_VEC_COMMON_SIZES(type) \
        DEFINE_SIMD_VEC(type, 2); \
        DEFINE_SIMD_VEC(type, 3); \
        DEFINE_SIMD_VEC(type, 4)

    DEFINE_SIMD_VEC_COMMON_SIZES(float);
    DEFINE_SIMD_VEC_COMMON_SIZES(double);
    DEFINE_SIMD_VEC_COMMON_SIZES(int);

    template <typename T, size_t N>
    using SimdVec = typename SimdVecHelper<T, N>::Type;
}

template <typename T, size_t N>
struct Vec {
    detail::SimdVec<T, N> elements;

    constexpr const static size_t Rows = N;
    constexpr const static size_t Cols = 1;

    template <typename U, typename V, typename... Ts>
    constexpr Vec(const U& x, const V& y, const Ts&... elems):
        elements{static_cast<T>(x), static_cast<T>(y), static_cast<T>(elems)...} {
    }

    constexpr Vec(const detail::SimdVec<T, N>& elements):
        elements(elements) {
    }

    T& operator[](size_t row) {
        return this->elements[row];
    }

    const T& operator[](size_t row) const {
        return this->elements[row];
    }

    const T* data() const {
        // This *might* be non standard
        return reinterpret_cast<const T*>(&this->elements);
    }

    template <typename U>
    Vec<T, N>& operator+=(const Vec<U, N>& other);

    template <typename U>
    Vec<T, N>& operator-=(const Vec<U, N>& other);

    template <typename U>
    Vec<T, N>& operator*=(const Vec<U, N>& other);

    template <typename U>
    Vec<T, N>& operator/=(const Vec<U, N>& other);
};

using Vec2F = Vec<float, 2>;
using Vec3F = Vec<float, 3>;
using Vec4F = Vec<float, 4>;

using Vec2D = Vec<double, 2>;
using Vec3D = Vec<double, 3>;
using Vec4D = Vec<double, 4>;

using Vec2I = Vec<int, 2>;
using Vec3I = Vec<int, 3>;
using Vec4I = Vec<int, 4>;

template <typename T, typename U, size_t N>
auto operator+(const Vec<T, N>& lhs, const Vec<U, N>& rhs) {
    using Result = decltype(std::declval<T>() + std::declval<U>());
    return Vec<Result, N>(lhs.elements + rhs.elements);
}

template <typename T, typename U, size_t N>
auto operator-(const Vec<T, N>& lhs, const Vec<U, N>& rhs) {
    using Result = decltype(std::declval<T>() - std::declval<U>());
    return Vec<Result, N>(lhs.elements - rhs.elements);
}

template <typename T, typename U, size_t N>
auto operator*(const Vec<T, N>& lhs, const Vec<U, N>& rhs) {
    using Result = decltype(std::declval<T>() * std::declval<U>());
    return Vec<Result, N>(lhs.elements * rhs.elements);
}

template <typename T, typename U, size_t N>
auto operator/(const Vec<T, N>& lhs, const Vec<U, N>& rhs) {
    using Result = decltype(std::declval<T>() / std::declval<U>());
    return Vec<Result, N>(lhs.elements / rhs.elements);
}

template <typename T, size_t N>
template <typename U>
Vec<T, N>& Vec<T, N>::operator+=(const Vec<U, N>& other) {
    *this = *this + other;
    return *this;
}

template <typename T, size_t N>
template <typename U>
Vec<T, N>& Vec<T, N>::operator-=(const Vec<U, N>& other) {
    *this = *this - other;
    return *this;
}

template <typename T, size_t N>
template <typename U>
Vec<T, N>& Vec<T, N>::operator*=(const Vec<U, N>& other) {
    *this = *this * other;
    return *this;
}

template <typename T, size_t N>
template <typename U>
Vec<T, N>& Vec<T, N>::operator/=(const Vec<U, N>& other) {
    *this = *this / other;
    return *this;
}

#endif

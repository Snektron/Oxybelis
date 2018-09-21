#ifndef _OYXBELIS_MATH_MAT_H
#define _OYXBELIS_MATH_MAT_H

#include <cstddef>
#include <array>
#include "math/Vec.h"

template <typename T, size_t M, size_t N>
struct Mat {
    constexpr const static size_t Rows = M;
    constexpr const static size_t Cols = N;

    std::array<Vec<T, M>, N> cols;

    constexpr Mat() = default;

    template <typename... Ts>
    constexpr Mat(const Vec<Ts, M>&... vecs):
        cols{vecs...} {
    }

    constexpr Mat(const std::array<Vec<T, M>, N>& cols):
        cols(cols) {
    }

    T operator()(size_t i, size_t j) const {
        return this->cols[j][i];
    }

    T operator[](size_t i) const {
        return this->cols[i / M][i % M];
    }

    Vec<T, M>& operator()(size_t i) {
        return this->cols[i];
    }

    const Vec<T, M>& operator()(size_t i) const {
        return this->cols[i];
    }
};

template <typename T, typename U, size_t M, size_t N>
auto operator+(const Mat<T, M, N>& lhs, const Mat<U, M, N>& rhs) {
    using Result = decltype(std::declval<T>() + std::declval<U>());

    Mat<Result, M, N> result;
    for (size_t i = 0; i < N; ++i)
        result(i) = lhs(i) + rhs(i);

    return result;
}

template <typename T, typename U, size_t M, size_t N>
auto operator-(const Mat<T, M, N>& lhs, const Mat<U, M, N>& rhs) {
    using Result = decltype(std::declval<T>() - std::declval<U>());

    Mat<Result, M, N> result;
    for (size_t i = 0; i < N; ++i)
        result(i) = lhs(i) - rhs(i);

    return result;
}

template <typename Os, typename T, size_t M, size_t N>
Os& operator<<(Os& os, const Mat<T, M, N>& mat) {
    os.precision(3);
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            os << mat(i, j);
            if (j != N - 1)
                os << '\t';
        }
        if (i != M - 1)
            os << std::endl;
    }

    return os;
}

using Mat4F = Mat<float, 4, 4>;

using Mat4D = Mat<double, 4, 4>;

using Mat4I = Mat<int, 4, 4>;

#endif

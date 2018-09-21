#ifndef _OYXBELIS_MATH_MAT_H
#define _OYXBELIS_MATH_MAT_H

#include <array>
#include <algorithm>
#include "math/Vec.h"

template <typename T, size_t M, size_t N>
struct Mat;

template <typename T>
using Mat2 = Mat<T, 2, 2>;
using Mat2F = Mat2<float>;
using Mat2D = Mat2<double>;
using Mat2I = Mat2<int>;

template <typename T>
using Mat3 = Mat<T, 3, 3>;
using Mat3F = Mat3<float>;
using Mat3D = Mat3<double>;
using Mat3I = Mat3<int>;

template <typename T>
using Mat4 = Mat<T, 4, 4>;
using Mat4F = Mat4<float>;
using Mat4D = Mat4<double>;
using Mat4I = Mat4<int>;

template <typename T, size_t M, size_t N>
struct BaseMat {
    union {
        std::array<T, M * N> elements;
        std::array<Vec<T, M>, N> columns;
    };

    constexpr BaseMat():
        elements{0} {
    }

    constexpr BaseMat(const T& diag):
        elements{0} {
        for (size_t i = 0; i < std::min(M, N); ++i)
            this->columns[i](i) = diag;
    }

    template <typename... Args, typename = std::enable_if_t<sizeof...(Args) == N && (M > 1)>>
    constexpr BaseMat(const Vec<Args, M>&... args):
        columns{static_cast<Vec<Args, M>>(args)...} {
    }

    template <typename... Args, typename = std::enable_if_t<sizeof...(Args) == M * N>>
    constexpr BaseMat(Args&&... args):
        elements{static_cast<T>(std::forward<Args>(args))...} {
    }
};

template <typename T, size_t N>
struct BaseMat<T, 1, N> {
    union {
        std::array<T, N> elements;
    };

    constexpr BaseMat():
        elements{0} {
    }

    constexpr BaseMat(const T& diag):
        elements{0} {
        this->elements[0] = diag;
    }

    template <typename... Args, typename = std::enable_if_t<sizeof...(Args) == N>>
    constexpr BaseMat(Args&&... args):
        elements{static_cast<T>(std::forward<Args>(args))...} {
    }
};

template <typename T, size_t M, size_t N>
struct Mat: BaseMat<T, M, N> {
    static_assert(N > 1, "Cannot create column matrix, use a vector instead");

    constexpr const static size_t Rows = M;
    constexpr const static size_t Cols = N;

    using Base = BaseMat<T, M, N>;
    using Base::Base;

    constexpr static Mat<T, M, N> identity() {
        return Mat<T, M, N>(1);
    }

    constexpr Vec<T, M>& column(size_t i) {
        return this->columns[i];
    }

    constexpr const Vec<T, M>& column(size_t i) const {
        return this->columns[i];
    }

    constexpr T& operator()(size_t i, size_t j) {
        return this->columns[j](i);
    }

    constexpr const T& operator()(size_t i, size_t j) const {
        return this->columns[j](i);
    }

    constexpr T& operator[](size_t i) {
        return this->elements[i];
    }

    constexpr const T& operator[](size_t i) const {
        return this->elements[i];
    }

    constexpr const T* data() const {
        return this->elements.data();
    }
};

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

#endif

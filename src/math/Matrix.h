#ifndef _OXYBELIS_MATH_MATRIX_H
#define _OXYBELIS_MATH_MATRIX_H

#include <cstddef>
#include <array>
#include <ostream>
#include <type_traits>
#include <cmath>

template <typename T, size_t R, size_t C>
class Matrix;

template <typename T = double>
using Matrix3 = Matrix<T, 3, 3>;

template <typename T = double>
using Matrix4 = Matrix<T, 4, 4>;

namespace op {
    template <typename Op, typename T>
    constexpr bool result_same() {
        return std::is_convertible<typename Op::ScalarResult, T>::value;
    }

    template <typename T, typename U, size_t M, size_t N>
    struct Add {
        using ScalarResult = decltype(std::declval<T>() + std::declval<U>());
        using Result = Matrix<ScalarResult, M, N>;
        using Lhs = Matrix<T, M, N>;
        using Rhs = Matrix<U, M, N>;

        constexpr void operator()(Result& dst, const Lhs& lhs, const Rhs& rhs) {
            for (size_t i = 0; i < M * N; ++i)
                dst[i] = lhs[i] + rhs[i];
        }
    };

    template <typename T, typename U, size_t M, size_t N>
    struct Sub {
        using ScalarResult = decltype(std::declval<T>() - std::declval<U>());
        using Result = Matrix<ScalarResult, M, N>;
        using Lhs = Matrix<T, M, N>;
        using Rhs = Matrix<U, M, N>;

        constexpr void operator()(Result& dst, const Lhs& lhs, const Rhs& rhs) {
            for (size_t i = 0; i < M * N; ++i)
                dst[i] = lhs[i] - rhs[i];
        }
    };

    template <typename T, typename U, size_t M, size_t N, size_t O>
    struct Product {
        using ScalarResult = decltype(std::declval<T>() * std::declval<U>() + std::declval<T>() * std::declval<U>());
        using Result = Matrix<ScalarResult, M, O>;
        using Lhs = Matrix<T, M, N>;
        using Rhs = Matrix<U, N, O>;

        constexpr void operator()(Result& dst, const Lhs& lhs, const Rhs& rhs) {
            auto result = Result::make_zeroes();
            for (size_t k = 0; k < N; ++k) {
                for (size_t i = 0; i < M; ++i) {
                    for (size_t j = 0; j < O; ++j) {
                        result(i, j) += lhs(i, k) * rhs(k, j);
                    }
                }
            }

            dst = std::move(result);
        }
    };

    template <typename T, typename U, size_t M, size_t N>
    struct Scale {
        using ScalarResult = decltype(std::declval<T>() * std::declval<U>());
        using Result = Matrix<ScalarResult, M, N>;
        using Lhs = Matrix<T, M, N>;
        using Rhs = U;

        constexpr void operator()(Result& dst, const Lhs& lhs, const Rhs& rhs) {
            for (size_t i = 0; i < M * N; ++i)
                dst[i] = lhs[i] * rhs;
        }
    };

    template <typename T, size_t M, size_t N>
    struct Negate {
        using ScalarResult = decltype(-std::declval<T>());
        using Result = Matrix<ScalarResult, M, N>;
        using Lhs = Matrix<T, M, N>;

        constexpr void operator()(Result& dst, const Lhs& lhs) {
            for (size_t i = 0; i < M * N; ++i)
                dst[i] = -lhs[i];
        }
    };
}

template <typename T, size_t M, size_t N>
class MatrixBase {
    static_assert(M >= 1 && N >= 1, "A matrix must at least be 1x1");
public:
    using Type = T;
    constexpr const static size_t Rows = M;
    constexpr const static size_t Cols = N;
    constexpr const static size_t Size = Rows * Cols;

protected:
    using Derived = Matrix<T, Rows, Cols>;
    std::array<T, Size> elements;

public:
    constexpr MatrixBase() {};

    constexpr MatrixBase(const Derived& other):
        elements(other.elements) {
    }

    constexpr MatrixBase(Derived&& other):
        elements(std::move(other.elements)) {
    }

    constexpr MatrixBase(const std::array<T, Size>& elements):
        elements(elements) {
    }

    constexpr MatrixBase(std::array<T, Size>&& elements):
        elements(std::move(elements)) {
    }

    constexpr static Derived make_identity() {
        static_assert(Rows == Cols, "Can only create a square identity matrix");
        Derived result;
        result.identity();
        return result;
    }

    constexpr static Derived make_zeroes() {
        return Derived({0});
    }

    constexpr static Derived make_filled(const T& initial) {
        Derived result;
        result.fill(initial);
        return result;
    }

    constexpr size_t rows() const {
        return Rows;
    }

    constexpr size_t columns() const {
        return Cols;
    }

    constexpr T& operator()(size_t row, size_t col) {
        return this->elements[col * Rows + row];
    }

    constexpr const T& operator()(size_t row, size_t col) const {
        return this->elements[col * Rows + row];
    }

    constexpr T& operator[](size_t i) {
        return this->elements[i];
    }

    constexpr const T& operator[](size_t i) const {
        return this->elements[i];
    }

    constexpr T* data() {
        return this->elements.data();
    }

    constexpr const T* data() const {
        return this->elements.data();
    }

    template <typename U>
    constexpr Derived& operator=(const Matrix<U, Rows, Cols>& other) {
        this->elements = other.elements;
        return this->derived();
    }

    template <typename U>
    constexpr Derived& operator=(Matrix<U, Rows, Cols>&& other) {
        this->elements = std::move(other.elements);
        return this->derived();
    }

    template <typename U>
    constexpr Derived& operator+=(const Matrix<U, Rows, Cols>& other) {
        using Add = op::Add<T, U, Rows, Cols>;
        static_assert(op::result_same<Add, T>(), "Incompatible types in in-place matrix addition");
        Add{}(this->derived(), this->derived(), other);
        return this->derived();
    }

    template <typename U>
    constexpr Derived& operator-=(const Matrix<U, Rows, Cols>& other) {
        using Sub = op::Sub<T, U, Rows, Cols>;
        static_assert(op::result_same<Sub, T>(), "Incompatible types in in-place matrix subtraction");
        Sub{}(this->derived(), this->derived(), other);
        return this->derived();
    }

    template <typename U>
    constexpr Derived& operator*=(const Matrix<U, Rows, Cols>& other) {
        static_assert(Rows == Cols, "Can only perform in-place addition with square matrices");
        using Product = op::Product<T, U, Rows, Cols, Cols>;
        static_assert(op::result_same<Product, T>(), "Incompatible types in in-place matrix product");
        Product{}(this->derived(), this->derived(), other);
        return this->derived();
    }

    template <typename U>
    constexpr Derived& operator*=(const U& scalar) {
        using Scale = op::Scale<T, U, Rows, Cols>;
        static_assert(op::result_same<Scale, T>(), "Incompatible types in in-place matrix scale");
        Scale{}(this->derived(), this->derived(), scalar);
        return this->derived();
    }

    constexpr Derived& negate() {
        using Negate = op::Negate<T, Rows, Cols>;
        static_assert(op::result_same<Negate, T>(), "Incompatible types in in-place matrix negate");
        Negate{}(this->derived(), this->derived());
        return this->derived();
    }

    constexpr Derived& fill(const T& value) {
        this->elements.fill(value);
        return this->derived();
    }

    constexpr Derived& zero() {
        return this->fill(0);
    }

    constexpr Derived& identity() {
        static_assert(Rows == Cols, "Can only set a square matrix to identity");
        for (size_t i = 0; i < Rows; ++i)
            for (size_t j = 0; j < Rows; ++j)
                (*this)(j, i) = i == j ? 1 : 0;
        return this->derived();
    }

    constexpr auto begin() {
        return this->elements.begin();
    }

    constexpr auto begin() const {
        return this->elements.begin();
    }

    constexpr auto end() {
        return this->elements.end();
    }

    constexpr auto end() const {
        return this->elements.end();
    }

protected:
    constexpr Derived& derived() {
        return *static_cast<Derived*>(this);
    }

    constexpr const Derived& derived() const {
        return *static_cast<Derived*>(this);
    }
};

template <typename T, size_t M, size_t N = M>
class Matrix: public MatrixBase<T, M, N> {
    using Base = MatrixBase<T, M, N>;
public:
    using Base::Base;
};

template <typename T, typename U, size_t M, size_t N>
constexpr auto operator+(const Matrix<T, M, N>& lhs, const Matrix<U, M, N>& rhs) {
    using Add = op::Add<T, U, M, N>;
    typename Add::Result result;
    Add{}(result, lhs, rhs);
    return result;
}

template <typename T, typename U, size_t M, size_t N>
constexpr auto operator-(const Matrix<T, M, N>& lhs, const Matrix<U, M, N>& rhs) {
    using Sub = op::Sub<T, U, M, N>;
    typename Sub::Result result;
    Sub{}(result, lhs, rhs);
    return result;
}

template <typename T, typename U, size_t M, size_t N, size_t O>
constexpr auto operator*(const Matrix<T, M, N>& lhs, const Matrix<U, N, O>& rhs) {
    using Product = op::Product<T, U, M, N, O>;
    typename Product::Result result;
    Product{}(result, lhs, rhs);
    return result;
}

template <typename T, typename U, size_t M, size_t N>
constexpr auto operator*(const Matrix<T, M, N>& lhs, const U& rhs) {
    using Scale = op::Scale<T, U, M, N>;
    typename Scale::Result result;
    Scale{}(result, lhs, rhs);
    return result;
}

template <typename T, typename U, size_t M, size_t N>
constexpr auto operator*(const U& rhs, const Matrix<T, M, N>& lhs) {
    using Scale = op::Scale<T, U, M, N>;
    typename Scale::Result result;
    Scale{}(result, lhs, rhs);
    return result;
}

template <typename T, size_t M, size_t N>
constexpr auto operator-(const Matrix<T, M, N>& lhs) {
    using Negate = op::Negate<T, M, N>;
    typename Negate::Result result;
    Negate{}(result, lhs);
    return result;
}

template <typename Os, typename T, size_t M, size_t N>
Os& operator<<(Os& os, const Matrix<T, M, N>& mat) {
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            os << mat(i, j);
            if (j != N - 1)
                os << ' ';
        }
        if (i != M - 1)
            os << std::endl;
    }

    return os;
}

template <typename T>
constexpr auto make_translation(const T& x, const T& y, const T& z) {
    auto result = Matrix<T, 4, 4>::make_identity();
    result(0, 3) = x;
    result(1, 3) = y;
    result(2, 3) = z;
    return result;
}

template <typename T>
constexpr auto make_scaling(const T& x, const T& y, const T& z) {
    auto result = Matrix<T, 4, 4>::make_zeroes();
    result(0, 0) = x;
    result(1, 1) = y;
    result(2, 2) = z;
    result(3, 3) = 1;
    return result;
}

template <typename T>
constexpr auto make_rotation(const T& x, const T& y, const T& z, const T& a) {
    auto result = Matrix<T, 4, 4>::make_zeroes();
    auto c = std::cos(a);
    auto ci = 1 - c;
    auto s = std::sin(a);

    result(0, 0) = x * x * ci + c;
    result(1, 0) = x * y * ci + z * s;
    result(2, 0) = x * z * ci - y * s;
    result(0, 1) = x * y * ci - z * s;
    result(1, 1) = y * y * ci + c;
    result(2, 1) = y * z * ci + x * s;
    result(0, 2) = x * z * ci + y * s;
    result(1, 2) = y * z * ci - x * s;
    result(2, 2) = z * z * ci + c;
    result(3, 3) = 1;
    return result;
}

template <typename T>
constexpr auto make_orthographic(const T& left, const T& right, const T& top, const T& bottom, const T& near, const T& far) {
    auto result = Matrix<T, 4, 4>::make_zeroes();

    auto rl = right - left;
    auto tb = top - bottom;
    auto nf = near - far;

    result(0, 0) = 2 / rl;
    result(1, 1) = 2 / tb;
    result(2, 2) = 2 / nf;
    result(0, 3) = -(right + left) / rl;
    result(1, 3) = -(top + bottom) / tb;
    result(2, 3) = -(near + far) / nf;
    result(3, 3) = 1;

    return result;
}

template <typename T>
constexpr auto make_frustrum(const T& top, const T& bottom, const T& left, const T& right, const T& near, const T& far) {
    auto result = Matrix<T, 4, 4>::make_zeroes();

    auto rl = right - left;
    auto tb = top - bottom;
    auto nf = near - far;

    result(0, 0) = 2 * near / rl;
    result(1, 1) = 2 * near / tb;
    result(0, 2) = -(right + left) / rl;
    result(1, 2) = -(bottom + top) / tb;
    result(2, 2) = (near + far) / nf;
    result(2, 3) = -2 * far * near / nf;
    result(3, 2) = 1;

    return result;
}

template <typename T>
constexpr auto make_perspective(const T& aspect, const T& fov, const T& near, const T& far) {
    auto result = Matrix<T, 4, 4>::make_zeroes();

    auto nf = near - far;
    auto tan_fov_2 = tan(fov / 2);

    result(0, 0) = 1 / (aspect * tan_fov_2);
    result(1, 1) = 1 / tan_fov_2;
    result(2, 2) = -(near + far) / nf;
    result(2, 3) = -2 * far * near / nf;
    result(3, 2) = -1;

    return result;
}

#endif

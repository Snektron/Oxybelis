#ifndef _OXYBELIS_MATH_MATRIX_H
#define _OXYBELIS_MATH_MATRIX_H

#include <cstddef>
#include <array>
#include <ostream>
#include <type_traits>

template <typename T, size_t R, size_t C>
class Matrix;

namespace op {
    template <typename Op, typename T>
    constexpr bool result_same() {
        return std::is_same<typename Op::ScalarResult, T>::value;
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
            Result result({0});
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

template <typename T, size_t R, size_t C>
class MatrixBase {
    static_assert(R >= 1 && C >= 1, "Matrix must at least be 1x1");
public:
    using Type = T;
    constexpr const static size_t Size = R * C;
    constexpr const static size_t Rows = R;
    constexpr const static size_t Cols = C;

protected:
    using Derived = Matrix<T, R, C>;
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

template <typename Os, typename T, size_t R, size_t C>
Os& operator<<(Os& os, const Matrix<T, R, C>& mat) {
    for (size_t i = 0; i < C; ++i) {
        for (size_t j = 0; j < R; ++j) {
            os << mat(j, i);
            if (j != R - 1)
                os << ' ';
        }
        if (i != C - 1)
            os << std::endl;
    }

    return os;
}

template <typename T, size_t R, size_t C>
class Matrix: public MatrixBase<T, R, C> {
    using Base = MatrixBase<T, R, C>;
public:
    using Base::Base;
};

template <typename T = double>
using Matrix4 = Matrix<T, 4, 4>;

template <typename T = double>
using Matrix3 = Matrix<T, 3, 3>;

#endif

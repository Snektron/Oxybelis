#ifndef _OXYBELIS_MATH_MATRIX_H
#define _OXYBELIS_MATH_MATRIX_H

#include <cstddef>
#include <array>
#include <ostream>
#include <type_traits>

template <typename T, size_t R, size_t C>
class Matrix;

template <typename T, size_t R, size_t C>
class MatrixBase {
    static_assert(R >= 1 && C >= 1, "Matrix must at least be 1x1");
    using Derived = Matrix<T, R, C>;
public:
    using Type = T;
    constexpr const static size_t Size = R * C;
    constexpr const static size_t Rows = R;
    constexpr const static size_t Cols = C;

private:
    std::array<T, Size> elements;

public:
    MatrixBase() = default;

    MatrixBase(const Derived& other):
        elements(other.elements) {
    }

    MatrixBase(Derived&& other):
        elements(std::move(other.elements)) {
    }

    MatrixBase(const std::array<T, Size>& elements):
        elements(elements) {
    }

    MatrixBase(std::array<T, Size>&& elements):
        elements(std::move(elements)) {
    }

    T& operator()(size_t row, size_t col) {
        return this->elements[col * Rows + row];
    }

    const T& operator()(size_t row, size_t col) const {
        return this->elements[col * Rows + row];
    }

    T& operator[](size_t i) {
        return this->elements[i];
    }

    const T& operator[](size_t i) const {
        return this->elements[i];
    }

    template <typename U>
    Derived& operator=(const Matrix<U, Rows, Cols>& other) {
        this->elements = other.elements;
        return this->derived();
    }

    template <typename U>
    Derived& operator=(Matrix<U, Rows, Cols>&& other) {
        this->elements = std::move(other.elements);
        return this->derived();
    }

    template <typename U, typename V>
    static void add(Derived& dst, const Matrix<U, Rows, Cols>& l, const Matrix<V, Rows, Cols>& r) {
        for (size_t i = 0; i < Size; ++i)
            dst[i] = l[i] + r[i];
    }

    template <typename U, typename V>
    static void sub(Derived& dst, const Matrix<U, Rows, Cols>& l, const Matrix<V, Rows, Cols>& r) {
        for (size_t i = 0; i < Size; ++i)
            dst[i] = l[i] - r[i];
    }

    template <typename U, typename V, size_t N>
    static void mul(Derived& dst, const Matrix<U, Rows, N>& l, const Matrix<V, N, Cols>& r) {
        Derived result({0});
        for (size_t k = 0; k < N; ++k) {
            for (size_t i = 0; i < Rows; ++i) {
                for (size_t j = 0; j < Cols; ++j) {
                    result(i, j) += l(i, k) * r(k, j);
                }
            }
        }

        dst = std::move(result);
    }

    template <typename U>
    static void neg(Derived& dst, const Matrix<U, Rows, Cols>& src) {
        for (size_t i = 0; i < Size; ++i)
            dst[i] = -src[i];
    }

    template <typename U, typename S>
    static void scale(Derived& dst, const Matrix<U, Rows, Cols>& src, const S& scalar) {
        for (size_t i = 0; i < Size; ++i)
            dst[i] = src[i] * scalar;
    }

    template <typename U>
    Derived& operator+=(const Matrix<U, Rows, Cols>& other) {
        using Result = decltype(std::declval<T>() + std::declval<U>());
        static_assert(std::is_same<Result, T>::value, "Cannot in-place add these types");
        add(this->derived(), this->derived(), other);
        return this->derived();
    }

    template <typename U>
    Derived& operator-=(const Matrix<U, Rows, Cols>& other) {
        using Result = decltype(std::declval<T>() + std::declval<U>());
        static_assert(std::is_same<Result, T>::value, "Cannot in-place subtract these types");
        sub(this->derived(), this->derived(), other);
        return this->derived();
    }

    template <typename U>
    Derived& operator*=(const Matrix<U, Rows, Cols>& other) {
        static_assert(Rows == Cols, "Cannot in-place multiply these matrices");
        using Result = decltype(std::declval<T>() + std::declval<U>());
        static_assert(std::is_same<Result, T>::value, "Cannot in-place multiply these types");
        mul(this->derived(), this->derived(), other);
        return this->derived();
    }

    template <typename U>
    Derived& operator*=(const U& scalar) {
        static_assert(Rows == Cols, "Cannot in-place multiply these matrices");
        using Result = decltype(std::declval<T>() * std::declval<U>());
        static_assert(std::is_same<Result, T>::value, "Cannot in-place scale these types");
        scale(this->derived(), this->derived(), scalar);
        return this->derived();
    }

    Derived& negate() {
        using Result = decltype(-std::declval<T>());
        static_assert(std::is_same<T, Result>::value, "Cannot in-place negate this type");
        neg(this->derived(), this->derived());
        return this->derived();
    }

private:
    Derived& derived() {
        return *static_cast<Derived*>(this);
    }

    const Derived& derived() const {
        return *static_cast<Derived*>(this);
    }
};

template <typename T, typename U, size_t M, size_t N>
auto operator+(const Matrix<T, M, N>& l, const Matrix<U, M, N>& r) {
    using Result = decltype(std::declval<T>() + std::declval<U>());
    Matrix<Result, M, N> result;
    Matrix<Result, M, N>::add(result, l, r);
    return result;
}

template <typename T, typename U, size_t M, size_t N>
auto operator-(const Matrix<T, M, N>& l, const Matrix<U, M, N>& r) {
    using Result = decltype(std::declval<T>() + std::declval<U>());
    Matrix<Result, M, N> result;
    Matrix<Result, M, N>::sub(result, l, r);
    return result;
}

template <typename T, typename U, size_t M, size_t N, size_t O>
auto operator*(const Matrix<T, M, N>& l, const Matrix<U, N, O>& r) {
    using Result = decltype(std::declval<T>() * std::declval<U>());
    Matrix<Result, M, O> result;
    Matrix<Result, M, O>::mul(result, l, r);
    return result;
}

template <typename T, typename U, size_t M, size_t N>
auto operator*(const Matrix<T, M, N>& src, const U& scalar) {
    using Result = decltype(std::declval<T>() * std::declval<U>());
    Matrix<Result, M, N> result;
    Matrix<Result, M, N>::scale(result, src, scalar);
    return result;
}

template <typename T, typename U, size_t M, size_t N>
auto operator-(const Matrix<T, M, N>& src) {
    using Result = decltype(-std::declval<T>());
    Matrix<Result, M, N> result;
    Matrix<Result, M, N>::neg(result, src);
    return result;
}

template <typename Os, typename T, size_t R, size_t C>
Os& operator<<(Os& os, const Matrix<T, R, C>& mat) {
    for (size_t i = 0; i < C; ++i) {
        for (size_t j = 0; j < R; ++j)
            os << mat(i, j) << ' ';
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

#endif

#ifndef _OXYBELIS_MATH_MATRIX_H
#define _OXYBELIS_MATH_MATRIX_H

#include <cstddef>
#include <array>
#include <ostream>

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

    template <typename U>
    auto operator+(const Matrix<U, Rows, Cols>& other) {
        using Result = decltype(std::declval<T>() + std::declval<U>());
        Matrix<Result, Rows, Cols> result;

        for (size_t i = 0; i < Size; ++i)
            result[i] = (*this)[i] + other[i];

        return result;
    }

    template <typename U>
    auto operator+=(const Matrix<U, Rows, Cols>& other) {
        for (size_t i = 0; i < Size; ++i)
            (*this)[i] += other[i];

        return this->derived();
    }

    template <typename U>
    auto operator-(const Matrix<U, Rows, Cols>& other) {
        using Result = decltype(std::declval<T>() + std::declval<U>());
        Matrix<Result, Rows, Cols> result;

        for (size_t i = 0; i < Size; ++i)
            result[i] = (*this)[i] - other[i];

        return result;
    }

    template <typename U>
    auto operator-() {
        using Result = decltype(std::declval<T>() + std::declval<U>());
        Matrix<Result, Rows, Cols> result;

        for (size_t i = 0; i < Size; ++i)
            result[i] = -(*this)[i];
        return result;
    }

    template <typename U>
    auto operator-=(const Matrix<U, Rows, Cols>& other) {
        for (size_t i = 0; i < Size; ++i)
            (*this)[i] -= other[i];

        return this->derived();
    }

    auto operator*(const T& scalar) {
        using Result = decltype(std::declval<T>() * scalar);
        Matrix<Result, Rows, Cols> result;

        for (size_t i = 0; i < Size; ++i)
            result[i] = (*this)[i] * scalar;

        return result;
    }

    auto operator*=(const T& scalar) {
        for (size_t i = 0; i < Size; ++i)
            (*this)[i] *= scalar;

        return this->derived();
    }

    template <typename U, size_t OtherCols>
    auto operator*(const Matrix<U, Cols, OtherCols>& other) {
        using Result = decltype(std::declval<T>() * std::declval<U>());
        Matrix<Result, Rows, OtherCols> result({0});

        for (size_t k = 0; k < Cols; ++k) {
            for (size_t i = 0; i < Rows; ++i) {
                for (size_t j = 0; j < OtherCols; ++j) {
                    result(i, j) += (*this)(i, k) * other(k, j);
                }
            }
        }

        return result;
    }

private:
    Derived& derived() {
        return *static_cast<Derived*>(this);
    }

    const Derived& derived() const {
        return *static_cast<Derived*>(this);
    }
};

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

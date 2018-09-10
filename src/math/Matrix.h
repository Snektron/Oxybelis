#ifndef _OXYBELIS_MATH_MATRIX_H
#define _OXYBELIS_MATH_MATRIX_H

#include <cstddef>
#include <utility>
#include <array>
#include "Storage.h"

template <typename T, size_t R, size_t C, typename S>
class Matrix;

template <typename T, size_t R, size_t C, typename S>
class MatrixBase {
    using Derived = Matrix<T, R, C, S>;

public:
    using Type = T;
    using Storage = S;

    constexpr const static size_t Rows = R;
    constexpr const static size_t Cols = C;

    Storage elements;

    MatrixBase() = default;

    MatrixBase(const S& data):
        elements(data)
    {}

    MatrixBase(S&& data):
        elements(std::forward(data))
    {}

    constexpr size_t rows() const {
        return Rows;
    }

    constexpr size_t cols() const {
        return Cols;
    }

private:
    Derived& derived() {
        return *static_cast<Derived*>(this);
    }

    const Derived& derived() const {
        return *static_cast<const Derived*>(this);
    }
};

template <typename T, size_t R, size_t C, typename S = ColumnMajor<T, R, C>> 
class Matrix: public MatrixBase<T, R, C, S> {
    using Base = MatrixBase<T, R, C, S>;
public:
    Matrix() = default;

    Matrix(const S& data):
        Base(data)
    {}

    Matrix(S&& data):
        Base(std::forward(data))
    {}
};

#endif

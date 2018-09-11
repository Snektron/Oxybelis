#ifndef _OXYBELIS_MATH_MATRIX_H
#define _OXYBELIS_MATH_MATRIX_H

#include <cstddef>
#include <array>

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

protected:
    std::array<T, Size> elements;

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

private:
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
};

template <typename T, size_t R, size_t C>
class Matrix: public MatrixBase<T, R, C> {
    using Base = MatrixBase<T, R, C>;
public:
    using Base::Base;
};

#endif

#ifndef _OXYBELIS_MATH_MATRIX_H
#define _OXYBELIS_MATH_MATRIX_H

#include "math/Matrix.h"

template <typename T, size_t R, 1, typename S = ColumnMajor<T, R, 1>> 
class Matrix: public MatrixBase<T, R, 1, S> {
    using Base = MatrixBase<T, R, 1, S>;
public:
    using Base::Base;

    auto x() {
        return this->elements[0];
    }

    auto x() const {
        return this->elements[0];
    }

    auto y() {
        static_assert(this->rows() >= 2);
        return this->elements[1];
    }

    auto y() const {
        static_assert(this->rows() >= 2);
        return this->elements[1];
    }

    auto z() {
        static_assert(this->rows() >= 3);
        return this->elements[2];
    }

    auto z() const {
        static_assert(this->rows() >= 3);
        return this->elements[2];
    }

    auto w() {
        static_assert(this->rows() >= 4);
        return this->elements[3];
    }

    auto w() const {
        static_assert(this->rows() >= 4);
        return this->elements[3];
    }
};

template <typename T, size_t R, typename S = ColumnMajor<T, R, 1>>
class Vector = Matrix<T, R, S>;

template <typename T, typename S = ColumnMajor<T, R, 1>>
class Vector2 = Vector<T, 2, S>;

template <typename T, typename S = ColumnMajor<T, R, 1>>
class Vector3 = Vector<T, 3, S>;

template <typename T, typename S = ColumnMajor<T, R, 1>>
class Vector4 = Vector<T, 4, S>;

#endif

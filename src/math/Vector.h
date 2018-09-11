#ifndef _OXYBELIS_MATH_MATRIX_H
#define _OXYBELIS_MATH_MATRIX_H

#include "math/Matrix.h"

template <typename T, size_t R>
class Matrix: public MatrixBase<T, R, 1> {
    using Base = MatrixBase<T, R, 1>;
public:
    using Base::Base;

    T& x() {
        return (*this)[0];
    }

    const T& x() const {
        return (*this)[0];
    }

    T& y() {
        static_assert(R >= 2, "Vector does not have y component.");
        return (*this)[1];
    }

    const T& y() const {
        static_assert(R >= 2, "Vector does not have y component.");
        return (*this)[1];
    }

    T& z() {
        static_assert(R >= 2, "Vector does not have z component.");
        return (*this)[2];
    }

    const T& z() const {
        static_assert(R >= 2, "Vector does not have z component.");
        return (*this)[2];
    }

    T& w() {
        static_assert(R >= 4, "Vector does not have w component.");
        return (*this)[3];
    }

    const T& w() const {
        static_assert(R >= 4, "Vector does not have w component.");
        return (*this)[3];
    }
};

template <typename T, size_t R>
class Vector = Matrix<T, R, 1>;

#endif

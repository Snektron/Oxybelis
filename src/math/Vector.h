#ifndef _OXYBELIS_MATH_VERTEX_H
#define _OXYBELIS_MATH_VERTEX_H

#include <type_traits>
#include "math/Matrix.h"

template <typename T, size_t M>
class Vector: public MatrixBase<T, M, 1, ColumnMajor, Vector<T, M>> {
public:
    Vector() = default;

    Vector(const T(&init)[M]):
        MatrixBase<T, M, 1, ColumnMajor, Vector<T, M>>(init)
    {}

    Vector(T(&&init)[M]):
        MatrixBase<T, M, 1, ColumnMajor, Vector<T, M>>(init)
    {}

    T& x() {
        return this->get(0, 0);
    }

    const T& x() const {
        return this->get(0, 0);
    }

    T& y() {
        static_assert(M >= 2, "Vector does not have y component");
        return this->get(1, 0);
    }

    const T& y() const {
        static_assert(M >= 2, "Vector does not have y component");
        return this->get(1, 0);
    }

    T& z() {
        static_assert(M >= 3, "Vector does not have z component");
        return this->get(2, 0);
    }

    const T& z() const {
        static_assert(M >= 3, "Vector does not have z component");
        return this->get(2, 0);
    }

    T& w() {
        static_assert(M >= 4, "Vector does not have w component");
        return this->get(3, 0);
    }

    const T& w() const {
        static_assert(M >= 4, "Vector does not have w component");
        return this->get(3, 0);
    }
};

#endif

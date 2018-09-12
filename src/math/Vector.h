#ifndef _OXYBELIS_MATH_VECTOR_H
#define _OXYBELIS_MATH_VECTOR_H

#include <numeric>
#include <cmath>
#include "math/Matrix.h"

template <typename T, size_t R>
using Vector = Matrix<T, R, 1>;

template <typename T = double>
using Vector2 = Vector<T, 2>;

template <typename T = double>
using Vector3 = Vector<T, 3>;

template <typename T = double>
using Vector4 = Vector<T, 4>;

namespace op {
    template <typename T, typename U>
    struct Cross {
        using ScalarResult = decltype(std::declval<T>() * std::declval<U>() + std::declval<U>() * std::declval<T>());
        using Result = Vector<ScalarResult, 3>;
        using Lhs = Vector<T, 3>;
        using Rhs = Vector<U, 3>;

        void operator()(Result& dst, const Lhs& lhs, const Rhs& rhs) {
            Result result;
            result.x() = lhs.y() * rhs.z() - rhs.y() * lhs.z();
            result.y() = lhs.z() * rhs.x() - rhs.z() * lhs.x();
            result.z() = lhs.x() * rhs.y() - rhs.x() * lhs.y();
            dst = std::move(result);
        }
    };
}

template <typename T, size_t R>
class Matrix<T, R, 1>: public MatrixBase<T, R, 1> {
    using Base = MatrixBase<T, R, 1>;
public:
    constexpr const static size_t INDEX_X = 0;
    constexpr const static size_t INDEX_Y = 1;
    constexpr const static size_t INDEX_Z = 2;
    constexpr const static size_t INDEX_W = 3;

    using Base::Base;

    constexpr T& x() {
        return (*this)[INDEX_X];
    }

    constexpr const T& x() const {
        return (*this)[INDEX_X];
    }

    constexpr T& y() {
        static_assert(R + 1 >= INDEX_Y, "Vector does not have y component.");
        return (*this)[INDEX_Y];
    }

    constexpr const T& y() const {
        static_assert(R + 1 >= INDEX_Y, "Vector does not have y component.");
        return (*this)[INDEX_Y];
    }

    constexpr T& z() {
        static_assert(R + 1 >= INDEX_Z, "Vector does not have z component.");
        return (*this)[INDEX_Z];
    }

    constexpr const T& z() const {
        static_assert(R + 1 >= INDEX_Z, "Vector does not have z component.");
        return (*this)[INDEX_Z];
    }

    constexpr T& w() {
        static_assert(R + 1 >= INDEX_W, "Vector does not have w component.");
        return (*this)[INDEX_W];
    }

    constexpr const T& w() const {
        static_assert(R + 1 >= INDEX_W, "Vector does not have w component.");
        return (*this)[INDEX_W];
    }

    template <typename U>
    constexpr auto dot(const Vector<U, R>& other) const {
        return std::inner_product(this->begin(), this->end(), other.begin(), 0);
    }

    constexpr auto magnitude_sq() const {
        return std::inner_product(this->begin(), this->end(), this->begin(), 0);
    }

    constexpr double magnitude() const {
        return std::sqrt(this->magnitude_sq());
    }

    template <typename U>
    constexpr Vector<T, R>& cross(const Vector<U, R>& other) {
        using Cross = op::Cross<T, U>;
        static_assert(op::result_same<Cross, T>(), "Incompatible types in in-place vector cross product");
        Cross{}(*this, *this, other);
        return *this;
    }
};

template <typename T, typename U>
auto cross(const Vector<T, 3>& l, const Vector<U, 3>& r) {
    using Cross = op::Cross<T, U>;
    typename Cross::Result result;
    Cross{}(result, l, r);
    return result;
}

#endif

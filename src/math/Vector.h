#ifndef _OXYBELIS_MATH_VECTOR_H
#define _OXYBELIS_MATH_VECTOR_H

#include <numeric>
#include <cmath>
#include <type_traits>
#include "math/Matrix.h"

template <typename T, size_t R>
using Vector = Matrix<T, R, 1>;

template <typename T>
using Vector2 = Vector<T, 2>;

template <typename T>
using Vector3 = Vector<T, 3>;

template <typename T>
using Vector4 = Vector<T, 4>;

using Vector2f = Vector2<float>;
using Vector3f = Vector3<float>;
using Vector4f = Vector4<float>;

using Vector2d = Vector2<double>;
using Vector3d = Vector3<double>;
using Vector4d = Vector4<double>;

using Vector2i = Vector2<int>;
using Vector3i = Vector3<int>;
using Vector4i = Vector4<int>;

namespace op {
    template <typename T, typename U>
    struct Cross {
        using ScalarResult = decltype(std::declval<T>() * std::declval<U>() + std::declval<U>() * std::declval<T>());
        using Result = Vector<ScalarResult, 3>;
        using Lhs = Vector<T, 3>;
        using Rhs = Vector<U, 3>;

        constexpr void operator()(Result& dst, const Lhs& lhs, const Rhs& rhs) {
            Result result({
                lhs.y() * rhs.z() - rhs.y() * lhs.z(),
                lhs.z() * rhs.x() - rhs.z() * lhs.x(),
                lhs.x() * rhs.y() - rhs.x() * lhs.y()
            });

            dst = std::move(result);
        }
    };
}

template <typename T, size_t M>
class Matrix<T, M, 1>: public MatrixBase<T, M, 1> {
    using Base = MatrixBase<T, M, 1>;
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
        static_assert(M + 1 >= INDEX_Y, "Vector does not have y component.");
        return (*this)[INDEX_Y];
    }

    constexpr const T& y() const {
        static_assert(M + 1 >= INDEX_Y, "Vector does not have y component.");
        return (*this)[INDEX_Y];
    }

    constexpr T& z() {
        static_assert(M + 1 >= INDEX_Z, "Vector does not have z component.");
        return (*this)[INDEX_Z];
    }

    constexpr const T& z() const {
        static_assert(M + 1 >= INDEX_Z, "Vector does not have z component.");
        return (*this)[INDEX_Z];
    }

    constexpr T& w() {
        static_assert(M + 1 >= INDEX_W, "Vector does not have w component.");
        return (*this)[INDEX_W];
    }

    constexpr const T& w() const {
        static_assert(M + 1 >= INDEX_W, "Vector does not have w component.");
        return (*this)[INDEX_W];
    }

    template <typename U>
    constexpr auto dot(const Vector<U, M>& other) const {
        return std::inner_product(this->begin(), this->end(), other.begin(), 0);
    }

    constexpr auto magnitude_sq() const {
        return std::inner_product(this->begin(), this->end(), this->begin(), 0);
    }

    constexpr auto magnitude() const {
        return std::sqrt(this->magnitude_sq());
    }

    template <typename U>
    constexpr Vector<T, M>& cross(const Vector<U, M>& other) {
        using Cross = op::Cross<T, U>;
        static_assert(op::result_same<Cross, T>(), "Incompatible types in in-place vector cross product");
        Cross{}(*this, *this, other);
        return *this;
    }
};

template <typename T, typename U>
constexpr auto cross(const Vector3<T>& l, const Vector3<U>& r) {
    using Cross = op::Cross<T, U>;
    typename Cross::Result result;
    Cross{}(result, l, r);
    return result;
}

template <typename T, typename U, size_t N>
constexpr auto distance(const Vector<T, N>& lhs, const Vector<U, N>& rhs) {
    return (lhs - rhs).magnitude();
}

template <typename T, typename... Args>
constexpr auto make_vector(Args&&... args) {
    return Vector<T, sizeof...(Args)>({args...});
}

template <typename... Args>
constexpr auto make_vector(Args&&... args) {
    using Type = std::common_type_t<Args...>;
    return Vector<Type, sizeof...(Args)>({args...});
}

template <typename T>
constexpr auto make_translation(const Vector3<T>& translation) {
    return make_translation(translation.x(), translation.y(), translation.z());
}

template <typename T>
constexpr auto make_scaling(const Vector3<T>& scaling) {
    return make_scaling(scaling.x(), scaling.y(), scaling.z());
}

template <typename T>
constexpr auto make_rotation(const Vector3<T>& axis, const T& a) {
    return make_rotation(axis.x(), axis.y(), axis.z(), a);
}

#endif

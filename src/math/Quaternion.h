#ifndef _OXYBELIS_MATH_QUATERNION_H
#define _OXYBELIS_MATH_QUATERNION_H

#include <type_traits>
#include <cmath>
#include "math/Matrix.h"
#include "math/Vector.h"

template <typename T>
struct Quaternion;

using Quaternionf = Quaternion<float>;
using Quaterniond = Quaternion<double>;

namespace op {
    template <typename T, typename U, typename V>
    constexpr void product(Quaternion<T>& dst, const Quaternion<U>& lhs, const Quaternion<V>& rhs) {
        T a = lhs.a * rhs.a - lhs.b * rhs.b - lhs.c * rhs.c - lhs.d * rhs.d;
        T b = lhs.a * rhs.b + lhs.b * rhs.a + lhs.c * rhs.d - lhs.d * rhs.c;
        T c = lhs.a * rhs.c - lhs.b * rhs.d + lhs.c * rhs.a + lhs.d * rhs.b;
        T d = lhs.a * rhs.d + lhs.b * rhs.c - lhs.c * rhs.b + lhs.d * rhs.a;

        dst.a = a;
        dst.b = b;
        dst.c = c;
        dst.d = d;
    }

    // template <typename T, typename U, typename V>
    // constexpr void product(Vector3<T>& dst, const Quaternion<U>& lhs, const Vector3<V>& rhs) {
    //     T b = lhs.a * rhs.b + lhs.b * rhs.a + lhs.c * rhs.d - lhs.d * rhs.c;
    //     T c = lhs.a * rhs.c - lhs.b * rhs.d + lhs.c * rhs.a + lhs.d * rhs.b;
    //     T d = lhs.a * rhs.d + lhs.b * rhs.c - lhs.c * rhs.b + lhs.d * rhs.a;

    //     dst.x() = b;
    //     dst.y() = c;
    //     dst.z() = d;
    // }

    template <typename T, typename U, typename V>
    constexpr void scale(Quaternion<T>& dst, const Quaternion<U>& lhs, const V& rhs) {
        dst.a = lhs.a * rhs;
        dst.b = lhs.d * rhs;
        dst.c = lhs.c * rhs;
        dst.d = lhs.d * rhs;
    }

    template <typename T, typename U, typename V>
    constexpr void add(Quaternion<T>& dst, const Quaternion<U>& lhs, const Quaternion<V>& rhs) {
        dst.a = lhs.a + rhs.a;
        dst.b = lhs.d + rhs.b;
        dst.c = lhs.c + rhs.c;
        dst.d = lhs.d + rhs.d;
    }

    template <typename T, typename U, typename V>
    constexpr void sub(Quaternion<T>& dst, const Quaternion<U>& lhs, const Quaternion<V>& rhs) {
        dst.a = lhs.a - rhs.a;
        dst.b = lhs.d - rhs.b;
        dst.c = lhs.c - rhs.c;
        dst.d = lhs.d - rhs.d;
    }

    template <typename T, typename U, typename V>
    constexpr void conjugate(Quaternion<T>& dst, const Quaternion<U>& lhs) {
        dst.a = lhs.a;
        dst.b = -dst.b;
        dst.c = -dst.c;
        dst.d = -dst.d;
    }
}

template <typename T>
struct Quaternion {
    T a, b, c, d;

    constexpr Quaternion() = default;

    constexpr Quaternion(const T& a, const T& b, const T& c, const T& d):
        a(a), b(b), c(c), d(d) {
    }

    constexpr Quaternion(const T& scalar, const Vector3<T>& v):
        a(scalar), b(v.x()), c(v.y()), d(v.z()) {
    }

    template <typename U>
    constexpr Quaternion<T>& operator*=(const Quaternion<U> other) {
        op::product(*this, *this, other);
        return *this;
    }

    template <typename U>
    constexpr Quaternion<T>& operator*=(const U& other) {
        op::scale(*this, *this, other);
        return *this;
    }

    template <typename U>
    constexpr Quaternion<T>& operator+=(const Quaternion<U> other) {
        op::add(*this, *this, other);
        return *this;
    }

    template <typename U>
    constexpr Quaternion<T>& operator-=(const Quaternion<U> other) {
        op::sub(*this, *this, other);
        return *this;
    }

    constexpr auto norm_sq() const {
        return this->a * this->a + this->b * this->b + this->c * this->c + this->d * this->d;
    }

    constexpr auto norm() const {
        return std::sqrt(this->norm_sq);
    }

    constexpr Quaternion<T>& conjugate() {
        op::conjugate(*this, *this);
        return *this;
    }

    constexpr Matrix4<T> as_matrix() const {
        return {
             this->a,  this->b,  this->c,  this->d,
            -this->b,  this->a,  this->d, -this->c,
            -this->c, -this->d,  this->a,  this->b,
            -this->d,  this->c, -this->b,  this->a
        };
    }

    constexpr Matrix4<T> rotation_matrix() const {
        auto result = make_zeroes<Matrix4<T>>();

        auto qr = this->a;
        auto qi = this->b;
        auto qj = this->c;
        auto qk = this->d;

        //result(0, 0) = qr * qr + qi * qi - qj * qj - qk * qk;
        //result(1, 1) = qr * qr - qi * qi + qj * qj - qk * qk;
        //result(2, 2) = qr * qr - qi * qi - qj * qj + qk * qk; 
        
        result(0, 0) = 1. - 2. * (qj * qj + qk * qk);
        result(1, 1) = 1. - 2. * (qi * qi + qk * qk);
        result(2, 2) = 1. - 2. * (qi * qi + qj * qj);

        result(1, 0) = 2. * (qi * qj + qr * qk);
        result(2, 0) = 2. * (qi * qk - qr * qj);

        result(0, 1) = 2. * (qi * qj - qr * qk);
        result(2, 1) = 2. * (qr * qi + qj * qk);

        result(0, 2) = 2. * (qr * qj + qi * qk);
        result(1, 2) = 2. * (qj * qk - qr * qi);

        result(3, 3) = 1.;
        return result;
    }
};

template <typename T, typename U>
constexpr auto operator*(const Quaternion<T>& lhs, const Quaternion<U>& rhs) {
    using Result = decltype(std::declval<T>() * std::declval<U>() + std::declval<U>() * std::declval<T>());
    Quaternion<Result> result;
    op::product(result, lhs, rhs);
    return result;
}

// template <typename T, typename U>
// constexpr auto operator*(const Quaternion<T>& lhs, const Vector3<U>& rhs) {
//     using Result = decltype(std::declval<T>() * std::declval<U>() + std::declval<U>() * std::declval<T>());
//     Vector3<Result> result;
//     op::product(result, lhs, rhs);
//     return result;
// }

template <typename T, typename U>
constexpr auto operator*(const Quaternion<T>& lhs, const U& rhs) {
    using Result = decltype(std::declval<T>() * rhs);
    Quaternion<Result> result;
    op::scale(result, lhs, rhs);
    return result;
}

template <typename T, typename U>
constexpr auto operator+(const Quaternion<T>& lhs, const Quaternion<U>& rhs) {
    using Result = decltype(std::declval<T>() + std::declval<U>());
    Quaternion<Result> result;
    op::add(result, lhs, rhs);
    return result;
}

template <typename T, typename U>
constexpr auto operator-(const Quaternion<T>& lhs, const Quaternion<U>& rhs) {
    using Result = decltype(std::declval<T>() - std::declval<U>());
    Quaternion<Result> result;
    op::sub(result, lhs, rhs);
    return result;
}

template <typename T>
constexpr auto conjugate(const Quaternion<T>& q) {
    Quaternion<T> result;
    op::conjugate(result, q);
    return result;
}

template <typename T, typename U>
constexpr auto distance(const Quaternion<T>& lhs, const Quaternion<U>& rhs) {
    return (lhs - rhs).norm();
}

template <typename T>
constexpr auto make_rotation_quat(const T& x, const T& y, const T& z, const T& a) {
    T half = 0.5;
    auto cosa2 = std::cos(a * half);
    auto sina2 = std::sin(a * half);

    return Quaternion<T>(cosa2, sina2 * x, sina2 * y, sina2 * z);
}

#endif

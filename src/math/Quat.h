#ifndef _OXYBELIS_MATH_QUAT_H
#define _OXYBELIS_MATH_QUAT_H

#include <cmath>
#include "math/Mat.h"
#include "math/Vec.h"

const static double SLERP_THRESHOLD = 0.9995;

template <typename T>
struct Quat;

using QuatF = Quat<float>;
using QuatD = Quat<double>;

template <typename T>
struct Quat {
    union {
        struct {
            T x, y, z, w;
        };
        struct {
            Vec3<T> vector;
            T scalar;
        };
        Vec4<T> elements;
    };

    constexpr static Quat<T> identity() {
        return Quat<T>(0, 0, 0, 1);
    }

    constexpr Quat():
        x(0), y(0), z(0), w(0) {
    }

    constexpr Quat(const T& x, const T& y, const T& z, const T& w):
        x(x), y(y), z(z), w(w) {
    }

    constexpr Quat(const Vec3<T>& vector, const T& scalar):
        vector(vector), scalar(scalar) {
    }

    constexpr Quat(const Vec4<T>& elements):
        elements(elements) {
    }

    constexpr const T* data() const {
        return this->elements.data();
    }

    template <typename U>
    constexpr Quat<T>& operator+=(const U& u);

    template <typename U>
    constexpr Quat<T>& operator-=(const U& u);

    template <typename U>
    constexpr Quat<T>& operator*=(const U& u);

    template <typename U>
    constexpr Quat<T>& operator/=(const U& u);

    constexpr Quat<T>& conjugate();

    constexpr Quat<T>& invert();

    constexpr Quat<T>& normalize();

    template <typename U>
    constexpr Quat<T>& mix(const Quat<U>& other);

    template <typename U>
    constexpr Quat<T>& smix(const Quat<U>& other);

    constexpr Mat4<T> to_matrix() const;
};

namespace quat {
    template <typename T>
    auto make(const T& x, const T& y, const T& z, const T& w) {
        return Quat<T>(x, y, z, w);
    }

    template <typename T>
    auto make(const Vec4<T>& v) {
        return Quat<T>(v);
    }

    template <typename T>
    auto make(const Vec3<T>& v, const T& s) {
        return Quat<T>(v, s);
    }

    template <typename T>
    constexpr auto axis_angle(const Vec3<T>& axis, const T& a) {
        T half = 0.5;
        auto cosa2 = std::cos(a * half);
        auto sina2 = std::sin(a * half);

        return make(axis * sina2, cosa2);
    }

    template <typename T>
    constexpr auto axis_angle(const T& x, const T& y, const T& z, const T& a) {
        return axis_angle(Vec3<T>(x, y, z), a);
    }

    template <typename T>
    constexpr auto extract(const Mat4<T>& m) {
        auto w = std::sqrt(1 + m(0, 0) + m(1, 1) + m(2, 2));
        auto w2 = w + w;
        return make(
            (m(2, 1) - m(1, 2)) / w2,
            (m(0, 2) - m(2, 1)) / w2,
            (m(1, 0) - m(0, 1)) / w2,
            w / 2
        );
    }
};

template <typename T, typename U>
constexpr auto operator+(const Quat<T>& lhs, const Quat<U>& rhs) {
    return quat::make(lhs.elements + rhs.elements);
}

template <typename T, typename U>
constexpr auto operator+(const T& lhs, const Quat<U>& rhs) {
    return quat::make(lhs + rhs.elements);
}

template <typename T, typename U>
constexpr auto operator+(const Quat<T>& lhs, const U& rhs) {
    return quat::make(lhs.elements + rhs);
}

template <typename T, typename U>
constexpr auto operator-(const Quat<T>& lhs, const Quat<U>& rhs) {
    return quat::make(lhs.elements - rhs.elements);
}

template <typename T, typename U>
constexpr auto operator-(const T& lhs, const Quat<U>& rhs) {
    return quat::make(lhs - rhs.elements);
}

template <typename T, typename U>
constexpr auto operator-(const Quat<T>& lhs, const U& rhs) {
    return quat::make(lhs.elements - rhs);
}

template <typename T>
constexpr auto operator-(const Quat<T>& q) {
    return quat::make(-q.elements);
}

template <typename T, typename U>
constexpr auto operator*(const Quat<T>& lhs, const Quat<U>& rhs) {
    return quat::make(
        lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x,
        lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w,
        lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z
    );
}

template <typename T, typename U>
constexpr auto operator*(const Quat<T>& lhs, const Vec3<U>& rhs) {
    return (lhs * quat::make<U>(rhs, 0)).vector;
}

template <typename T, typename U>
constexpr auto operator*(const T& lhs, const Quat<U>& rhs) {
    return quat::make(lhs * rhs.elements);
}

template <typename T, typename U>
constexpr auto operator*(const Quat<T>& lhs, const U& rhs) {
    return quat::make(lhs.elements * rhs);
}

template <typename T, typename U>
constexpr auto operator/(const T& lhs, const Quat<U>& rhs) {
    return quat::make(lhs / rhs.elements);
}

template <typename T, typename U>
constexpr auto operator/(const Quat<T>& lhs, const U& rhs) {
    return quat::make(lhs.elements / rhs);
}

template <typename Os, typename T>
Os& operator<<(Os& os, const Quat<T>& q) {
    auto p = [&os](const T& x, char c) {
        os << (x < 0 ? " - " : " + ") << std::abs(x) << c;
    };

    os << q.w;
    p(q.x, 'i');
    p(q.y, 'j');
    p(q.z, 'k');
    return os;
}

template <typename T>
constexpr auto conjugate(const Quat<T>& q) {
    return quat::make(-q.vector, q.scalar);
}

template <typename T>
constexpr auto length_sq(const Quat<T>& q) {
    return length_sq(q.elements);
}

template <typename T>
constexpr auto length(const Quat<T>& q) {
    return length(q.elements);
}

template <typename T>
constexpr auto inverse(const Quat<T>& q) {
    return conjugate(q) / length_sq(q);
}

template <typename T>
constexpr auto normalize(const Quat<T>& q) {
    return q / length(q);
}

template <typename T, typename U, typename V>
constexpr auto mix(const Quat<T>& lhs, const Quat<U>& rhs, V t) {
    return lhs + t * (rhs - lhs);
}

template <typename T, typename U, typename V>
constexpr auto smix(const Quat<T>& lhs, const Quat<U>& rhs, V t) {
    auto dot = ::dot(lhs.vector, rhs.vector);
    auto q = normalize(rhs);

    if (dot < static_cast<T>(0)) {
        dot = -dot;
        q = -q;
    }

    if (dot > static_cast<T>(SLERP_THRESHOLD))
        return normalize(mix(lhs, q, t));

    auto theta_0 = std::acos(dot);
    auto theta = theta_0 * t;
    auto sin_theta = std::sin(theta);
    auto sin_theta_0 = std::sin(theta_0);

    auto s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;
    auto s1 = sin_theta / sin_theta_0;

    return normalize(s0 * lhs + s1 * q);
}

template <typename T, typename U>
constexpr auto distance_sq(const Quat<T>& lhs, const Quat<U>& rhs) {
    return length_sq(lhs - rhs);
}

template <typename T, typename U>
constexpr auto distance(const Quat<T>& lhs, const Quat<U>& rhs) {
    return length(lhs - rhs);
}

template <typename T>
template <typename U>
constexpr Quat<T>& Quat<T>::operator+=(const U& u) {
    return *this = *this + u;
}

template <typename T>
template <typename U>
constexpr Quat<T>& Quat<T>::operator-=(const U& u) {
    return *this = *this - u;
}

template <typename T>
template <typename U>
constexpr Quat<T>& Quat<T>::operator*=(const U& u) {
    return *this = *this * u;
}

template <typename T>
template <typename U>
constexpr Quat<T>& Quat<T>::operator/=(const U& u) {
    return *this = *this / u;
}

template <typename T>
constexpr Quat<T>& Quat<T>::conjugate() {
    return *this = ::conjugate(*this);
}

template <typename T>
constexpr Quat<T>& Quat<T>::invert() {
    return *this = ::inverse(*this);
}

template <typename T>
constexpr Quat<T>& Quat<T>::normalize() {
    return *this = ::normalize(*this);
}

template <typename T>
template <typename U>
constexpr Quat<T>& Quat<T>::mix(const Quat<U>& other) {
    return *this = mix(*this, other);
}

template <typename T>
template <typename U>
constexpr Quat<T>& Quat<T>::smix(const Quat<U>& other) {
    return *this = smix(*this, other);
}

template <typename T>
constexpr Mat4<T> Quat<T>::to_matrix() const {
    Mat4<T> result;

    auto qr = this->w;
    auto qi = this->x;
    auto qj = this->y;
    auto qk = this->z;

    auto one = static_cast<T>(1);
    auto dbl = [](auto x){ return x + x; };

    result(0, 0) = one - dbl(qj * qj + qk * qk);
    result(1, 1) = one - dbl(qi * qi + qk * qk);
    result(2, 2) = one - dbl(qi * qi + qj * qj);

    result(1, 0) = dbl(qi * qj + qr * qk);
    result(2, 0) = dbl(qi * qk - qr * qj);

    result(0, 1) = dbl(qi * qj - qr * qk);
    result(2, 1) = dbl(qr * qi + qj * qk);

    result(0, 2) = dbl(qr * qj + qi * qk);
    result(1, 2) = dbl(qj * qk - qr * qi);

    result(3, 3) = one;
    return result;
}

#endif

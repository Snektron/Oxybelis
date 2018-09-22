#ifndef _OXYBELIS_MATH_TRANSFORM_H
#define _OXYBELIS_MATH_TRANSFORM_H

#include "math/Vec.h"
#include "math/Quat.h"
#include "math/Mat.h"

template <typename T>
struct Transform {
    Vec3<T> translation;
    Vec3<T> scale;
    Quat<T> rotation;

    Transform():
        scale(1) {
    }

    Transform(const Vec3<T>& translation, const Vec3<T>& scale, const Quat<T>& rotation):
        translation(translation),
        scale(scale),
        rotation(rotation) {
    }

    Transform(const Vec3<T>& translation, const Quat<T>& rotation):
        translation(translation),
        scale(1),
        rotation(rotation) {
    }

    Mat4<T> to_matrix() const;
};

using TransformF = Transform<float>;

template <typename T>
Mat4<T> Transform<T>::to_matrix() const {
    Mat4<T> result;

    auto qr = this->rotation.w;
    auto qi = this->rotation.x;
    auto qj = this->rotation.y;
    auto qk = this->rotation.z;

    auto one = static_cast<T>(1);
    auto dbl = [](auto x){ return x + x; };

    result(0, 0) = (one - dbl(qj * qj + qk * qk)) * this->scale.x;
    result(1, 1) = (one - dbl(qi * qi + qk * qk)) * this->scale.y;
    result(2, 2) = (one - dbl(qi * qi + qj * qj)) * this->scale.z;

    result(1, 0) = dbl(qi * qj + qr * qk) * this->scale.x;
    result(2, 0) = dbl(qi * qk - qr * qj) * this->scale.x;

    result(0, 1) = dbl(qi * qj - qr * qk) * this->scale.y;
    result(2, 1) = dbl(qr * qi + qj * qk) * this->scale.y;

    result(0, 2) = dbl(qr * qj + qi * qk) * this->scale.z;
    result(1, 2) = dbl(qj * qk - qr * qi) * this->scale.z;

    result.column(3).xyz = this->translation;
    result(3, 3) = one;
    return result;
}

#endif

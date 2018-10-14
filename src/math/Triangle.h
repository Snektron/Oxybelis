#ifndef _OXYBELIS_MATH_TRIANGLE_H
#define _OXYBELIS_MATH_TRIANGLE_H

#include "math/Vec.h"

template <typename T>
struct Triangle {
    Vec3<T> a, b, c;

    Triangle(const Vec3<T>& a, const Vec3<T>& b, const Vec3<T>& c):
        a(a), b(b), c(c) {
    }

    Vec3<T> face_normal() {
        return normalize(cross(this->c - this->a, this->b - this->a));
    }

    Vec3<T> center() {
        return (this->a + this->b + this->c) / static_cast<T>(3);
    }
};

using TriangleF = Triangle<float>;

#endif

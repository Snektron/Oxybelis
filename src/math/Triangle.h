#ifndef _OXYBELIS_MATH_TRIANGLE_H
#define _OXYBELIS_MATH_TRIANGLE_H

#include <cmath>
#include <array>
#include "math/Vec.h"

template <typename T>
struct Triangle {
    union {
        struct {
            Vec3<T> a, b, c;
        };

        std::array<Vec3<T>, 3> points;
    };

    constexpr Triangle(const Vec3<T>& a, const Vec3<T>& b, const Vec3<T>& c):
        a(a), b(b), c(c) {
    }

    constexpr Vec3<T> face_normal() {
        return normalize(cross(this->c - this->a, this->b - this->a));
    }

    constexpr Vec3<T> center() {
        return (this->a + this->b + this->c) / static_cast<T>(3);
    }

    /// Classify the location of a point with respect to the triangle,
    /// as if the triangle lays on a sphere at the origin.
    /// 0 means the point is above or below the triangle, 
    /// 1 means the point is between sides a-b,
    /// 2 between b-c and 3 between c-a.
    constexpr int sphere_classify(const Vec3<T>& p) {
        auto test = [&](const auto& x, const auto& y) {
            auto pn = cross(y, x);
            return std::signbit(dot(p, pn));
        };

        if (test(this->a, this->b))
            return 1;
        else if (test(this->b, this->c))
            return 2;
        else if (test(this->c, this->a))
            return 3;
        else
            return 0;
    }

    /// Orient the triangle in such a way that p lies on side a-b
    /// p must lie outside the triangle
    constexpr Triangle<T>& orient(const Vec3<T>& p) {
        auto test = [&](const auto& x, const auto& y) {
            auto pn = cross(y, x);
            return std::signbit(dot(p, pn));
        };

        if (test(this->b, this->c)) {
            // p is on b-c, rotate clockwise            
            auto tmp = this->a;
            this->a = this->b;
            this->b = this->c;
            this->c = tmp;
        } else if (test(this->c, this->a)) {
            // p is on c-a, rotate counter clockwise
            auto tmp = this->c;
            this->c = this->b;
            this->b = this->a;
            this->a = tmp;
        }

        return *this;
    }
};

using TriangleF = Triangle<float>;

#endif

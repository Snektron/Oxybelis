#ifndef _OXYBELIS_GRAPHICS_CAMERA_PROJECTION_H
#define _OXYBELIS_GRAPHICS_CAMERA_PROJECTION_H

#include "math/Mat.h"
#include "math/Vec.h"

struct Perspective {
    float aspect, fov, znear, zfar;

    constexpr Perspective(float aspect, float fov, float znear, float zfar):
        aspect(aspect), fov(fov), znear(znear), zfar(zfar) {
    }

    constexpr Perspective(float width, float height, float fov, float znear, float zfar):
        aspect(width / height), fov(fov), znear(znear), zfar(zfar) {
    }

    constexpr Perspective(Vec2F dim, float fov, float znear, float zfar):
        aspect(dim.x / dim.y), fov(fov), znear(znear), zfar(zfar) {
    }

    constexpr void resize(float width, float height) {
        this->aspect = width / height;
    }

    constexpr void resize(Vec2F dim) {
        this->aspect = dim.x / dim.y;
    }

    constexpr Mat4F to_matrix() const;

    constexpr Mat4F to_inverse_matrix() const;
};

struct Orthographic {
    float left, right;
    float top, bottom;
    float znear, zfar;

    constexpr Orthographic(float left, float right, float top, float bottom, float znear, float zfar):
        left(left), right(right), top(top), bottom(bottom), znear(znear), zfar(zfar) {
    }

    constexpr Mat4F to_matrix() const;
};


constexpr Mat4F Perspective::to_matrix() const {
    return Mat4F::perspective(this->aspect, this->fov, this->znear, this->zfar);
}

constexpr Mat4F Perspective::to_inverse_matrix() const {
    Mat4F result;

    float nf = this->znear - this->zfar;
    float tan_fov_2 = std::tan(this->fov / 2.f);
    float nf2 = 2 * this->znear * this->zfar;

    result(0, 0) = this->aspect * tan_fov_2;
    result(1, 1) = tan_fov_2;
    result(2, 3) = 1;
    result(3, 2) = nf / nf2;
    result(3, 3) = (this->znear + this->zfar) / nf2;

    return result;
}

constexpr Mat4F Orthographic::to_matrix() const {
    return Mat4F::orthographic(this->left, this->right, this->top, this->bottom, this->znear, this->zfar);
}

#endif

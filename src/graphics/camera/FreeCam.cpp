#include "graphics/camera/FreeCam.h"

FreeCam::FreeCam(const QuatF& rotation, const Vec3F& translation):
    rotation(rotation), translation(translation) {
}

Mat4F FreeCam::to_view_matrix() const {
    return normalize(this->rotation).conjugate().to_matrix() * mat::translation(-this->translation);
}
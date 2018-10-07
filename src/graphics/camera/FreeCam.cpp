#include "graphics/camera/FreeCam.h"

FreeCam::FreeCam(const QuatF& rotation, const Vec3F& translation):
    rotation(rotation), translation(translation) {
}

void FreeCam::rotate_pitch(float amount) {
    this->rotation *= QuatF::axis_angle(1, 0, 0, amount);
    this->rotation.normalize();
}

void FreeCam::rotate_yaw(float amount) {
    this->rotation *= QuatF::axis_angle(0, 1, 0, amount);
    this->rotation.normalize();
}

void FreeCam::rotate_roll(float amount) {
    this->rotation *= QuatF::axis_angle(0, 0, 1, amount);
    this->rotation.normalize();
}

void FreeCam::forward(float amount) {
    this->translation += this->rotation.to_matrix().column(2).xyz * amount;
}

void FreeCam::strafe(float amount) {
    this->translation += this->rotation.to_matrix().column(0).xyz * amount;
}

void FreeCam::fly(float amount) {
    this->translation += this->rotation.to_matrix().column(1).xyz * amount;
}

Mat4F FreeCam::to_view_matrix() const {
    return normalize(this->rotation).conjugate().to_matrix() * Mat4F::translation(-this->translation);
}

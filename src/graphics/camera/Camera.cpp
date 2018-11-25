#include "graphics/camera/Camera.h"

Camera::Camera(const QuatD& rotation, const Vec3D& translation):
    rotation(rotation), translation(translation) {
}

void Camera::rotate_pitch(double amount) {
    this->rotation *= QuatD::axis_angle(1, 0, 0, amount);
    this->rotation.normalize();
}

void Camera::rotate_yaw(double amount) {
    this->rotation *= QuatD::axis_angle(0, 1, 0, amount);
    this->rotation.normalize();
}

void Camera::rotate_roll(double amount) {
    this->rotation *= QuatD::axis_angle(0, 0, 1, amount);
    this->rotation.normalize();
}

void Camera::forward(double amount) {
    this->translation += this->rotation.forward() * amount;
}

void Camera::strafe(double amount) {
    this->translation += this->rotation.right() * amount;
}

void Camera::fly(double amount) {
    this->translation += this->rotation.up() * amount;
}

Mat4D Camera::to_view_matrix() const {
    return this->rotation.to_view_matrix() * Mat4D::translation(-this->translation);
}

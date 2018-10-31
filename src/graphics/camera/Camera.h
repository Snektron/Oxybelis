#ifndef _OXYBELIS_GRAPHICS_CAMERA_FREECAM_H
#define _OXYBELIS_GRAPHICS_CAMERA_FREECAM_H

#include "math/Mat.h"
#include "math/Vec.h"
#include "math/Quat.h"

struct Camera {
    QuatD rotation;
    Vec3D translation;

    Camera(const QuatD& rotation, const Vec3D& translation);

    void rotate_pitch(double amount);
    void rotate_yaw(double amount);
    void rotate_roll(double amount);

    void forward(double amount);
    void strafe(double amount);
    void fly(double amount);

    Mat4D to_view_matrix() const;
};

#endif

#ifndef _OXYBELIS_GRAPHICS_CAMERA_FREECAM_H
#define _OXYBELIS_GRAPHICS_CAMERA_FREECAM_H

#include "math/Mat.h"
#include "math/Vec.h"
#include "math/Quat.h"

struct FreeCam {
    QuatF rotation;
    Vec3F translation;

    FreeCam(const QuatF& rotation, const Vec3F& translation);

    Mat4F to_view_matrix() const;
};

#endif

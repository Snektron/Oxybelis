#ifndef _OXYBELIS_PLANET_RENDER_RENDERINFO_H
#define _OXYBELIS_PLANET_RENDER_RENDERINFO_H

#include "math/Mat.h"

struct Camera;

struct RenderInfo {
    const Camera& cam;
    Mat4F proj;
    Mat4F inv_proj;
    Mat4F view;
};

#endif

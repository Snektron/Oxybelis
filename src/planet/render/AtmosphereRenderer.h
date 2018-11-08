#ifndef _OXYBELIS_PLANET_RENDER_ATMOSPHERE_H
#define _OXYBELIS_PLANET_RENDER_ATMOSPHERE_H

#include "planet/Planet.h"
#include "graphics/camera/Camera.h"
#include "graphics/VertexArray.h"
#include "graphics/shader/Program.h"
#include "graphics/Buffer.h"
#include "math/Mat.h"

class AtmosphereRenderer {
    VertexArray vao;
    Buffer quad;

    Program shader;
    Uniform inv_proj;
    Uniform model;
    Uniform camera_origin;
    Uniform camera_up;
    Uniform camera_dir;

public:
    AtmosphereRenderer();
    void render(const Mat4F& inv_proj, const Camera& cam);
};

#endif

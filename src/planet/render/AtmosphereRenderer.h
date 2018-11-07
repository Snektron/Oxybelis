#ifndef _OXYBELIS_PLANET_RENDER_ATMOSPHERE_H
#define _OXYBELIS_PLANET_RENDER_ATMOSPHERE_H

#include "planet/Planet.h"
#include "graphics/camera/Camera.h"
#include "graphics/VertexArray.h"
#include "graphics/shader/Program.h"
#include "graphics/Buffer.h"
#include "math/Mat.h"

class AtmosphereRenderer {
    const Planet& planet;

    VertexArray vao;
    Buffer quad;

    Program shader;

public:
    AtmosphereRenderer(const Planet& planet);
    void render(const Mat4F& proj, const Camera& cam);
};

#endif

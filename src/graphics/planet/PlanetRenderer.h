#ifndef _OXYBELIS_GRAPHICS_PLANET_PLANETRENDERER_H
#define _OXYBELIS_GRAPHICS_PLANET_PLANETRENDERER_H

#include <cstddef>
#include "graphics/planet/Planet.h"
#include "graphics/camera/FreeCam.h"
#include "graphics/shader/Program.h"
#include "graphics/Buffer.h"
#include "graphics/VertexArray.h"
#include "math/Vec.h"
#include "math/Mat.h"

struct Node {
    Vec3F a, b, c;
};

class PlanetRenderer {
    Program shader;
    const Planet& planet;

    VertexArray vao;
    Buffer vertices;
    size_t num_vertices;

    Uniform perspective;
    Uniform model;

public:
    PlanetRenderer(const Planet& planet);

    void render(const Mat4F& proj, const FreeCam& cam);
};

#endif

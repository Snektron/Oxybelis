#ifndef _OXYBELIS_PLANET_RENDER_PLANETRENDERER_H
#define _OXYBELIS_PLANET_RENDER_PLANETRENDERER_H

#include <vector>
#include <cstddef>
#include "math/Vec.h"
#include "graphics/camera/FreeCam.h"
#include "graphics/shader/Program.h"
#include "planet/Chunk.h"

class PlanetRenderer {
    Program shader;
    Uniform perspective;
    Uniform model;

public:
    PlanetRenderer();
    void render(const Mat4F& proj, const FreeCam& cam);
};

#endif

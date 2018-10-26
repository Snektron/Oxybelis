#ifndef _OXYBELIS_PLANET_RENDER_PLANETRENDERER_H
#define _OXYBELIS_PLANET_RENDER_PLANETRENDERER_H

#include <vector>
#include <cstddef>
#include "math/Vec.h"
#include "graphics/camera/FreeCam.h"
#include "graphics/shader/Program.h"

struct Planet;

class PlanetRenderer {
    Program shader;
    Uniform perspective;
    Uniform model;

    Planet& planet;
public:
    PlanetRenderer(Planet& planet);
    void render(const Mat4F& proj, const FreeCam& cam);
};

#endif

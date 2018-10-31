#ifndef _OXYBELIS_PLANET_RENDER_PLANETRENDERER_H
#define _OXYBELIS_PLANET_RENDER_PLANETRENDERER_H

#include <vector>
#include <cstddef>
#include "math/Vec.h"
#include "math/Mat.h"
#include "graphics/camera/Camera.h"
#include "graphics/shader/Program.h"
#include "planet/ChunkCache.h"
#include "planet/ChunkPatch.h"
#include "utility/Option.h"

struct Planet;

class PlanetRenderer {
    Program shader;
    Uniform perspective;
    Uniform model;

    Planet& planet;
    ChunkCache cache;
    Option<ChunkPatch> patch;
public:
    PlanetRenderer(Planet& planet);
    void render(const Mat4F& proj, const Camera& cam);
};

#endif

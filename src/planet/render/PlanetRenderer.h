#ifndef _OXYBELIS_PLANET_RENDER_PLANETRENDERER_H
#define _OXYBELIS_PLANET_RENDER_PLANETRENDERER_H

#include "math/Vec.h"
#include "graphics/camera/FreeCam.h"
#include "planet/ChunkId.h"

class PlanetRenderer {
public:
    void render(const Mat4F& proj, const FreeCam& cam);
};

#endif

#ifndef _OXYBELIS_PLANET_RENDER_PLANETRENDERER_H
#define _OXYBELIS_PLANET_RENDER_PLANETRENDERER_H

#include "math/Vec.h"
#include "graphics/camera/FreeCam.h"

struct Chunk {
    Vec3F a, b, c;

    void render(const Mat4F& proj, const FreeCam& cam);
};

class PlanetRenderer {
    Chunk current;
    Chunk inner_neighbors[12];
    Chunk outer_neighbors[24];

public:
    void render(const Mat4F& proj, const FreeCam& cam);
};

#endif

#ifndef _OXYBELIS_PLANET_RENDER_PLANETRENDERER_H
#define _OXYBELIS_PLANET_RENDER_PLANETRENDERER_H

#include <vector>
#include "math/Vec.h"
#include "graphics/camera/FreeCam.h"
#include "graphics/shader/Program.h"
#include "planet/Chunk.h"

class PlanetRenderer {
    Program shader;
    Uniform perspective;
    Uniform model;
    std::vector<Chunk> chunks;

public:
    PlanetRenderer();
    void generate_patch(ChunkId center);
    void rec(ChunkId center, ChunkId current);
    void render(const Mat4F& proj, const FreeCam& cam);
};

#endif

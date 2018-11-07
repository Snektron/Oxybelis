#ifndef _OXYBELIS_PLANET_RENDER_PLANETRENDERER_H
#define _OXYBELIS_PLANET_RENDER_PLANETRENDERER_H

#include <vector>
#include <cstddef>
#include "math/Vec.h"
#include "math/Mat.h"
#include "graphics/camera/Camera.h"
#include "graphics/shader/Program.h"
#include "planet/terragen/TerrainGenerator.h"
#include "planet/chunk/ChunkLoader.h"
#include "planet/chunk/ChunkPatch.h"
#include "utility/Option.h"

struct Planet;

class TerrainRenderer {
    Program shader;
    Uniform perspective;
    Uniform model;

    Planet& planet;
    ChunkLoader loader;
    Option<ChunkPatch> patch;
    Option<ChunkPatch> pending_patch;
public:
    TerrainRenderer(TerrainGenerator& gen, Planet& planet);
    void update_viewpoint(const Camera& cam);
    void render(const Mat4F& proj, const Camera& cam);
};

#endif

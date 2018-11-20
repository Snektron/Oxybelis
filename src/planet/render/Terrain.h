#ifndef _OXYBELIS_PLANET_RENDER_TERRAIN_H
#define _OXYBELIS_PLANET_RENDER_TERRAIN_H

#include <functional>
#include "math/Vec.h"
#include "graphics/camera/Camera.h"
#include "planet/chunk/ChunkLoader.h"
#include "planet/chunk/ChunkPatch.h"
#include "utility/Option.h"

struct Planet;
struct TerrainGeneratorBase;

using ChunkPatchRef = std::reference_wrapper<ChunkPatch>;

class Terrain {
    const Planet& planet;
    ChunkLoader loader;

    Option<ChunkPatch> patch;
    Option<ChunkPatch> pending_patch;
public:
    Terrain(const Planet& planet, TerrainGeneratorBase& tg);
    void update(const Camera& cam);
    Option<ChunkPatchRef> current_patch();

    friend class TerrainRenderer;
    friend class ShadowRenderer;
};

#endif

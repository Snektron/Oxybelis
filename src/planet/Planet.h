#ifndef _OXYBELIS_PLANET_PLANET_H
#define _OXYBELIS_PLANET_PLANET_H

#include <functional>
#include "math/Vec.h"
#include "graphics/camera/Camera.h"
#include "planet/chunk/ChunkLoader.h"
#include "planet/chunk/ChunkPatch.h"
#include "math/Vec.h"
#include "utility/Option.h"

struct TerrainGeneratorBase;

using ChunkPatchRef = std::reference_wrapper<ChunkPatch>;

class Planet {
    ChunkLoader loader;
    Option<ChunkPatch> patch;
    Option<ChunkPatch> pending_patch;

public:
    Vec3D translation;
    double radius;

    Planet(const Vec3D& translation, double radius, TerrainGeneratorBase& tg);
    void update(const Camera& cam);
    Option<ChunkPatchRef> current_patch();

    friend class TerrainRenderer;
    friend class ShadowRenderer;
};

#endif

#ifndef _OXYBELIS_PLANET_PLANET_H
#define _OXYBELIS_PLANET_PLANET_H

#include <functional>
#include "math/Vec.h"
#include "graphics/camera/Camera.h"
#include "planet/chunk/ChunkLoader.h"
#include "planet/chunk/ChunkPatch.h"
#include "math/Vec.h"
#include "math/Quat.h"
#include "utility/Option.h"

struct TerrainGeneratorBase;

class Planet {
    ChunkLoader loader;
    Option<ChunkPatch> patch;
    Option<ChunkPatch> pending_patch;

public:
    Vec3D translation;
    QuatD rotation;
    double radius;

    Planet(const Vec3D& translation, const QuatD& rotation, double radius, TerrainGeneratorBase& tg);
    void update(const Camera& cam);

    bool has_drawable_terrain() const;

    template <typename F>
    void foreach_chunk(F f) const;

    friend class TerrainRenderer;
    friend class ShadowRenderer;
};

template <typename F>
void Planet::foreach_chunk(F f) const {
    if (!this->patch && !this->pending_patch)
        return;

    auto& patch = this->patch ? this->patch.value() : this->pending_patch.value();

    for (auto& entry : patch.chunks) {
        if (entry->is_ready())
            f(entry->chunk());
    }
}

#endif

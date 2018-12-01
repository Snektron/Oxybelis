#include "planet/terrain/Terrain.h"
#include "planet/Planet.h"

namespace {
    // Height constants taken from VoxelStorm planet renderer.
    size_t lod_from_alt(double alt_sq) {
        if (alt_sq <= std::pow(4'000.0, 2.0))
            return 7;
        else if (alt_sq <= std::pow(10'000.0, 2.0))
            return 6;
        else if (alt_sq <= std::pow(32'000.0, 2.0))
            return 5;
        else if (alt_sq <= std::pow(60'000.0, 2.0))
            return 4;
        else if (alt_sq <= std::pow(250'000.0, 2.0))
            return 3;
        else if (alt_sq <= std::pow(1'000'000.0, 2.0))
            return 2;
        else if (alt_sq <= std::pow(1'600'000.0, 2.0))
            return 1;
        else
            return 0;
    }
}

Terrain::Terrain(const Planet& planet, TerrainGeneratorBase& tg):
    planet(planet),
    loader(tg),
    patch(NONE),
    pending_patch(NONE) {
}
    
void Terrain::update(const Camera& cam) {
    double dst = distance(cam.translation, planet.translation) - this->planet.radius;
    auto lod = lod_from_alt(dst * dst);
    auto loc = ChunkLocation(cam.translation, lod, this->planet.radius);

    if (((!this->patch || this->patch->center.id != loc.id) && !this->pending_patch) ||
        (this->pending_patch && this->pending_patch->center.id != loc.id)) {
        this->pending_patch = ChunkPatch(cam.translation, lod, this->planet.radius, this->loader);
    }

    if (this->pending_patch && this->pending_patch->update()) {
        this->patch = std::move(this->pending_patch);
        this->pending_patch = NONE;
        this->loader.collect_garbage();
    }
}

Option<ChunkPatchRef> Terrain::current_patch() {
    if (this->patch) {
        return std::ref(this->patch.value());
    } else if (this->pending_patch) {
        return std::ref(this->pending_patch.value());
    }

    return NONE;
}
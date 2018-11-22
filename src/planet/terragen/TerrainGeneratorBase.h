#ifndef _OXYBELIS_PLANET_TERRAGEN_TERRAINGENERATORBASE_H
#define _OXYBELIS_PLANET_TERRAGEN_TERRAINGENERATORBASE_H

#include <future>
#include "math/Vec.h"
#include "planet/terragen/TerrainData.h"

struct PointProperties {
    Vec3D normal;
    double land_height;
    double water_height;

    bool submerged() const {
        return this->water_height > this->land_height;
    }
};

struct TriangleProperties {
    PointProperties& a;
    PointProperties& b;
    PointProperties& c;
};

struct TerrainGeneratorBase {
    virtual std::future<TerrainData> generate(const TerrainGenerationParameters& param) = 0;
    virtual ~TerrainGeneratorBase() = default;
};

#endif

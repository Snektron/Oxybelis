#ifndef _OXYBELIS_PLANET_TERRAGEN_TERRAINGENERATORBASE_H
#define _OXYBELIS_PLANET_TERRAGEN_TERRAINGENERATORBASE_H

#include <future>
#include "math/Vec.h"
#include "planet/terragen/TerrainData.h"

struct PointProperties {
    double land_height;
    double water_height;

    bool submerged() const {
        return this->water_height > this->land_height;
    }
};

struct PointData {
    PointProperties prop;
    Vec3D normal;
};

struct TriangleProperties {
    PointData& a;
    PointData& b;
    PointData& c;
};

struct TerrainGeneratorBase {
    virtual std::future<TerrainData> generate(const TerrainGenerationParameters& param) = 0;
    virtual ~TerrainGeneratorBase() = default;
};

#endif

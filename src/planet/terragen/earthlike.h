#ifndef _OXYBELIS_PLANET_TERRAGEN_EARTHLIKE_H
#define _OXYBELIS_PLANET_TERRAGEN_EARTHLIKE_H

#include <cstddef>
#include <noise/module/ridgedmulti.h>
#include "planet/terragen/TerrainGenerator.h"
#include "math/Vec.h"

namespace earthlike {
    struct PointGenerator {
        noise::module::RidgedMulti ridged_multi;

        explicit PointGenerator(size_t seed);

        PointProperties operator()(const Vec3D& pos);
    };

    struct TriangleGenerator {
        Vec3F operator()(const TriangleProperties&, bool submerged);
    };

    using Generator = TerrainGenerator<PointGenerator, TriangleGenerator>;
}

#endif

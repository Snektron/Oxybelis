#ifndef _OXYBELIS_PLANET_TERRAGEN_EARTHLIKE_H
#define _OXYBELIS_PLANET_TERRAGEN_EARTHLIKE_H

#include <cstddef>
#include <noisepp/Noise.h>
#include "planet/terragen/TerrainGenerator.h"
#include "math/Vec.h"

namespace earthlike {
    struct PointGenerator {
        size_t seed;
        noisepp::Pipeline3D pipeline;
        noisepp::Cache* cache;
        noisepp::PipelineElement3D* noise;

        explicit PointGenerator(size_t seed);

        PointGenerator(PointGenerator&& other):
            PointGenerator(other.seed) {
        }

        PointGenerator(const PointGenerator& other):
            PointGenerator(other.seed) {
        }

        PointGenerator& operator=(PointGenerator&& other) = delete;
        PointGenerator& operator=(const PointGenerator& other) = delete;

        PointProperties operator()(const Vec3D& pos);
    };

    struct TriangleGenerator {
        Vec3F operator()(const TriangleProperties&, bool submerged);
    };

    using Generator = TerrainGenerator<PointGenerator, TriangleGenerator>;
}

#endif

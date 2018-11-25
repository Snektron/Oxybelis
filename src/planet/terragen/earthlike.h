#ifndef _OXYBELIS_PLANET_TERRAGEN_EARTHLIKE_H
#define _OXYBELIS_PLANET_TERRAGEN_EARTHLIKE_H

#include <vector>
#include <memory>
#include <cstddef>
#include <noise/noise.h>
#include "planet/terragen/TerrainGenerator.h"
#include "math/Vec.h"

namespace earthlike {
    struct PointGenerator {
        std::vector<std::unique_ptr<noise::module::Module>> modules;
        noise::module::Module* terrain;
        noise::module::Module* temperature;
        noise::module::Module* waves;

        explicit PointGenerator(size_t seed);

        PointProperties operator()(const Vec3D& pos);

        template <typename T>
        T& add_module() {
            auto mod = std::make_unique<T>();
            auto* ptr = mod.get();
            this->modules.push_back(std::move(mod));
            return *ptr;
        }
    };

    struct TriangleGenerator {
        Vec3F operator()(const TriangleProperties&, bool submerged);
    };

    using Generator = TerrainGenerator<PointGenerator, TriangleGenerator>;
}

#endif

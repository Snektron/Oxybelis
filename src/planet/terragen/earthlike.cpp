#include "planet/terragen/earthlike.h"

namespace earthlike {
    const Vec3F GROUND_ALBEDO = Vec3F(0.15, 0.35, 0.08) * 0.2;
    const Vec3F WATER_ALBEDO = Vec3F(0.06, 0.08, 0.35) * 0.2;

    PointGenerator::PointGenerator(size_t seed):
        seed(seed), cache(this->pipeline.createCache()) {
        auto ridged_multi = noisepp::RidgedMultiModule();
        ridged_multi.setSeed(seed);
        ridged_multi.setOctaveCount(8);
        ridged_multi.setFrequency(400);

        this->noise = this->pipeline.getElement(ridged_multi.addToPipe(this->pipeline));
    }

    PointProperties PointGenerator::operator()(const Vec3D& pos) {
        this->pipeline.cleanCache(this->cache);
        return {
            pos,
            this->noise->getValue(pos.x, pos.y, pos.z, this->cache) * 2'000,
            0
        };
    }

    Vec3F TriangleGenerator::operator()(const TriangleProperties&, bool submerged) {
        return submerged ? WATER_ALBEDO : GROUND_ALBEDO;
    }
}

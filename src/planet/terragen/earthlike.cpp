#include "planet/terragen/earthlike.h"

namespace earthlike {
    const Vec3F GROUND_ALBEDO = Vec3F(0.15, 0.35, 0.08) * 0.2;
    const Vec3F WATER_ALBEDO = Vec3F(0.06, 0.08, 0.35) * 0.2;

    PointGenerator::PointGenerator(size_t seed) {
        this->ridged_multi.SetSeed(seed);
        this->ridged_multi.SetOctaveCount(8);
        this->ridged_multi.SetFrequency(400);
        this->ridged_multi.SetLacunarity(2.0);
    }

    PointProperties PointGenerator::operator()(const Vec3D& pos) {
        return {
            pos,
            this->ridged_multi.GetValue(pos.x, pos.y, pos.z) * 2'000 / 1.5,
            0
        };
    }

    Vec3F TriangleGenerator::operator()(const TriangleProperties&, bool submerged) {
        return submerged ? WATER_ALBEDO : GROUND_ALBEDO;
    }
}

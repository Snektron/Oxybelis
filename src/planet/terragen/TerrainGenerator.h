#ifndef _OXYBELIS_PLANET_TERRAGEN_TERRAINGENERATOR_H
#define _OXYBELIS_PLANET_TERRAGEN_TERRAINGENERATOR_H

#include <future>
#include <functional>
#include <thread>
#include "planet/chunk/ChunkId.h"
#include "planet/terragen/TerrainData.h"
#include "utility/ThreadPool.h"

struct PointProperties {
    double land_height;
    double water_height;

    bool submerged() const;
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

using PointCallback = std::function<PointProperties(const Vec3D& pos)>;
using TriangleCallback = std::function<Vec3F(const TriangleProperties& prop, bool submerged)>;

class TerrainGenerator {
    ThreadPool& pool;
    PointCallback point_callback;
    TriangleCallback triangle_callback;

public:
    TerrainGenerator(ThreadPool& pool);
    std::future<TerrainData> generate(const TerrainGenerationParameters& param);

private:
    TerrainData generate_chunk_data(const TerrainGenerationParameters& param);
};

#endif

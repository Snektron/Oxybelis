#ifndef _OXYBELIS_PLANET_TERRAGEN_TERRAINDATA_H
#define _OXYBELIS_PLANET_TERRAGEN_TERRAINDATA_H

#include <vector>
#include <cstddef>
#include <utility>
#include "math/Vec.h"
#include "planet/chunk/ChunkId.h"
#include "utility/utility.h"

struct TerrainGenerationParameters {
    ChunkLocation loc;
    double radius;
    size_t side_points;
    size_t inner_points;
};

namespace std {
    template <>
    struct hash<TerrainGenerationParameters> {
        size_t operator()(const TerrainGenerationParameters& param) const {
            size_t hash = std::hash<ChunkId>{}(param.loc.id);
            hash = hash_combine(hash, param.radius);
            hash = hash_combine(hash, param.side_points);
            hash = hash_combine(hash, param.inner_points);
            return hash;
        }
    };
}

inline bool operator==(const TerrainGenerationParameters& a, const TerrainGenerationParameters& b) {
    return std::make_tuple(a.loc.id, a.radius, a.side_points, a.inner_points)
        == std::make_tuple(b.loc.id, b.radius, b.side_points, b.inner_points);
}

struct TerrainData {
    struct VertexData {
        Vec3F vertex;
        Vec3F normal;

        VertexData(const Vec3F& vertex, const Vec3F& normal):
            vertex(vertex), normal(normal) {}
    };

    ChunkLocation loc;
    std::vector<VertexData> terrain_data;

    TerrainData(const TerrainGenerationParameters& param);
};

#endif

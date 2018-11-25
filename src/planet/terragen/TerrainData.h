#ifndef _OXYBELIS_PLANET_TERRAGEN_TERRAINDATA_H
#define _OXYBELIS_PLANET_TERRAGEN_TERRAINDATA_H

#include <vector>
#include <functional>
#include <utility>
#include <cstddef>
#include "math/Vec.h"
#include "planet/chunk/ChunkId.h"

enum class Lod: size_t {
    Low,
    High
};

struct TerrainGenerationParameters {
    ChunkLocation loc;
    double radius;
    Lod lod;
};

namespace std {
    template <>
    struct hash<TerrainGenerationParameters> {
        size_t operator()(const TerrainGenerationParameters& param) const {
            size_t hash = std::hash<ChunkId>{}(param.loc.id);
            hash = hash_combine(hash, param.radius);
            hash = hash_combine(hash, static_cast<size_t>(param.lod));
            return hash;
        }
    };
}

inline bool operator==(const TerrainGenerationParameters& a, const TerrainGenerationParameters& b) {
    return std::make_tuple(a.loc.id, a.radius, a.lod)
        == std::make_tuple(b.loc.id, b.radius, b.lod);
}

struct TerrainData {
    struct VertexData {
        Vec4F vertex;
        Vec4F normal;
        Vec4F color;
        Vec4F nn;

        VertexData(const Vec3F& vertex, const Vec3F& normal, const Vec3F& color, const Vec3F& nn = Vec3F(0)):
            vertex(vertex, 1), normal(normal, 1), color(color, 1), nn(nn, 1) {
        }
    };

    ChunkLocation loc;
    Lod lod;
    std::vector<VertexData> terrain_data;

    TerrainData(const ChunkLocation& loc, Lod lod, std::vector<VertexData>&& terrain_data):
        loc(loc), lod(lod), terrain_data(std::move(terrain_data)) {
    }
};

#endif

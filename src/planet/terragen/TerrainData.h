#ifndef _OXYBELIS_PLANET_TERRAGEN_TERRAINDATA_H
#define _OXYBELIS_PLANET_TERRAGEN_TERRAINDATA_H

#include <vector>
#include "math/Vec.h"
#include "planet/chunk/ChunkId.h"

struct TerrainData {
    struct VertexData {
        Vec3F vertex;
        Vec3F normal;

        VertexData(const Vec3F& vertex, const Vec3F& normal):
            vertex(vertex), normal(normal) {}
    };

    ChunkLocation loc;
    std::vector<VertexData> terrain_data;

    TerrainData(const ChunkLocation& loc, double radius);
};

#endif

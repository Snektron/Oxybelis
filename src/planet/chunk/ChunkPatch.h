#ifndef _OXYBELIS_PLANET_CHUNK_CHUNKPATCH_H
#define _OXYBELIS_PLANET_CHUNK_CHUNKPATCH_H

#include <vector>
#include "math/Vec.h"
#include "math/Mat.h"
#include "graphics/camera/Camera.h"
#include "graphics/shader/Program.h"
#include "planet/chunk/ChunkId.h"
#include "planet/chunk/Chunk.h"
#include "planet/chunk/ChunkLoader.h"

class ChunkPatch {
    ChunkLocation center;
    std::vector<SharedCachedChunk> chunks;
    bool ready;

public:
    ChunkPatch(const Vec3D& p, unsigned depth, double radius, ChunkLoader& cache);
    bool is_ready();
    void render(const Camera& cam, Uniform model, Uniform camera_origin);

    friend class TerrainRenderer;
    friend class Oxybelis;
};

#endif

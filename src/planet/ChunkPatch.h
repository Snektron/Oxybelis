#ifndef _OXYBELIS_PLANET_CHUNKPATCH_H
#define _OXYBELIS_PLANET_CHUNKPATCH_H

#include <vector>
#include "math/Vec.h"
#include "math/Mat.h"
#include "graphics/camera/Camera.h"
#include "graphics/shader/Program.h"
#include "planet/ChunkId.h"
#include "planet/Chunk.h"

class ChunkPatch {
    ChunkLocation center;
    std::vector<Chunk> chunks;

public:
    ChunkPatch(const Vec3D& p, unsigned depth, double radius);
    void render(const Camera& cam, Uniform model);

    friend class PlanetRenderer;
};

#endif

#ifndef _OXYBELIS_PLANET_CHUNK_CHUNK_H
#define _OXYBELIS_PLANET_CHUNK_CHUNK_H

#include <cstddef>
#include "graphics/Buffer.h"
#include "math/Mat.h"
#include "graphics/camera/Camera.h"
#include "graphics/shader/Program.h"
#include "planet/chunk/ChunkId.h"
#include "planet/terragen/TerrainData.h"

class Chunk {
    ChunkLocation loc;
    Buffer terrain;
    size_t vertices;
    Vec3D center;

public:
    Chunk(const TerrainData& terrain);

    void render(const Camera& cam, Uniform model) const;

    ChunkId id() const {
        return this->loc.id;
    }
};

#endif

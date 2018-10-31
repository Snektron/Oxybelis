#ifndef _OXYBELIS_PLANET_CHUNK_H
#define _OXYBELIS_PLANET_CHUNK_H

#include <cstddef>
#include "graphics/VertexArray.h"
#include "graphics/Buffer.h"
#include "math/Mat.h"
#include "graphics/camera/Camera.h"
#include "graphics/shader/Program.h"
#include "planet/ChunkId.h"

class Chunk {
    ChunkLocation loc;
    VertexArray vao;
    Buffer terrain;
    size_t vertices;
    Vec3D center;

public:
    Chunk(const ChunkLocation& loc, double radius);
    void render(const Camera& cam, Uniform model);

    ChunkId id() const {
        return this->loc.id;
    }
};

#endif

#ifndef _OXYBELIS_PLANET_CHUNK_H
#define _OXYBELIS_PLANET_CHUNK_H

#include <cstddef>
#include "graphics/VertexArray.h"
#include "graphics/Buffer.h"
#include "planet/ChunkId.h"

class Chunk {
    ChunkLocation loc;
    VertexArray vao;
    Buffer terrain;
    size_t vertices;

public:
    Chunk(const ChunkLocation& loc, double radius);

    friend class PlanetRenderer;
    friend class ChunkPatch;
};

#endif

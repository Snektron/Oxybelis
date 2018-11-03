#ifndef _OXYBELIS_PLANET_CHUNK_CHUNKCACHE_H
#define _OXYBELIS_PLANET_CHUNK_CHUNKCACHE_H

#include <vector>
#include <memory>
#include "planet/chunk/ChunkId.h"
#include "planet/chunk/Chunk.h"

struct ChunkLocation;

class ChunkCache {
public:
    using ChunkPtr = std::shared_ptr<Chunk>;

private:
    std::vector<ChunkPtr> chunks;

    auto find(ChunkId id) const;
public:
    bool contains(ChunkId id) const;
    void insert(ChunkPtr& chunk);
    ChunkPtr get(ChunkId id) const;
    void collect_garbage();
};

#endif

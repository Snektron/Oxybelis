#ifndef _OXYBELIS_PLANET_CHUNKCACHE_H
#define _OXYBELIS_PLANET_CHUNKCACHE_H

#include <vector>
#include <cstddef>
#include "planet/Chunk.h"

struct ChunkLocation;

class ChunkCache {
    struct Entry {
        Chunk chunk;
        size_t uses;
    };

    struct Ref {
        Entry& entry;

        Ref(Entry& entry);
        Chunk& chunk();
        ~Ref();
    };

    std::vector<Entry> cached_chunks;
    size_t max_cached_chunks;

public:
    ChunkCache(size_t max_cached_chunks);
    Ref get_or_construct(const ChunkLocation& loc, double radius);
    void gc();
};

ChunkCache::Ref::Ref(Entry& entry):
    entry(entry) {
    ++this->entry.uses;
}

Chunk& ChunkCache::Ref::chunk() {
    return this->entry.chunk;
}

ChunkCache::Ref::~Ref() {
    --this->entry.uses;
}

#endif

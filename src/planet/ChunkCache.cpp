#include "planet/ChunkCache.h"

ChunkCache::ChunkCache(size_t max_cached_chunks):
    max_cached_chunks(max_cached_chunks) {
}

auto ChunkCache::get_or_construct(const ChunkLocation& loc, double radius) -> Ref {
    
}

void ChunkCache::gc() {
    
}
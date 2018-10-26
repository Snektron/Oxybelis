#include "planet/ChunkCache.h"

ChunkCache::ChunkCache(size_t max_cached_chunks):
    max_cached_chunks(max_cached_chunks) {
}

Ref ChunkCache::get_or_construct(const ChunkLocation& loc, double radius) {
    
}

void ChunkCache::gc() {
    
}
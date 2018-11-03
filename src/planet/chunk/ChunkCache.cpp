#include "planet/chunk/ChunkCache.h"
#include <algorithm>

auto ChunkCache::find(ChunkId id) const {
    return std::find_if(this->chunks.begin(), this->chunks.end(), [id](const auto& chunk_ptr) {
        return chunk_ptr->id() == id;
    });
}

bool ChunkCache::contains(ChunkId id) const {
    return this->find(id) != this->chunks.end();
}

void ChunkCache::insert(ChunkPtr& chunk) {
    this->chunks.emplace_back(chunk);
}

ChunkCache::ChunkPtr ChunkCache::get(ChunkId id) const {
    auto it = this->find(id);
    if (it == this->chunks.end())
        return nullptr;

    return *it;
}

void ChunkCache::collect_garbage() {
    auto it = std::remove_if(this->chunks.begin(), this->chunks.end(), [](const auto& chunk_ptr) {
        return chunk_ptr.use_count() <= 1;
    });

    this->chunks.erase(it, this->chunks.end());
}

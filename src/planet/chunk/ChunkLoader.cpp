#include "planet/chunk/ChunkLoader.h"
#include <chrono>
#include <iostream>

constexpr const std::chrono::seconds TERRAIN_FUTURE_TIMEOUT{0};

CachedChunk::Status CachedChunk::update() {
    if (this->data.has_type<TerrainFuture>()) {
        auto& fut = this->data.get_unchecked<TerrainFuture>();
        if (fut.wait_for(TERRAIN_FUTURE_TIMEOUT) == std::future_status::ready) {
            auto terrain = fut.get();
            this->data = Chunk(terrain);
            return Status::Ready;
        } else {
            return Status::Pending;            
        }
    } else {
        return Status::Ready;
    }
}

const Chunk& CachedChunk::chunk() const {
    return this->data.get<Chunk>();
}

bool ChunkLoader::contains(ChunkId id) const {
    return this->cache.find(id) != this->cache.end();
}

std::shared_ptr<CachedChunk> ChunkLoader::get_or_queue(const ChunkLocation& loc, double radius) {
    auto it = this->cache.find(loc.id);
    if (it != this->cache.end())
        return it->second;

    auto fut = this->generator.generate(loc, radius);
    auto ptr = std::make_shared<CachedChunk>(std::move(fut));
    return this->cache.emplace_hint(it, loc.id, std::move(ptr))->second;
}

void ChunkLoader::collect_garbage() {
    auto it = this->cache.begin();
    size_t cleaned = 0;
    while (it != this->cache.end()) {
        if (it->second.use_count() == 1) {
            ++cleaned;
            it = this->cache.erase(it);
        } else
            ++it;
    }

    std::cout << __PRETTY_FUNCTION__ << ": Collected " << cleaned << " chunks, " << this->cache.size() << " remaining" << std::endl;
}

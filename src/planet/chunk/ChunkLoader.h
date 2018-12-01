#ifndef _OXYBELIS_PLANET_CHUNK_CHUNKLOADER_H
#define _OXYBELIS_PLANET_CHUNK_CHUNKLOADER_H

#include <unordered_map>
#include <vector>
#include <memory>
#include "planet/chunk/ChunkId.h"
#include "planet/chunk/Chunk.h"
#include "planet/terragen/TerrainGeneratorBase.h"
#include "planet/terragen/TerrainData.h"
#include "utility/Variant.h"

class CachedChunk {
    using TerrainFuture = std::future<TerrainData>;
    Variant<TerrainFuture, Chunk> data;

public:
    enum class Status {
        Pending, // Terrain data is being calculated    
        Ready // Terrain data is ready and uploaded to OpenGL
    };

    CachedChunk(TerrainFuture&& fut) {
        this->data = std::move(fut);
    }

    Status update();
    bool is_ready() const;
    const Chunk& chunk() const;
};

using SharedCachedChunk = std::shared_ptr<CachedChunk>;

class ChunkLoader {
    TerrainGeneratorBase& generator;
    std::unordered_map<TerrainGenerationParameters, SharedCachedChunk> cache;

public:
    ChunkLoader(TerrainGeneratorBase& generator):
        generator(generator) {
    }

    std::shared_ptr<CachedChunk> get_or_queue(const TerrainGenerationParameters& param);
    void collect_garbage();
    size_t vram_usage() const;
};

#endif

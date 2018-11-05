#ifndef _OXYBELIS_PLANET_CHUNK_CHUNKLOADER_H
#define _OXYBELIS_PLANET_CHUNK_CHUNKLOADER_H

#include <unordered_map>
#include <memory>
#include "planet/chunk/ChunkId.h"
#include "planet/chunk/Chunk.h"
#include "planet/terragen/TerrainGenerator.h"
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

    // Must be called from the OpenGL thread
    Status update();
    const Chunk& chunk() const;
};

using SharedCachedChunk = std::shared_ptr<CachedChunk>;

class ChunkLoader {
    TerrainGenerator& generator;
    std::unordered_map<TerrainGenerationParameters, SharedCachedChunk> cache;

public:
    ChunkLoader(TerrainGenerator& generator):
        generator(generator) {
    }

    std::shared_ptr<CachedChunk> get_or_queue(const TerrainGenerationParameters& param);
    void collect_garbage();
};

#endif

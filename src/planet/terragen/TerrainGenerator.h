#ifndef _OXYBELIS_PLANET_TERRAGEN_TERRAINGENERATOR_H
#define _OXYBELIS_PLANET_TERRAGEN_TERRAINGENERATOR_H

#include <future>
#include "planet/chunk/ChunkId.h"
#include "planet/terragen/TerrainData.h"
#include "utility/ThreadPool.h"

class TerrainGenerator {
    ThreadPool& pool;

public:
    TerrainGenerator(ThreadPool& pool);
    std::future<TerrainData> generate(const ChunkLocation& loc, double radius);
};

#endif

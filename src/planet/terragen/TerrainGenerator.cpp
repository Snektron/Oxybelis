#include "planet/terragen/TerrainGenerator.h"

TerrainGenerator::TerrainGenerator(ThreadPool& pool):
    pool(pool) {
}

std::future<TerrainData> TerrainGenerator::generate(const ChunkLocation& loc, double radius) {
    auto task = std::packaged_task<TerrainData()>([loc, radius](){
        return TerrainData(loc, radius);
    });

    auto fut = task.get_future();
    this->pool.schedule(std::move(task));
    return fut;
}

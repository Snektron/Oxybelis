#include "planet/terragen/TerrainGenerator.h"

TerrainGenerator::TerrainGenerator(ThreadPool& pool):
    pool(pool) {
}

std::future<TerrainData> TerrainGenerator::generate(const TerrainGenerationParameters& param) {
    auto task = std::packaged_task<TerrainData()>([param](){
        return TerrainData(param);
    });

    auto fut = task.get_future();
    this->pool.schedule(std::move(task));
    return fut;
}

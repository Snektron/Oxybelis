#ifndef _OXYBELIS_PLANET_TERRAGEN_TERRAINGENERATOR_H
#define _OXYBELIS_PLANET_TERRAGEN_TERRAINGENERATOR_H

#include <vector>
#include <future>
#include "utility/ThreadPool.h"
#include "math/Vec.h"
#include "fast-poly2tri/fastpoly2tri.h"

struct TerrainVertexData {
    Vec3F vertex;
    Vec3F normal;
};

using Terrain = std::vector<TerrainVertexData>;

class TerrainGenerator {
    ThreadPool& thread_pool;

public:
    TerrainGenerator(ThreadPool& thread_pool):
        thread_pool(thread_pool) {
    }

    template <typename F>
    std::future<Terrain> queue_chunk(size_t points, const Vec3F& a, const Vec3F& b, const Vec3F& c, F f);

    template <typename F>
    static Terrain worker(size_t points, const Vec3F& a, const Vec3F& b, const Vec3F& c, F f);
};

template <typename F>
std::future<Terrain> TerrainGenerator::queue_chunk(size_t points, const Vec3F& a, const Vec3F& b, const Vec3F& c, F f) {
    std::packaged_task<Terrain()> task([=](){
        return TerrainGenerator::worker<F>(points, a, b, c, f);
    });

    auto fut = task.get_future();
    this->thread_pool.schedule(std::move(task));
    return fut;
}

template <typename F>
Terrain TerrainGenerator::worker(size_t, const Vec3F&, const Vec3F&, const Vec3F&, F) {
    return Terrain();
}

#endif

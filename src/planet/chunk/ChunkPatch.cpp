#include "planet/chunk/ChunkPatch.h"
#include "planet/terragen/TerrainData.h"
#include <iostream>

const size_t PATH_LEFT = 0b01;
const size_t PATH_RIGHT = 0b10;
const size_t PATH_END = 0b00;

enum class Lod {
    High, Low
};

struct PathElement {
    size_t cmd;
    Lod lod;
};

const PathElement CHUNK_PATCH_PATH[] = {
    {PATH_LEFT | PATH_RIGHT, Lod::High},
    {PATH_RIGHT, Lod::High},
    {PATH_LEFT | PATH_RIGHT, Lod::Low},
    {PATH_END, Lod::Low},
    {PATH_END, Lod::Low},
    {PATH_LEFT | PATH_RIGHT, Lod::High},
    {PATH_RIGHT, Lod::Low},
    {PATH_END, Lod::Low},
    {PATH_LEFT, Lod::High},
    {PATH_LEFT | PATH_RIGHT, Lod::Low},
    {PATH_END, Lod::Low},
    {PATH_END, Lod::Low}
};

struct ChunkInfo {
    ChunkLocation loc;
    Lod lod;
};

ChunkLocation neighbor(size_t depth, double radius, const ChunkLocation& loc, const Vec3D& c, size_t side) {
    auto mid = mix(loc.corners.points[side], loc.corners.points[(side + 1) % 3], 0.5f);
    auto v = mix(c, mid, 2.f);
    auto n = ChunkLocation(v, depth, radius);
    n.corners.orient(c);
    return n;
}

template <typename It>
void generate_patch_part(std::vector<ChunkInfo>& chunks, size_t depth, double radius, const ChunkLocation& loc, It& begin) {
    size_t path_cmd = begin->cmd;
    chunks.push_back(ChunkInfo{loc, begin->lod});
    ++begin;

    auto center = loc.corners.center();

    if (path_cmd & PATH_LEFT) {
        auto left = neighbor(depth, radius, loc, center, 2);
        generate_patch_part(chunks, depth, radius, left, begin);
    }

    if (path_cmd & PATH_RIGHT) {
        auto right = neighbor(depth, radius, loc, center, 1);
        generate_patch_part(chunks, depth, radius, right, begin);
    }
}

ChunkPatch::ChunkPatch(const Vec3D& p, unsigned depth, double radius, ChunkLoader& loader):
    center(p, depth, radius) {

    auto chunk_locs = std::vector<ChunkInfo>();
    chunk_locs.push_back(ChunkInfo{this->center, Lod::High});

    auto center = this->center.corners.center();

    for (size_t i = 0; i < 3; ++i) {
        auto cl = neighbor(depth, radius, this->center, center, i);
        auto begin = std::begin(CHUNK_PATCH_PATH);
        generate_patch_part(chunk_locs, depth, radius, cl, begin);
    }

    auto vec_cmp = [](const Vec3D& a, const Vec3D& b){
        return std::make_tuple(a.x, a.y, a.z) < std::make_tuple(b.x, b.y, b.z);
    };

    std::for_each(chunk_locs.begin(), chunk_locs.end(), [&](auto& info) {
        while (vec_cmp(info.loc.corners.points[1], info.loc.corners.points[0])
            || vec_cmp(info.loc.corners.points[2], info.loc.corners.points[0])) {
            info.loc.corners.rotate_cw();
        }
    });

    std::sort(chunk_locs.begin(), chunk_locs.end(), [&](const auto& a, const auto& b) {
        return a.loc.id.raw() < b.loc.id.raw();
    });

    chunk_locs.erase(std::unique(chunk_locs.begin(), chunk_locs.end(), [](const auto& a, const auto& b) {
        return a.loc.id == b.loc.id;
    }), chunk_locs.end());

    auto chunks = std::vector<Chunk>();

    TerrainGenerationParameters param_template{
        this->center, // Placeholder
        radius,
        200,
        0 // Placeholder
    };

    std::transform(chunk_locs.begin(), chunk_locs.end(), std::back_inserter(this->chunks), [&](const auto& info) {
        param_template.loc = info.loc;
        switch (info.lod) {
            case Lod::Low:
                param_template.inner_points = 1'000;
                break;
            case Lod::High:
                param_template.inner_points = 50'000;
                break;
        }

        return loader.get_or_queue(param_template);
    });
}

bool ChunkPatch::is_ready() {
    for (auto& chunk_ref : this->chunks) {
        if (chunk_ref->update() != CachedChunk::Status::Ready)
            return false;
    }
    std::cout << "==Ready==" << std::endl;
    return true;
}

void ChunkPatch::render(const Camera& cam, Uniform model, Uniform camera_origin) {
    for (auto& chunk_ref : this->chunks) {
        if (chunk_ref->update() == CachedChunk::Status::Ready) {
            chunk_ref->chunk().render(cam, model, camera_origin);
        }
    }    
}

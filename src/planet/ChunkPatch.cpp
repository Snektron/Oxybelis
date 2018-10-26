#include "planet/ChunkPatch.h"

const size_t PATH_LEFT = 0b01;
const size_t PATH_RIGHT = 0b10;
const size_t PATH_END = 0b00;

const size_t CHUNK_PATCH_PATH[] = {
    PATH_LEFT | PATH_RIGHT, PATH_RIGHT, PATH_LEFT | PATH_RIGHT,
    PATH_END, PATH_END, PATH_LEFT | PATH_RIGHT, PATH_RIGHT,
    PATH_END, PATH_LEFT, PATH_LEFT | PATH_RIGHT
};

ChunkLocation neighbor(size_t depth, double radius, const ChunkLocation& loc, const Vec3D& c, size_t side) {
    auto mid = mix(loc.corners.points[side], loc.corners.points[(side + 1) % 3], 0.5f);
    auto v = mix(c, mid, 2.f);
    auto n = ChunkLocation(v, depth, radius);
    n.corners.orient(c);
    return n;
}

template <typename It>
void generate_patch_part(std::vector<ChunkLocation>& chunks, size_t depth, double radius, const ChunkLocation& loc, It& begin, const It& end) {
    chunks.push_back(loc);

    if (begin == end)
        return;

    size_t path_cmd = *begin;
    ++begin;

    auto center = loc.corners.center();

    if (path_cmd & PATH_LEFT) {
        auto left = neighbor(depth, radius, loc, center, 2);
        generate_patch_part(chunks, depth, radius, left, begin, end);
    }

    if (path_cmd & PATH_RIGHT) {
        auto right = neighbor(depth, radius, loc, center, 1);
        generate_patch_part(chunks, depth, radius, right, begin, end);
    }
}

ChunkPatch::ChunkPatch(const Vec3D& p, unsigned depth, double radius):
    center(p, depth, radius) {

    auto chunk_locs = std::vector<ChunkLocation>();
    chunk_locs.push_back(this->center);

    auto center = this->center.corners.center();

    for (size_t i = 0; i < 3; ++i) {
        auto cl = neighbor(depth, radius, this->center, center, i);
        auto begin = std::begin(CHUNK_PATCH_PATH);
        generate_patch_part(chunk_locs, depth, radius, cl, begin, std::end(CHUNK_PATCH_PATH));
    }

    auto vec_cmp = [](const Vec3D& a, const Vec3D& b){
        return std::make_tuple(a.x, a.y, a.z) < std::make_tuple(b.x, b.y, b.z);
    };

    std::for_each(chunk_locs.begin(), chunk_locs.end(), [&](auto& loc) {
        while (vec_cmp(loc.corners.points[1], loc.corners.points[0])
            || vec_cmp(loc.corners.points[2], loc.corners.points[0])) {
            loc.corners.rotate_cw();
        }
    });

    std::sort(chunk_locs.begin(), chunk_locs.end(), [&](const auto& a, const auto& b) {
        return a.id.raw() < b.id.raw();
    });

    chunk_locs.erase(std::unique(chunk_locs.begin(), chunk_locs.end(), [](const auto& a, const auto& b) {
        return a.id == b.id;
    }), chunk_locs.end());

    auto chunks = std::vector<Chunk>();

    std::transform(chunk_locs.begin(), chunk_locs.end(), std::back_inserter(this->chunks), [radius](auto& loc) {
        return Chunk(loc, radius);
    });
}

void ChunkPatch::render() {
    for (auto& chunk : chunks) {
        chunk.vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, chunk.vertices);
    }    
}

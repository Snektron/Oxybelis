#include "planet/render/PlanetRenderer.h"
#include <iostream>
#include <array>
#include <cmath>
#include "glad/glad.h"
#include "graphics/shader/ProgramBuilder.h"
#include "assets.h"

const size_t PATH_LEFT = 0b01;
const size_t PATH_RIGHT = 0b10;
const size_t PATH_END = 0b00;

const size_t CHUNK_PATCH_PATH[] = {
    PATH_LEFT | PATH_RIGHT, PATH_RIGHT, PATH_LEFT | PATH_RIGHT,
    PATH_END, PATH_END, PATH_LEFT | PATH_RIGHT, PATH_RIGHT,
    PATH_END, PATH_LEFT, PATH_LEFT | PATH_RIGHT
};

ChunkLocation neighbor(size_t depth, const ChunkLocation& loc, const Vec3F& c, size_t side) {
    auto mid = mix(loc.corners.points[side], loc.corners.points[(side + 1) % 3], 0.5f);
    auto v = mix(c, mid, 2.f);
    auto n = ChunkLocation(v, depth);
    n.corners.orient(c);
    return n;
}

template <typename It>
void generate_patch_part(std::vector<Chunk>& chunks, size_t depth, const ChunkLocation& loc, It& begin, const It& end) {
    chunks.emplace_back(loc);

    if (begin == end)
        return;

    size_t path_cmd = *begin;
    ++begin;

    auto center = loc.corners.center();

    if (path_cmd & PATH_LEFT) {
        auto left = neighbor(depth, loc, center, 2);
        generate_patch_part(chunks, depth, left, begin, end);
    }

    if (path_cmd & PATH_RIGHT) {
        auto right = neighbor(depth, loc, center, 1);
        generate_patch_part(chunks, depth, right, begin, end);
    }
}

Program another_load_shader() {
    return ProgramBuilder()
        .with(ShaderType::Vertex, assets::passthrough_vs)
        .with(ShaderType::Fragment, assets::passthrough_fs)
        .link();
}

PlanetRenderer::PlanetRenderer():
    shader(another_load_shader()),
    perspective(this->shader.uniform("uPerspective")),
    model(this->shader.uniform("uModel")) {
}

void PlanetRenderer::render(const Mat4F& proj, const FreeCam& cam) {
    this->shader.use();
    glUniformMatrix4fv(this->model, 1, GL_FALSE, cam.to_view_matrix().data());
    glUniformMatrix4fv(this->perspective, 1, GL_FALSE, proj.data());

    size_t depth = 3;

    auto loc = ChunkLocation(cam.translation, depth);

    auto chunks = std::vector<Chunk>();
    chunks.emplace_back(loc);

    auto center = loc.corners.center();

    for (size_t i = 0; i < 3; ++i) {
        auto cl = neighbor(depth, loc, center, i);
        auto begin = std::begin(CHUNK_PATCH_PATH);
        generate_patch_part(chunks, depth, cl, begin, std::end(CHUNK_PATCH_PATH));
    }

    for (auto& chunk : chunks) {
        chunk.vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, chunk.vertices);
    }
}

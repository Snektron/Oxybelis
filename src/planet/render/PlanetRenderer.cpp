#include "planet/render/PlanetRenderer.h"
#include <cmath>
#include <iostream>
#include "glad/glad.h"
#include "graphics/shader/ProgramBuilder.h"
#include "assets.h"

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

    auto center = ChunkId(0);
    // this->generate_patch(center);

    // this->generate_patch(ChunkId(0, 1, 3));

    // this->chunks.emplace_back(center);
    // this->chunks.emplace_back(ChunkId(0, 1, 2));

    // for (unsigned i = 0; i < 20 * 4; ++i) {
    //     this->chunks.emplace_back(ChunkId(i, 0));
    //     this->chunks.emplace_back(ChunkId(i, 1));
    //     this->chunks.emplace_back(ChunkId(i, 2));
    //     this->chunks.emplace_back(ChunkId(i, 3));
    // }
}

void PlanetRenderer::generate_patch(ChunkId center) {
    for (size_t i = 0; i < 20; ++i) {
        this->find_patch_chunks(center, ChunkId(i));
    }
}

void PlanetRenderer::find_patch_chunks(ChunkId center, ChunkId current) {
    if (center.depth() == current.depth()) {
        Vec3F n1 = center.to_triangle().face_normal();
        Vec3F n2 = current.to_triangle().face_normal();

        if (center != current && std::acos(dot(n1, n2)) < 0.8f / (1 << center.depth()) * 3.3f)
            this->chunks.emplace_back(current);
    } else {
        this->find_patch_chunks(center, current.child(0));
        this->find_patch_chunks(center, current.child(1));
        this->find_patch_chunks(center, current.child(2));
        this->find_patch_chunks(center, current.child(3));
    }
}

void PlanetRenderer::render(const Mat4F& proj, const FreeCam& cam) {
    this->shader.use();
    glUniformMatrix4fv(this->model, 1, GL_FALSE, cam.to_view_matrix().data());
    glUniformMatrix4fv(this->perspective, 1, GL_FALSE, proj.data());

    std::vector<Chunk> chunks;

    for (size_t i = 0; i < 20 * 4 * 4; ++i) {
        auto id = ChunkId(i / 16, (i / 4) % 4, i % 4);
        auto tri = id.to_triangle();
        if (tri.sphere_classify(cam.translation) != 0)
            chunks.emplace_back(id);
    }

    for (auto& chunk : chunks) {
        chunk.vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, chunk.vertices);
    }
}

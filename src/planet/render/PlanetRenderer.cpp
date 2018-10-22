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

    auto center = ChunkId(0, 3, 3, 3);
    this->chunks.emplace_back(center);
    auto center1 = ChunkId(0);
    this->chunks.emplace_back(center1);
    // this->chunks.emplace_back();


    // this->chunks.emplace_back(center.neighbor(0));
    // this->chunks.emplace_back(center.neighbor(1));
    // this->chunks.emplace_back(center.neighbor(2));

    // this->generate_patch(center);

    // for (unsigned i = 0; i < 20 * 4; ++i) {
    //     this->chunks.emplace_back(ChunkId(i, 0));
    //     this->chunks.emplace_back(ChunkId(i, 1));
    //     this->chunks.emplace_back(ChunkId(i, 2));
    //     this->chunks.emplace_back(ChunkId(i, 3));
    // }
}

void PlanetRenderer::generate_patch(ChunkId center) {
    for (size_t i = 0; i < 20; ++i) {
        this->rec(center, ChunkId(i));
    }
}

void PlanetRenderer::rec(ChunkId center, ChunkId current) {
    if (center.depth() == current.depth()) {
        Vec3F n1 = center.to_triangle().face_normal();
        Vec3F n2 = current.to_triangle().face_normal();

        // if (std::acos(dot(n1, n2)) < (140.f / 4.f * 3.f) * 3.14159265f / 180.f)
        this->chunks.emplace_back(current);
    } else {
        // this->chunks.emplace_back(current);            
        this->rec(center, current.child(0));
        this->rec(center, current.child(1));
        this->rec(center, current.child(2));
        this->rec(center, current.child(3));
    }
}

void PlanetRenderer::render(const Mat4F& proj, const FreeCam& cam) {
    this->shader.use();
    glUniformMatrix4fv(this->model, 1, GL_FALSE, cam.to_view_matrix().data());
    glUniformMatrix4fv(this->perspective, 1, GL_FALSE, proj.data());

    for (auto& chunk : this->chunks) {
        chunk.vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, chunk.vertices);
    }
}

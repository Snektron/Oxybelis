#include "planet/render/PlanetRenderer.h"
#include "glad/glad.h"
#include "graphics/shader/ProgramBuilder.h"
#include "assets.h"
#include <iostream>

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

    auto center = ChunkId(0, 0, 1, 2);

    std::cout << center << std::endl;
    std::cout << center.child(3).child(1).child(0) << std::endl;

    this->chunks.emplace_back(center);

    // for (unsigned i = 0; i < 20 * 4; ++i) {
    //     this->chunks.emplace_back(ChunkId(i, 0));
    //     this->chunks.emplace_back(ChunkId(i, 1));
    //     this->chunks.emplace_back(ChunkId(i, 2));
    //     this->chunks.emplace_back(ChunkId(i, 3));
    // }
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

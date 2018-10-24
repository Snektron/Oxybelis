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
}

void PlanetRenderer::render(const Mat4F& proj, const FreeCam& cam) {
    this->shader.use();
    glUniformMatrix4fv(this->model, 1, GL_FALSE, cam.to_view_matrix().data());
    glUniformMatrix4fv(this->perspective, 1, GL_FALSE, proj.data());

    size_t depth = 3;

    auto id = ChunkId(cam.translation, depth);

    auto chunks = std::vector<Chunk>();
    chunks.emplace_back(id);

    auto tri = id.to_triangle();
    auto center = tri.center();

    auto neighbor = [depth](const TriangleF& t, const Vec3F& c, size_t side){
        auto mid = mix(t.points[side], t.points[(side + 1) % 3], 0.5f);
        auto v = mix(c, mid, 2.f);
        return ChunkId(v, depth);
    };

    auto add_arm = [&](size_t side){
        auto id = neighbor(tri, center, side);
        chunks.emplace_back(id);

        auto tri = id.to_triangle();
        tri.orient(center);
        auto c = tri.center();

        chunks.emplace_back(neighbor(tri, c, 1));

        auto id_1 = neighbor(tri, c, 2);
        chunks.emplace_back(id_1);

        auto tri_1 = id_1.to_triangle();
        tri_1.orient(c);
        auto c1 = tri_1.center();

        chunks.emplace_back(neighbor(tri_1, c1, 2));
    };

    add_arm(0);
    add_arm(1);
    add_arm(2);

    for (auto& chunk : chunks) {
        chunk.vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, chunk.vertices);
    }
}

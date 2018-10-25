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

    auto loc = ChunkLocation(cam.translation, depth);

    auto chunks = std::vector<Chunk>();
    chunks.emplace_back(loc);

    auto center = loc.corners.center();

    auto neighbor = [depth](const TriangleF& t, const Vec3F& c, size_t side){
        auto mid = mix(t.points[side], t.points[(side + 1) % 3], 0.5f);
        auto v = mix(c, mid, 2.f);
        return ChunkLocation(v, depth);
    };

    auto add_arm = [&](size_t side){
        auto x = neighbor(loc.corners, center, side);
        chunks.emplace_back(x);

        x.corners.orient(center);
        auto cx = x.corners.center();
        chunks.emplace_back(neighbor(x.corners, cx, 1));

        auto y = neighbor(x.corners, cx, 2);
        chunks.emplace_back(y);

        y.corners.orient(cx);
        auto cy = y.corners.center();
        chunks.emplace_back(neighbor(y.corners, cy, 2));
    };

    for (unsigned i = 0; i < 3; ++i)
        add_arm(i);

    for (auto& chunk : chunks) {
        chunk.vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, chunk.vertices);
    }
}

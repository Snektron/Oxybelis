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

    this->generate_patch(ChunkId(0, 0, 0));
    std::cout << size_t(ChunkId(0).depth()) << std::endl;
}

void PlanetRenderer::generate_patch(ChunkId center) {
    for (size_t i = 0; i < 20; ++i) {
        this->find_patch_chunks(center, ChunkId(i));
    }
}

void PlanetRenderer::find_patch_chunks(ChunkId center, ChunkId current) {
    if (center.depth() == current.depth()) {
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

    size_t d = 4;

    auto id = ChunkId::from_position(cam.translation, d);

    auto chunks = std::vector<Chunk>();
    // chunks.emplace_back(id);

    auto tri = id.to_triangle();
    auto center = tri.center();

    auto neigh = [d](const TriangleF& t, const Vec3F& c, size_t side){
        auto mid = (t.points[side] + t.points[(side + 1) % 3]) / 2;
        auto v = 2 * mid - c;
        return ChunkId::from_position(v, d);
    };

    auto add_arm = [&](size_t side){
        auto id = neigh(tri, center, side);
        chunks.emplace_back(id);
        auto tri = id.to_triangle();
        auto center = tri.center();
        
        auto id_a = neigh(tri, center, (side + 1) % 3);
        chunks.emplace_back(id_a);
        auto tri_a = id_a.to_triangle();
        auto center_a = tri_a.center();
        chunks.emplace_back(neigh(tri_a, center_a, (side + 2) % 3));

        auto id_b = neigh(tri, center, (side + 2) % 3);
        chunks.emplace_back(id_b);
        auto tri_b = id_b.to_triangle();
        auto center_b = tri_b.center();
        chunks.emplace_back(neigh(tri_b, center_b, (side + 1) % 3));
    };

    add_arm(0);
    add_arm(1);
    add_arm(2);

    for (auto& chunk : chunks) {
        chunk.vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, chunk.vertices);
    }
}

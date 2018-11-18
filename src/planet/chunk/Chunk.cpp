#include "planet/chunk/Chunk.h"

Chunk::Chunk(const TerrainData& terrain):
    loc(terrain.loc), vertices(terrain.terrain_data.size()), center(loc.corners.center()) {
    this->vao.bind();
    this->terrain.bind(GL_ARRAY_BUFFER);
    Buffer::upload_data(GL_ARRAY_BUFFER, GL_STATIC_DRAW, terrain.terrain_data);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(Vec3F), 0);
    this->vao.enable_attrib(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(Vec3F), reinterpret_cast<void*>(sizeof(Vec3F)));
    this->vao.enable_attrib(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(Vec3F), reinterpret_cast<void*>(2 * sizeof(Vec3F)));
    this->vao.enable_attrib(2);

    this->vram_usage = terrain.terrain_data.size() * sizeof(TerrainData::VertexData);
}

void Chunk::render(const Camera& cam, Uniform model, Uniform camera_origin) const {
    auto view = cam.to_view_matrix();
    view *= Mat4D::translation(this->center);

    glUniformMatrix4fv(model, 1, GL_FALSE, static_cast<Mat4F>(view).data());
    glUniform3fv(camera_origin, 1, static_cast<Vec3F>(cam.translation - this->center).data());

    this->vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, this->vertices);
}
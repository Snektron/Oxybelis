#include "planet/Chunk.h"
#include <array>

Chunk::Chunk(ChunkId id):
    id(id), vertices(3) {
    auto tri = this->id.to_triangle();

    this->vao.bind();
    this->terrain.bind(GL_ARRAY_BUFFER);

    std::array<Vec3F, 6> v;

    if (this->id.depth() == 0 || this->id.quadrant(this->id.depth()) != 5) {
        v = {tri.a, Vec3F(1, 0, 0), tri.b, Vec3F(0, 1, 0), tri.c, Vec3F(0, 0, 1)};
    } else if (this->id.quadrant(this->id.depth()) == 1) {
        v = {tri.a, Vec3F(1, 0, 0), tri.b, Vec3F(0, 0, 0), tri.c, Vec3F(0, 0, 0)};
    } else if (this->id.quadrant(this->id.depth()) == 2) {
        v = {tri.a, Vec3F(0, 0, 0), tri.b, Vec3F(0, 1, 0), tri.c, Vec3F(0, 0, 0)};
    } else {
        v = {tri.a, Vec3F(0, 0, 0), tri.b, Vec3F(0, 0, 0), tri.c, Vec3F(0, 0, 1)};
    }

    Buffer::upload_data(GL_ARRAY_BUFFER, GL_STATIC_DRAW, v);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3F), 0);
    this->vao.enable_attrib(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3F), (void*)(3 * sizeof(GLfloat)));
    this->vao.enable_attrib(1);
}

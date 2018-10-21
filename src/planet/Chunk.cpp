#include "planet/Chunk.h"

Chunk::Chunk(ChunkId id):
    id(id), vertices(3) {
    auto tri = this->id.to_triangle();

    this->vao.bind();
    this->terrain.bind(GL_ARRAY_BUFFER);

    auto n = tri.face_normal();
    Vec3F v[] = {tri.a, n, tri.b, n, tri.c, n};
    Buffer::upload_data(GL_ARRAY_BUFFER, GL_STATIC_DRAW, v);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3F), 0);
    this->vao.enable_attrib(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3F), (void*)(3 * sizeof(GLfloat)));
    this->vao.enable_attrib(1);
}

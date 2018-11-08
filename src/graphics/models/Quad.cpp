#include "graphics/models/Quad.h"

const GLfloat QUAD_DATA[] = {
    -1, -1,
    1, 1,
    -1, 1,

    -1, -1,
    1, -1,
    1, 1
};

Quad::Quad(Attribute vertices) {
    this->vao.bind();
    this->buffer.bind(GL_ARRAY_BUFFER);
    Buffer::upload_data(GL_ARRAY_BUFFER, GL_STATIC_DRAW, QUAD_DATA);
    glVertexAttribPointer(vertices, 2, GL_FLOAT, GL_FALSE, 0, 0);
    this->vao.enable_attrib(vertices);
}

void Quad::render() const {
    this->vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

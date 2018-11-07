#include "planet/render/AtmosphereRenderer.h"
#include <iostream>
#include "graphics/shader/ProgramBuilder.h"
#include "assets.h"

const GLfloat QUAD_DATA[] = {
    -1, -1,
    1, 1,
    -1, 1,

    -1, -1,
    1, -1,
    1, 1
};

static Program load_shader() {
    return ProgramBuilder()
        .with(ShaderType::Vertex, assets::atmosphere_vs)
        .with(ShaderType::Fragment, assets::atmosphere_fs)
        .link();
}

AtmosphereRenderer::AtmosphereRenderer(const Planet& planet):
    planet(planet),
    shader(load_shader()) {
    this->vao.bind();
    this->quad.bind(GL_ARRAY_BUFFER);

    Buffer::upload_data(GL_ARRAY_BUFFER, GL_STATIC_DRAW, QUAD_DATA);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    this->vao.enable_attrib(0);

    this->shader.use();
    glUniform1i(shader.uniform("uTerrain"), 0);
}

void AtmosphereRenderer::render(const Mat4F& proj, const Camera& cam) {
    this->shader.use();
    glUniformMatrix4fv(shader.uniform("uProjection"), 1, GL_FALSE, proj.data());

    this->vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

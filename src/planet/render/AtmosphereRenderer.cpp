#include "planet/render/AtmosphereRenderer.h"
#include <iostream>
#include "graphics/shader/ProgramBuilder.h"
#include "graphics/camera/Projection.h"
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

AtmosphereRenderer::AtmosphereRenderer():
    shader(load_shader()) {
    this->vao.bind();
    this->quad.bind(GL_ARRAY_BUFFER);

    Buffer::upload_data(GL_ARRAY_BUFFER, GL_STATIC_DRAW, QUAD_DATA);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    this->vao.enable_attrib(0);

    this->shader.use();
    glUniform1i(shader.uniform("uTerrain"), 0);
    glUniform1i(shader.uniform("uDistance"), 1);

    this->inv_proj = shader.uniform("uInvProjection");
    this->model = shader.uniform("uModel");
    this->camera_origin = shader.uniform("uCameraOrigin");
    this->camera_up = shader.uniform("uCameraUp");
    this->camera_dir = shader.uniform("uCameraDir");
}

void AtmosphereRenderer::render(const Mat4F& inv_proj, const Camera& cam) {
    auto model = static_cast<Mat4F>(cam.to_view_matrix());

    this->shader.use();
    glUniformMatrix4fv(this->inv_proj, 1, GL_FALSE, inv_proj.data());
    glUniformMatrix4fv(this->model, 1, GL_FALSE, model.data());

    glUniform3fv(this->camera_origin, 1, static_cast<Vec3F>(cam.translation).data());
    glUniform3fv(this->camera_up, 1, static_cast<Vec3F>(cam.rotation.up()).data());
    glUniform3fv(this->camera_dir, 1, static_cast<Vec3F>(cam.rotation.forward()).data());

    this->vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

#include "planet/render/AtmosphereRenderer.h"
#include <iostream>
#include "graphics/shader/ProgramBuilder.h"
#include "graphics/camera/Projection.h"
#include "assets.h"

namespace {
    Program load_shader() {
        return ProgramBuilder()
            .with(ShaderType::Vertex, assets::atmosphere_vs)
            .with(ShaderType::Fragment, assets::atmosphere_fs)
            .link();
    }
}

AtmosphereRenderer::AtmosphereRenderer():
    shader(load_shader()),
    quad(0) {
    this->shader.use();
    glUniform1i(shader.uniform("uTerrain"), 0);
    glUniform1i(shader.uniform("uNormalDistance"), 1);

    this->inv_proj_mat = shader.uniform("uInvProjection");
    this->model_mat = shader.uniform("uModel");
    this->camera_origin = shader.uniform("uCameraOrigin");
    this->camera_up = shader.uniform("uCameraUp");
    this->camera_dir = shader.uniform("uCameraDir");
}

void AtmosphereRenderer::render(const Mat4F& inv_proj, const Camera& cam) {
    auto model = static_cast<Mat4F>(cam.to_view_matrix());

    this->shader.use();
    glUniformMatrix4fv(this->inv_proj_mat, 1, GL_FALSE, inv_proj.data());
    glUniformMatrix4fv(this->model_mat, 1, GL_FALSE, model.data());

    glUniform3fv(this->camera_origin, 1, static_cast<Vec3F>(cam.translation).data());
    glUniform3fv(this->camera_up, 1, static_cast<Vec3F>(cam.rotation.up()).data());
    glUniform3fv(this->camera_dir, 1, static_cast<Vec3F>(cam.rotation.forward()).data());

    this->quad.render();
}

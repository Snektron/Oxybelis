#include "planet/render/AtmosphereRenderer.h"
#include "planet/render/atmosphere_constants.h"
#include "graphics/shader/ProgramBuilder.h"
#include "assets.h"

#include <thread>
#include <chrono>

namespace {
    Program load_shader(GLuint shader) {
        return ProgramBuilder()
            .with(ShaderType::Vertex, assets::atmosphere_vs)
            .with(shader)
            .with(ShaderType::Fragment, assets::atmosphere_fs)
            .link();
    }
}

AtmosphereRenderer::AtmosphereRenderer(GLuint color_tex, GLuint nd_tex, GLuint dndz_tex, GLuint zminmax_tex, double radius, double outer_radius):
    model(atmosphere_constants::load_earthlike_model(radius, outer_radius)),
    shader(load_shader(this->model->GetShader())),
    quad(0) {

    this->shader.use();

    glUniform1i(this->shader.uniform("uTerrain"), color_tex);
    glUniform1i(this->shader.uniform("uNormalDistance"), nd_tex);
    glUniform1i(this->shader.uniform("uDndz"), dndz_tex);
    glUniform1i(this->shader.uniform("uZminmax"), zminmax_tex);

    this->inv_proj_mat = this->shader.uniform("uInvProjection");
    this->model_mat = this->shader.uniform("uModel");
    this->camera_origin = this->shader.uniform("uCameraOrigin");
    this->camera_up = this->shader.uniform("uCameraUp");
    this->camera_dir = this->shader.uniform("uCameraDir");

    this->model->SetProgramUniforms(this->shader, 2, 3, 4, 5);
}

void AtmosphereRenderer::render(const RenderInfo& info) const {
    auto model_mat = static_cast<Mat4F>(info.view);

    this->shader.use();
    glUniformMatrix4fv(this->inv_proj_mat, 1, GL_FALSE, info.inv_proj.data());
    glUniformMatrix4fv(this->model_mat, 1, GL_FALSE, model_mat.data());

    glUniform3fv(this->camera_origin, 1, static_cast<Vec3F>(info.cam.translation).data());
    glUniform3fv(this->camera_up, 1, static_cast<Vec3F>(info.cam.rotation.up()).data());
    glUniform3fv(this->camera_dir, 1, static_cast<Vec3F>(info.cam.rotation.forward()).data());

    this->quad.render();
}

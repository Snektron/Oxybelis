#include "planet/render/PrecomputedAtmosphereRenderer.h"
#include "graphics/shader/ProgramBuilder.h"
#include "planet/render/atmosphere_constants.h"
#include "assets.h"

namespace {
    Program load_shader(GLuint shader) {
        return ProgramBuilder()
            .with(ShaderType::Vertex, assets::atmosphere_vs)
            .with(shader)
            .with(ShaderType::Fragment, assets::precomputed_atmosphere_fs)
            .link();
    }
}

PrecomputedAtmosphereRenderer::PrecomputedAtmosphereRenderer(double radius, double outer_radius):
    model(atmosphere_constants::load_earthlike_model(radius, outer_radius)),
    shader(load_shader(this->model->GetShader())),
    quad(0) {

    this->shader.use();

    glUniform1i(shader.uniform("uTerrain"), 0);
    glUniform1i(shader.uniform("uDistance"), 1);

    this->inv_proj_mat = shader.uniform("uInvProjection");
    this->model_mat = shader.uniform("uModel");
    this->camera_origin = shader.uniform("uCameraOrigin");
    this->camera_up = shader.uniform("uCameraUp");
    this->camera_dir = shader.uniform("uCameraDir");

    this->model->SetProgramUniforms(this->shader, 2, 3, 4, 5);
}

void PrecomputedAtmosphereRenderer::render(const Mat4F& inv_proj, const Camera& cam) const {
    auto model_mat = static_cast<Mat4F>(cam.to_view_matrix());

    this->shader.use();
    glUniformMatrix4fv(this->inv_proj_mat, 1, GL_FALSE, inv_proj.data());
    glUniformMatrix4fv(this->model_mat, 1, GL_FALSE, model_mat.data());

    glUniform3fv(this->camera_origin, 1, static_cast<Vec3F>(cam.translation).data());
    glUniform3fv(this->camera_up, 1, static_cast<Vec3F>(cam.rotation.up()).data());
    glUniform3fv(this->camera_dir, 1, static_cast<Vec3F>(cam.rotation.forward()).data());

    this->quad.render();
}

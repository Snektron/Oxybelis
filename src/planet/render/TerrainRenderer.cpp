#include "planet/render/TerrainRenderer.h"
#include <iostream>
#include <array>
#include <utility>
#include <cmath>
#include <glad/glad.h>
#include "graphics/shader/ProgramBuilder.h"
#include "planet/Planet.h"
#include "planet/render/Terrain.h"
#include "assets.h"

namespace {
    Program load_shader() {
        return ProgramBuilder()
            .with(ShaderType::Vertex, assets::terrain_vs)
            .with(ShaderType::Fragment, assets::terrain_fs)
            .link();
    }
}

TerrainRenderer::TerrainRenderer():
    shader(load_shader()),
    perspective(this->shader.uniform("uPerspective")),
    model(this->shader.uniform("uModel")),
    camera_origin(this->shader.uniform("uCameraOrigin")) {
}

void TerrainRenderer::render(Terrain& terrain, const Mat4F& proj, const Camera& cam) {
    this->shader.use();
    glUniformMatrix4fv(this->perspective, 1, GL_FALSE, proj.data());

    auto patch = terrain.current_patch();
    if (patch) {
        patch->get().render(cam, this->model, this->camera_origin);
    }
}

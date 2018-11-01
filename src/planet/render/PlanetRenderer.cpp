#include "planet/render/PlanetRenderer.h"
#include <iostream>
#include <array>
#include <utility>
#include <cmath>
#include <iomanip>
#include "glad/glad.h"
#include "graphics/shader/ProgramBuilder.h"
#include "planet/ChunkId.h"
#include "planet/Planet.h"
#include "assets.h"

size_t lod_from_alt(double alt_sq) {
    if (alt_sq < 0.05)
        return 4;
    else if (alt_sq < 0.15)
        return 3;
    else if (alt_sq < 0.5)
        return 2;
    else if (alt_sq < 5.0)
        return 1;
    return 0;
}

Program another_load_shader() {
    return ProgramBuilder()
        .with(ShaderType::Vertex, assets::passthrough_vs)
        .with(ShaderType::Fragment, assets::passthrough_fs)
        .link();
}

PlanetRenderer::PlanetRenderer(Planet& planet):
    shader(another_load_shader()),
    perspective(this->shader.uniform("uPerspective")),
    model(this->shader.uniform("uModel")),
    planet(planet),
    patch(NONE) {
}

void PlanetRenderer::render(const Mat4F& proj, const Camera& cam) {
    double dst = distance_sq(cam.translation, Vec3D(0)) - std::pow(this->planet.radius, 2.0);
    dst /= std::pow(this->planet.radius, 2.0);
    auto lod = lod_from_alt(dst);
    auto loc = ChunkLocation(cam.translation, lod, this->planet.radius);

    if (this->patch->center.id != loc.id || !this->patch) {
        this->patch = ChunkPatch(cam.translation, lod, this->planet.radius, this->cache);
        this->cache.collect_garbage();
    }

    this->shader.use();
    glUniformMatrix4fv(this->perspective, 1, GL_FALSE, proj.data());

    this->patch->render(cam, this->model);
}

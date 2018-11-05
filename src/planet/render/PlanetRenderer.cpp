#include "planet/render/PlanetRenderer.h"
#include <iostream>
#include <array>
#include <utility>
#include <cmath>
#include <iomanip>
#include "glad/glad.h"
#include "graphics/shader/ProgramBuilder.h"
#include "planet/chunk/ChunkId.h"
#include "planet/Planet.h"
#include "assets.h"

size_t lod_from_alt(double alt_sq) {
    if (alt_sq < 0.01)
        return 5;
    else if (alt_sq < 0.05)
        return 4;
    else if (alt_sq < 0.15)
        return 3;
    else if (alt_sq < 0.5)
        return 2;
    else if (alt_sq < 5.0)
        return 1;
    return 0;
}

size_t lod_from_alt_2(double alt_sq) {
    if (alt_sq <= std::pow(4'000.0, 2.0))
        return 7;
    else if (alt_sq <= std::pow(10'000.0, 2.0))
        return 6;
    else if (alt_sq <= std::pow(32'000.0, 2.0))
        return 5;
    else if (alt_sq <= std::pow(60'000.0, 2.0))
        return 4;
    else if (alt_sq <= std::pow(250'000.0, 2.0))
        return 3;
    else if (alt_sq <= std::pow(1'000'000.0, 2.0))
        return 2;
    else if (alt_sq <= std::pow(1'600'000.0, 2.0))
        return 1;
    else
        return 0;
}

Program load_shader() {
    return ProgramBuilder()
        .with(ShaderType::Vertex, assets::terrain_vs)
        .with(ShaderType::Fragment, assets::terrain_fs)
        .link();
}

PlanetRenderer::PlanetRenderer(TerrainGenerator& gen, Planet& planet):
    shader(load_shader()),
    perspective(this->shader.uniform("uPerspective")),
    model(this->shader.uniform("uModel")),
    planet(planet),
    loader(gen),
    patch(NONE),
    pending_patch(NONE) {
}

void PlanetRenderer::render(const Mat4F& proj, const Camera& cam) {
    double dst = distance(cam.translation, planet.translation) - this->planet.radius;
    auto lod = lod_from_alt_2(dst * dst);
    // std::cout << (dst / 1'000.0) << " " << lod << std::endl;
    auto loc = ChunkLocation(cam.translation, lod, this->planet.radius);

    if (((!this->patch || this->patch->center.id != loc.id) && !this->pending_patch) ||
        (this->pending_patch && this->pending_patch->center.id != loc.id)) {
        this->pending_patch = ChunkPatch(cam.translation, lod, this->planet.radius, this->loader);
    }

    if (this->pending_patch && this->pending_patch->is_ready()) {
        this->patch = std::move(this->pending_patch);
        this->pending_patch = NONE;
        this->loader.collect_garbage();
    }

    this->shader.use();
    glUniformMatrix4fv(this->perspective, 1, GL_FALSE, proj.data());
    if (this->patch) {
        this->patch->render(cam, this->model);
    } else if (this->pending_patch) {
        this->pending_patch->render(cam, this->model);
    }
}

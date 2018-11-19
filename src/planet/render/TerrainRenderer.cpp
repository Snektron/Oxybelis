#include "planet/render/TerrainRenderer.h"
#include <iostream>
#include <array>
#include <utility>
#include <cmath>
#include <iomanip>
#include <glad/glad.h>
#include "graphics/shader/ProgramBuilder.h"
#include "planet/chunk/ChunkId.h"
#include "planet/Planet.h"
#include "assets.h"

namespace {
    // Height constants taken from VoxelStorm planet renderer.
    size_t lod_from_alt(double alt_sq) {
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
}

TerrainRenderer::TerrainRenderer(TerrainGenerator& gen, Planet& planet):
    shader(load_shader()),
    perspective(this->shader.uniform("uPerspective")),
    model(this->shader.uniform("uModel")),
    camera_origin(this->shader.uniform("uCameraOrigin")),
    planet(planet),
    loader(gen),
    patch(NONE),
    pending_patch(NONE) {
}

void TerrainRenderer::update_viewpoint(const Camera& cam) {
    double dst = distance(cam.translation, planet.translation) - this->planet.radius;
    auto lod = lod_from_alt(dst * dst);
    auto loc = ChunkLocation(cam.translation, lod, this->planet.radius);

    if (((!this->patch || this->patch->center.id != loc.id) && !this->pending_patch) ||
        (this->pending_patch && this->pending_patch->center.id != loc.id)) {
        this->pending_patch = ChunkPatch(cam.translation, lod, this->planet.radius, this->loader);
    }

    if (this->pending_patch && this->pending_patch->is_ready()) {
        std::cout << "VRAM usage: " << double(this->loader.vram_usage()) / (1024 * 1024) << " MiB" << std::endl;

        this->patch = std::move(this->pending_patch);
        this->pending_patch = NONE;
        this->loader.collect_garbage();
    }
}

void TerrainRenderer::render(const Mat4F& proj, const Camera& cam) {
    this->shader.use();
    glUniformMatrix4fv(this->perspective, 1, GL_FALSE, proj.data());

    if (this->patch) {
        this->patch->render(cam, this->model, this->camera_origin);
    } else if (this->pending_patch) {
        this->pending_patch->render(cam, this->model, this->camera_origin);
    }
}

Option<std::reference_wrapper<ChunkPatch>> TerrainRenderer::current_patch() {
    if (this->patch) {
        return std::ref(this->patch.value());
    } else if (this->pending_patch) {
        return std::ref(this->pending_patch.value());
    }
    return NONE;
}
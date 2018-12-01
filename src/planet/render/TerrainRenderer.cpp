#include "planet/render/TerrainRenderer.h"
#include <iostream>
#include <array>
#include <utility>
#include <cmath>
#include <glad/glad.h>
#include "graphics/shader/ProgramBuilder.h"
#include "planet/Planet.h"
#include "planet/render/Terrain.h"
#include "planet/chunk/Chunk.h"
#include "assets.h"

namespace {
    Program load_shader() {
        return ProgramBuilder()
            .with(ShaderType::Vertex, assets::terrain_vs)
            .with(ShaderType::Fragment, assets::terrain_fs)
            .link();
    }
}

TerrainRenderer::FrameBufferState::FrameBufferState(const Vec2UI& dim):
    depth(GL_DEPTH_COMPONENT32, dim.x, dim.y) {
    this->fb.bind();
    glViewport(0, 0, dim.x, dim.y);

    glActiveTexture(GL_TEXTURE0);
    this->color.bind();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dim.x, dim.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->color, 0);

    glActiveTexture(GL_TEXTURE1);
    this->distance.bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, dim.x, dim.y, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, this->distance, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depth);

    GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, buffers);
}

TerrainRenderer::TerrainRenderer(const Vec2UI& dim):
    shader(load_shader()),
    perspective(this->shader.uniform("uPerspective")),
    model(this->shader.uniform("uModel")),
    camera_origin(this->shader.uniform("uCameraOrigin")),
    state(dim) {
}

void TerrainRenderer::resize(const Vec2UI& dim) {
    this->state = FrameBufferState(dim);
}

void TerrainRenderer::render(ChunkPatch& patch, const Mat4F& proj, const Camera& cam) {
    this->prepare(proj);
    // patch.render(cam, this->model, this->camera_origin);

    for (auto& entry : patch.chunks) {
        if (entry->is_ready() && entry->chunk().lod == Lod::High)
            this->dispatch(entry->chunk(), cam);
    }
}

void TerrainRenderer::prepare(const Mat4F& proj) {
    this->state.fb.bind();     
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->shader.use();
    glUniformMatrix4fv(this->perspective, 1, GL_FALSE, proj.data());
}

void TerrainRenderer::dispatch(const Chunk& chunk, const Camera& cam) {
    auto view = cam.to_view_matrix() * Mat4D::translation(chunk.center);

    glUniformMatrix4fv(this->model, 1, GL_FALSE, static_cast<Mat4F>(view).data());
    glUniform3fv(this->camera_origin, 1, static_cast<Vec3F>(cam.translation - chunk.center).data());

    chunk.vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, chunk.vertices);
}
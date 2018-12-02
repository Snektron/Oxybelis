#include "planet/render/ShadowRenderer.h"
#include <iostream>
#include "math/Vec.h"
#include "graphics/shader/ProgramBuilder.h"
#include "graphics/camera/Camera.h"
#include "planet/chunk/Chunk.h"
#include "planet/terragen/TerrainData.h"
#include "planet/Planet.h"
#include "utility/utility.h"
#include "utility/units.h"
#include "assets.h"

namespace {
    constexpr const size_t MAX_SHADOW_BUFFER_QUADS = 2'000'000;
    constexpr const size_t SHADOW_VERTEX_SIZE = sizeof(Vec4F) * 2; // position and normal
    constexpr const size_t SHADOW_BUFFER_SIZE = MAX_SHADOW_BUFFER_QUADS * SHADOW_VERTEX_SIZE * 3 * 2; // 2 triangles for each shadow quad

    // These must be the same as in assets/shadow.glsl
    constexpr const GLint TERRAIN_BINDING = 0;
    constexpr const GLint SHADOW_VOLUMES_BINDING = 1;
    constexpr const GLint COUNTER_BINDING = 2;
    constexpr const size_t LOCAL_GROUP_SIZE = 32;

    Program load_compute_shader() {
        return ProgramBuilder()
            .with(ShaderType::Compute, assets::shadow_glsl)
            .link();
    }

    Program load_draw_shader() {
        return ProgramBuilder()
            .with(ShaderType::Vertex, assets::shadow_vs)
            .with(ShaderType::Fragment, assets::shadow_fs)
            .link();
    }

    struct ShadowVertex {
        Vec4F position;
        Vec4F normal;
    };
}

ShadowRenderer::FrameBufferState::FrameBufferState(const Vec2UI& dim):
    shadow_depth(GL_DEPTH_COMPONENT32, dim.x, dim.y) {

    this->fb.bind();
    glViewport(0, 0, dim.x, dim.y);

    glActiveTexture(GL_TEXTURE6); // 1, 2 taken by terrain, 3 4 5 taken by atmosphere
    this->dndz.bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, dim.x, dim.y, 0, GL_RG, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->dndz, 0);

    glActiveTexture(GL_TEXTURE7);
    this->zminmax.bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, dim.x, dim.y, 0, GL_RG, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, this->zminmax, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->shadow_depth);

    GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, buffers);
}

ShadowRenderer::ShadowRenderer(const Vec2UI& dim, GLuint normal_distance_texture):
    shadow_compute(load_compute_shader()),
    num_vertices(this->shadow_compute.uniform("uNumVertices")),
    center(this->shadow_compute.uniform("uCenter")),
    shadow_draw(load_draw_shader()),
    perspective(this->shadow_draw.uniform("uPerspective")),
    model(this->shadow_draw.uniform("uModel")),
    state(dim) {

    this->shadow_compute.use();

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SHADOW_VOLUMES_BINDING, this->shadow_volumes);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, SHADOW_BUFFER_SIZE, nullptr, 0);

    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, COUNTER_BINDING, this->counter);
    GLuint counter_value[] = {0, 0};
    glBufferStorage(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint) * 2, reinterpret_cast<GLvoid*>(&counter_value), 0);

    glUniform1ui(this->shadow_compute.uniform("uMaxOutputs"), MAX_SHADOW_BUFFER_QUADS);

    this->shadow_draw.use();
    glUniform1i(this->shadow_draw.uniform("uNormalDistance"), normal_distance_texture);

    this->vao.bind();
    this->shadow_volumes.bind(GL_ARRAY_BUFFER);

    auto sz = sizeof(ShadowVertex);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sz, reinterpret_cast<void*>(offsetof(ShadowVertex, position)));
    this->vao.enable_attrib(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sz, reinterpret_cast<void*>(offsetof(ShadowVertex, normal)));
    this->vao.enable_attrib(1);
}

void ShadowRenderer::resize(const Vec2UI& dim) {
    this->state = FrameBufferState(dim);
}

void ShadowRenderer::render(const Planet& planet, const RenderInfo& info) {
    this->prepare();

    planet.foreach_chunk([&, this](const auto& chunk) {
        if (chunk.lod == Lod::High)
        // auto chunk_radius = distance(chunk.loc.corners.a, chunk.loc.corners.b) / 2.0;
        // if (distance(info.cam.translation, chunk.center) < chunk_radius + SHADOW_MIN_DST)
            this->dispatch(chunk, info.cam);
    });

    this->finish(info.proj, info.cam);
}

void ShadowRenderer::prepare() {
    this->shadow_compute.use();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SHADOW_VOLUMES_BINDING, this->shadow_volumes);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, COUNTER_BINDING, this->counter);
    glInvalidateBufferData(this->counter);
    GLuint counter_value[] = {0, 0};
    // For some reason OpenGL requires this data to be a float, even though its binary and will be 
    // reinterpreted as an atomic_uint
    glClearBufferData(GL_ATOMIC_COUNTER_BUFFER, GL_RG32F, GL_RG, GL_FLOAT, reinterpret_cast<void*>(&counter_value));
}

void ShadowRenderer::dispatch(const Chunk& chunk, const Camera& cam) {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, TERRAIN_BINDING, chunk.terrain);
    glUniform1ui(this->num_vertices, chunk.vertices / 3);
    glUniform3fv(this->center, 1, static_cast<Vec3F>(chunk.center - cam.translation).data());
    auto group_size = (next_2pow(chunk.vertices / 3) + LOCAL_GROUP_SIZE - 1) / LOCAL_GROUP_SIZE;
    glDispatchCompute(group_size, 1, 1);
}

void ShadowRenderer::finish(const Mat4F& proj, const Camera& cam) {
    glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);

    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, COUNTER_BINDING, this->counter);
    GLuint counters[2];
    glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint) * 2, &counters);

    this->state.fb.bind();    

    GLfloat dndz_clear[] = {0, 0, 0, 0};
    GLfloat zminmax_clear[] = {std::numeric_limits<GLfloat>::lowest(), 0, 0, 0};

    if (counters[1] > 0) {
        dndz_clear[0] = -GLint(counters[1]);
        zminmax_clear[0] = 0;
    }

    glClearBufferfv(GL_COLOR, 0, dndz_clear);
    glClearBufferfv(GL_COLOR, 1, zminmax_clear);
    glClear(GL_DEPTH_BUFFER_BIT);

    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);

    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquationi(0, GL_FUNC_ADD);
    glBlendEquationi(1, GL_MAX);

    this->vao.bind();
    this->shadow_draw.use();
    glUniformMatrix4fv(this->perspective, 1, GL_FALSE, proj.data());
    glUniformMatrix4fv(this->model, 1, GL_FALSE, static_cast<Mat4F>(cam.rotation.to_view_matrix()).data());
    glDrawArrays(GL_TRIANGLES, 0, counters[0] * 3 * 2);

    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

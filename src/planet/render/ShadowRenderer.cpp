#include "planet/render/ShadowRenderer.h"
#include <iostream>
#include "math/Vec.h"
#include "graphics/shader/ProgramBuilder.h"
#include "graphics/camera/Camera.h"
#include "planet/chunk/Chunk.h"
#include "planet/chunk/ChunkPatch.h"
#include "planet/terragen/TerrainData.h"
#include "utility/utility.h"
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

ShadowRenderer::ShadowRenderer(GLuint normal_distance_texture):
    shadow_compute(load_compute_shader()),
    num_vertices(this->shadow_compute.uniform("uNumVertices")),
    center(this->shadow_compute.uniform("uCenter")),
    shadow_draw(load_draw_shader()),
    perspective(this->shadow_draw.uniform("uPerspective")),
    model(this->shadow_draw.uniform("uModel")) {

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

void ShadowRenderer::begin() {
    this->shadow_compute.use();
    this->reset_counter();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SHADOW_VOLUMES_BINDING, this->shadow_volumes);
}

void ShadowRenderer::dispatch(const Chunk& chunk, const Camera& cam) {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, TERRAIN_BINDING, chunk.terrain);
    glUniform1ui(this->num_vertices, chunk.vertices / 3);
    glUniform3fv(this->center, 1, static_cast<Vec3F>(chunk.center - cam.translation).data());
    auto group_size = (next_2pow(chunk.vertices / 3) + LOCAL_GROUP_SIZE - 1) / LOCAL_GROUP_SIZE;
    glDispatchCompute(group_size, 1, 1);
}

void ShadowRenderer::end(const Mat4F& proj, const Camera& cam) {
    glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);

    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, COUNTER_BINDING, this->counter);
    GLuint counters[2];
    glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint) * 2, &counters);

    if (counters[1] > 0) {
        GLfloat val = -GLint(counters[1]);
        GLfloat dndz_clear[] = {val, 0, 0, 0};
        GLfloat zminmax_clear[] = {0, 0, 0, 0};
        glClearBufferfv(GL_COLOR, 0, dndz_clear);
        glClearBufferfv(GL_COLOR, 1, zminmax_clear);
    }

    this->vao.bind();
    this->shadow_draw.use();
    glUniformMatrix4fv(this->perspective, 1, GL_FALSE, proj.data());
    glUniformMatrix4fv(this->model, 1, GL_FALSE, static_cast<Mat4F>(cam.rotation.to_view_matrix()).data());
    glDrawArrays(GL_TRIANGLES, 0, counters[0] * 3 * 2);
}

void ShadowRenderer::render(ChunkPatch& patch, const Mat4F& proj, const Camera& cam) {
    this->begin();

    for (auto& entry : patch.chunks) {
        if (entry->is_ready() && entry->chunk().lod == Lod::High) {
            this->dispatch(entry->chunk(), cam);
        }
    }

    this->end(proj, cam);
}

void ShadowRenderer::reset_counter() {
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, COUNTER_BINDING, this->counter);
    glInvalidateBufferData(this->counter);
    GLuint counter_value[] = {0, 0};
    // For some reason OpenGL requires this data to be a float, even though its binary and will be 
    // reinterpreted as an atomic_uint
    glClearBufferData(GL_ATOMIC_COUNTER_BUFFER, GL_RG32F, GL_RG, GL_FLOAT, reinterpret_cast<void*>(&counter_value));
}

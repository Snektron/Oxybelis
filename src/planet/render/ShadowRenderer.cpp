#include "planet/render/ShadowRenderer.h"
#include <iostream>
#include "planet/chunk/Chunk.h"
#include "graphics/shader/ProgramBuilder.h"
#include "math/Vec.h"
#include "utility/utility.h"
#include "assets.h"

namespace {
    constexpr const size_t SHADOW_BUFFER_ELEMENTS = 100;
    constexpr const size_t SHADOW_VERTEX_SIZE = sizeof(Vec3F) * 2; // 2 position and normal

    // These must be the same as in assets/shadow.glsl
    constexpr const GLint TERRAIN_BINDING = 0;
    constexpr const GLint SHADOW_VOLUMES_BINDING = 1;
    constexpr const GLint COUNTER_BINDING = 2;
    constexpr const size_t LOCAL_GROUP_SIZE = 16;

    Program load_shader() {
        return ProgramBuilder()
            .with(ShaderType::Compute, assets::shadow_glsl)
            .link();
    }
}

ShadowRenderer::ShadowRenderer():
    shadow_compute(load_shader()),
    num_vertices(this->shadow_compute.uniform("uNumVertices")) {
    this->shadow_compute.use();

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SHADOW_VOLUMES_BINDING, this->shadow_volumes);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, SHADOW_BUFFER_ELEMENTS * SHADOW_VERTEX_SIZE * 3, nullptr, 0);

    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, COUNTER_BINDING, this->counter);
    GLuint counter_value = 0;
    glBufferStorage(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), reinterpret_cast<GLvoid*>(&counter_value), 0);

    glUniform1ui(this->shadow_compute.uniform("uMaxOutputs"), SHADOW_BUFFER_ELEMENTS);
}

void ShadowRenderer::begin() {
    this->shadow_compute.use();
    this->reset_counter();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SHADOW_VOLUMES_BINDING, this->shadow_volumes);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, COUNTER_BINDING, this->counter);
}

void ShadowRenderer::dispatch(const Camera& cam, const Chunk& chunk) {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, TERRAIN_BINDING, chunk.terrain);
    glUniform1ui(this->num_vertices, chunk.vertices);
    auto group_size = (next_2pow(chunk.vertices) + LOCAL_GROUP_SIZE - 1) / LOCAL_GROUP_SIZE;
    glDispatchCompute(group_size, 1, 1);
}

void ShadowRenderer::end(const Mat4F& proj, const Camera& cam) {
    glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);

    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, COUNTER_BINDING, this->counter);
    GLuint counter;
    glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &counter);

    // std::cout << "Atomic counter value: " << counter << std::endl;
}

void ShadowRenderer::reset_counter() {
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, COUNTER_BINDING, this->counter);
    glInvalidateBufferData(this->counter);
    GLuint counter_value = 0;
    glClearBufferData(GL_ATOMIC_COUNTER_BUFFER, GL_R32F, GL_RED, GL_FLOAT, reinterpret_cast<void*>(&counter_value));
}

#include "Tri.h"
#include <random>
#include <iostream>
#include <chrono>
#include <cstdint>
#include <noise/noise.h>
#include "fast-poly2tri/fastpoly2tri.h"
#include "graphics/shader/ProgramBuilder.h"
#include "assets.h"

Program load_shader_() {
    return ProgramBuilder()
        .with(ShaderType::Vertex, assets::passthrough_vs)
        .with(ShaderType::Fragment, assets::passthrough_fs)
        .link();
}

Tri::Tri():
    shader(load_shader_()),
    perspective(this->shader.uniform("uPerspective")),
    model(this->shader.uniform("uModel")) {

    auto begin = std::chrono::high_resolution_clock::now();

    uint32_t max_point_count = 100'000;
    auto mem = MPE_PolyAllocateMem(max_point_count);

    MPEPolyContext poly_ctx;

    MPE_PolyInitContext(&poly_ctx, mem.get(), max_point_count);

    auto rd = std::random_device();
    auto seed = rd();
    auto gen = std::mt19937(seed);
    auto dist = std::uniform_real_distribution<float>(-10.0f, 10.0f);

    auto perlin = noise::module::Perlin();
    perlin.SetOctaveCount(6);
    perlin.SetSeed(seed);

    auto add_pt = [&](float x, float z, bool advance = false) {
        auto* pt = MPE_PolyPushPoint(&poly_ctx);
        pt->X = x;
        pt->Y = z;
        if (advance)
            poly_ctx.Points[poly_ctx.PointCount++] = pt;
    };

    const size_t pts = 200;
    for (size_t i = 0; i < pts - 1; ++i) {
        float x = float(i) / pts * 20.f - 10.f;
        add_pt(x, -10.f);
    }

    for (size_t i = 0; i < pts - 1; ++i) {
        float x = float(i) / pts * 20.f - 10.f;
        add_pt(10.f, x);
    }

    for (size_t i = 0; i < pts - 1; ++i) {
        float x = float(i) / pts * 20.f - 10.f;
        add_pt(-x, 10.f);
    }

    for (size_t i = 0; i < pts - 1; ++i) {
        float x = float(i) / pts * 20.f - 10.f;
        add_pt(-10.f, -x);
    }

    MPE_PolyAddEdge(&poly_ctx);

    for (size_t i = 0; i < max_point_count - (pts * 4 - 4); ++i) {
        add_pt(dist(gen), dist(gen), true);
    }

    MPE_PolyTriangulate(&poly_ctx);

    auto to_vec = [&](const MPEPolyPoint* pt) {
        float y = perlin.GetValue(pt->X / 5.f, 0, pt->Y / 5.f) * 1.0;
        return Vec3F(pt->X, y, pt->Y);
    };

    for (size_t i = 0; i < poly_ctx.TriangleCount; ++i) {
        const auto* t = poly_ctx.Triangles[i];
        const auto* ta = t->Points[0];
        const auto* tb = t->Points[1];
        const auto* tc = t->Points[2];

        Vec3F a = to_vec(ta);
        Vec3F b = to_vec(tb);
        Vec3F c = to_vec(tc);

        auto normal = normalize(cross(c - a, b - a));
        this->vertices.push_back(a);
        this->vertices.push_back(normal);
        this->vertices.push_back(b);
        this->vertices.push_back(normal);
        this->vertices.push_back(c);
        this->vertices.push_back(normal);
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::chrono::milliseconds::period> elapsed = end - begin;
    std::cout << "Generation took " << elapsed.count() << "ms" << std::endl;
    std::cout << "Total triangles: " << this->vertices.size() / 6 << std::endl; 

    this->reupload();
}

void Tri::reupload() {
    this->tri_vao.bind();
    this->tri_buffer.bind(GL_ARRAY_BUFFER);
    Buffer::upload_data(GL_ARRAY_BUFFER, GL_STATIC_DRAW, this->vertices);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3F), 0);
    this->tri_vao.enable_attrib(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3F), reinterpret_cast<void*>(sizeof(Vec3F)));
    this->tri_vao.enable_attrib(1);
}

void Tri::render(const Mat4F& proj, const FreeCam& cam) {
    this->shader.use();
    glUniformMatrix4fv(this->model, 1, GL_FALSE, cam.to_view_matrix().data());
    glUniformMatrix4fv(this->perspective, 1, GL_FALSE, proj.data());

    this->tri_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, this->vertices.size() / 2);
}
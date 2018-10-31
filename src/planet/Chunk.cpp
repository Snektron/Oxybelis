#include "planet/Chunk.h"
#include <array>
#include <vector>
#include <random>
#include <cstddef>
#include <noise/noise.h>
#include "fast-poly2tri/fastpoly2tri.h"

std::vector<Vec3F> generate_data(const ChunkLocation& loc, size_t side_points, size_t points, double radius) {
    auto gen = std::mt19937(loc.id.raw());
    auto dist = std::uniform_real_distribution<double>(0., 1.);

    size_t max_pts = side_points * 3 + points;
    auto mem = MPE_PolyAllocateMem(max_pts);
    MPEPolyContext poly_ctx;

    MPE_PolyInitContext(&poly_ctx, mem.get(), max_pts);

    auto add_edge_pt = [&](double x, double y) {
        auto* pt = MPE_PolyPushPoint(&poly_ctx);
        pt->X = x;
        pt->Y = y;
    };

    auto add_pt = [&](double x, double y) {
        auto* pt = MPE_PolyPushSteinerPoint(&poly_ctx);
        pt->X = x;
        pt->Y = y;
    };

    for (size_t i = 0; i < side_points; ++i) {
        double x = double(i) / side_points;
        add_edge_pt(x, 0);
    }

    for (size_t i = 0; i < side_points; ++i) {
        double x = double(i) / side_points;
        add_edge_pt(1 - x, x);
    }

    for (size_t i = 0; i < side_points; ++i) {
        double x = double(i) / side_points;
        add_edge_pt(0, 1.f - x);
    }

    MPE_PolyAddEdge(&poly_ctx);

    constexpr auto center = (Vec2D(0, 0) + Vec2D(1, 0) + Vec2D(0, 1)) / 3.f;
    double edge_dst = 1.f / side_points;

    for (size_t i = 0; i < points; ++i) {
        auto p = Vec2D(dist(gen), dist(gen));

        if (p.x + p.y > 1)
            p = 1 - p;

        p = mix(p, center, edge_dst);

        add_pt(p.x, p.y);
    }

    MPE_PolyTriangulate(&poly_ctx);

    auto vertices = std::vector<Vec3F>();

    auto ba = loc.corners.b - loc.corners.a;
    auto ca = loc.corners.c - loc.corners.a;

    auto perlin = noise::module::Perlin();
    perlin.SetOctaveCount(6);
    perlin.SetSeed(0);

    auto chunk_center = loc.corners.center();

    auto get_vec = [&](double x, double y) {
        auto v = normalize(x * ba + y * ca + loc.corners.a);
        double h = perlin.GetValue(v.x, v.y, v.z);
        h *= h * h * h;
        h = h * 0.02 + 1.0;
        return v * radius * h;
    };

    for (size_t i = 0; i < poly_ctx.TriangleCount; ++i) {
        const auto* t = poly_ctx.Triangles[i];
        const auto* ta = t->Points[0];
        const auto* tb = t->Points[1];
        const auto* tc = t->Points[2];

        auto a = get_vec(ta->X, ta->Y);
        auto b = get_vec(tb->X, tb->Y);
        auto c = get_vec(tc->X, tc->Y);

        auto normal = normalize(cross(c - a, b - a));
        vertices.push_back(a - chunk_center);
        vertices.push_back(normal);
        vertices.push_back(b - chunk_center);
        vertices.push_back(normal);
        vertices.push_back(c - chunk_center);
        vertices.push_back(normal);
    }

    return vertices;
}

Chunk::Chunk(const ChunkLocation& loc, double radius):
    loc(loc), vertices(3), center(loc.corners.center()) {

    this->vao.bind();
    this->terrain.bind(GL_ARRAY_BUFFER);

    auto v = generate_data(this->loc, 50, 1000, radius);
    Buffer::upload_data(GL_ARRAY_BUFFER, GL_STATIC_DRAW, v);
    this->vertices = v.size() / 2;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3F), 0);
    this->vao.enable_attrib(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3F), (void*)(sizeof(Vec3F)));
    this->vao.enable_attrib(1);
}

void Chunk::render(const Camera& cam, Uniform model) {
    auto view = cam.to_view_matrix();
    view *= Mat4D::translation(this->center);

    glUniformMatrix4fv(model, 1, GL_FALSE, static_cast<Mat4F>(view).data());

    this->vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, this->vertices);
}
#include "planet/Chunk.h"
#include <array>
#include <vector>
#include <random>
#include <cstddef>
#include "fast-poly2tri/fastpoly2tri.h"

std::vector<Vec3F> generate_data(const ChunkLocation& loc, size_t side_points, size_t points, float radius) {
    auto gen = std::mt19937(loc.id.raw());
    auto dist = std::uniform_real_distribution<float>(0.05f, 0.95f);

    size_t max_pts = side_points * 3 + points;
    auto mem = MPE_PolyAllocateMem(max_pts);
    MPEPolyContext poly_ctx;

    MPE_PolyInitContext(&poly_ctx, mem.get(), max_pts);

    auto add_edge_pt = [&](float x, float y) {
        auto* pt = MPE_PolyPushPoint(&poly_ctx);
        pt->X = x;
        pt->Y = y;
    };

    auto add_pt = [&](float x, float y) {
        auto* pt = MPE_PolyPushSteinerPoint(&poly_ctx);
        pt->X = x;
        pt->Y = y;
    };

    for (size_t i = 0; i < side_points; ++i) {
        float x = float(i) / side_points;
        add_edge_pt(x, 0);
    }

    for (size_t i = 0; i < side_points; ++i) {
        float x = float(i) / side_points;
        add_edge_pt(1 - x, 1);
    }

    for (size_t i = 0; i < side_points; ++i) {
        float x = float(i) / side_points;
        add_edge_pt(0, 1.f - x);
    }

    MPE_PolyAddEdge(&poly_ctx);

    for (size_t i = 0; i < points; ++i) {
        add_pt(dist(gen), dist(gen));
    }

    MPE_PolyTriangulate(&poly_ctx);

    auto vertices = std::vector<Vec3F>();

    auto v1 = loc.corners.b - loc.corners.a;
    auto v2 = loc.corners.c - loc.corners.a;

    for (size_t i = 0; i < poly_ctx.TriangleCount; ++i) {
        const auto* t = poly_ctx.Triangles[i];
        const auto* ta = t->Points[0];
        const auto* tb = t->Points[1];
        const auto* tc = t->Points[2];

        auto a = normalize(ta->X * v1 + (1.f - ta->X) * ta->Y * v2 + loc.corners.a) * radius;
        auto b = normalize(tb->X * v1 + (1.f - tb->X) * tb->Y * v2 + loc.corners.a) * radius;
        auto c = normalize(tc->X * v1 + (1.f - tc->X) * tc->Y * v2 + loc.corners.a) * radius;

        auto normal = normalize(cross(c - a, b - a));
        vertices.push_back(a);
        vertices.push_back(normal);
        vertices.push_back(b);
        vertices.push_back(normal);
        vertices.push_back(c);
        vertices.push_back(normal);
    }

    return vertices;
}

Chunk::Chunk(const ChunkLocation& loc, double radius):
    loc(loc), vertices(3) {

    this->vao.bind();
    this->terrain.bind(GL_ARRAY_BUFFER);

    auto v = generate_data(this->loc, 10, 100, radius);
    Buffer::upload_data(GL_ARRAY_BUFFER, GL_STATIC_DRAW, v);
    this->vertices = v.size() / 2;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3F), 0);
    this->vao.enable_attrib(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3F), (void*)(sizeof(Vec3F)));
    this->vao.enable_attrib(1);
}

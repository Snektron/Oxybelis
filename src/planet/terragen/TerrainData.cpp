#include "planet/terragen/TerrainData.h"
#include <random>
#include <noise/noise.h>
#include "fast-poly2tri/fastpoly2tri.h"

TerrainData::TerrainData(const ChunkLocation& loc, double radius):
    loc(loc) {
    size_t side_points = 100;
    size_t points = 10'000;
    size_t max_pts = side_points * 3 + points;

    auto gen = std::mt19937(loc.id.raw());

    auto mem = MPE_PolyAllocateMem(max_pts);
    MPEPolyContext poly_ctx;

    MPE_PolyInitContext(&poly_ctx, mem.get(), max_pts);

    auto a_b = loc.corners.b - loc.corners.a;
    auto c_d = -cross(loc.corners.face_normal(), a_b);

    double ab = length(a_b);
    double ad = dot(a_b, loc.corners.c - loc.corners.a) / ab;
    double db = ab - ad;
    double cd = std::sqrt(distance_sq(loc.corners.a, loc.corners.c) - ad * ad);

    double adn = ad / ab;
    double dbn = db / ab;
    double cdn = cd / ab;

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
        add_edge_pt(1 - x * dbn, x * cdn);
    }

    for (size_t i = 0; i < side_points; ++i) {
        double x = double(i) / side_points;
        add_edge_pt((1 - x) * adn, (1 - x) * cdn);
    }

    MPE_PolyAddEdge(&poly_ctx);

    auto center = Vec2D(1 + adn, cdn) / 3.0;
    double edge_dst = 1.0 / side_points;

    auto dist_x = std::uniform_real_distribution<double>(adn / 2.0, 1 - dbn / 2.0);
    auto dist_y = std::uniform_real_distribution<double>(0, cdn);

    for (size_t i = 0; i < points; ++i) {
        auto p = Vec2D(dist_x(gen), dist_y(gen));

        if (p.y * ad > cd * p.x) {
            p.y = cdn - p.y;
            p.x = adn - p.x;
        } else if (p.y * db > (1 - p.x) * cd){
            p.y = cdn - p.y;
            p.x = 1 - p.x + adn;
        }

        p = mix(p, center, edge_dst);

        add_pt(p.x, p.y);
    }

    MPE_PolyTriangulate(&poly_ctx);

    this->terrain_data.reserve(poly_ctx.TriangleCount * 3);

    auto perlin = noise::module::Perlin();
    perlin.SetOctaveCount(8);
    perlin.SetSeed(0);

    auto chunk_center = loc.corners.center();

    auto get_vec = [&](double x, double y) {
        auto v = normalize(loc.corners.a + a_b * x + c_d * y);
        double h = perlin.GetValue(v.x * 50.0, v.y * 50.0, v.z * 50.0) - 0.5;
        h *= h;
        h += 0.5;
        h *= 5'000.0;
        return v * (radius + h);
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

        this->terrain_data.emplace_back(a - chunk_center, normal);
        this->terrain_data.emplace_back(b - chunk_center, normal);
        this->terrain_data.emplace_back(c - chunk_center, normal);
    }
}

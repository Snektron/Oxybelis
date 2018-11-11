#include "planet/terragen/TerrainData.h"
#include <random>
#include <array>
#include <utility>
#include <iostream>
#include <thread>
#include <noise/noise.h>
#include "fast-poly2tri/fastpoly2tri.h"
#include "utility/Timer.h"

TerrainData::TerrainData(const TerrainGenerationParameters& param):
    loc(param.loc) {
    // auto timer = Timer("Chunk generation took ");
    size_t max_pts = param.side_points * 3 + param.inner_points;

    auto gen = std::mt19937(param.loc.id.raw());

    auto mem = MPE_PolyAllocateMem(max_pts);
    MPEPolyContext poly_ctx;

    MPE_PolyInitContext(&poly_ctx, mem.get(), max_pts);

    const auto& corners = param.loc.corners;

    auto a_b = corners.b - corners.a;
    auto c_d = -cross(corners.face_normal(), a_b);

    double ab = length(a_b);
    double ad = dot(a_b, corners.c - corners.a) / ab;
    double db = ab - ad;
    double cd = std::sqrt(distance_sq(corners.a, corners.c) - ad * ad);

    double adn = ad / ab;
    double dbn = db / ab;
    double cdn = cd / ab;

    const auto sides = std::array<Vec2D, 3>{
        Vec2D(0),
        Vec2D(1, 0),
        Vec2D(adn, cdn)
    };

    for (size_t i = 0; i < sides.size(); ++i) {
        const auto& begin = sides[i];
        const auto& end = sides[(i + 1) % sides.size()];

        for (size_t j = 0; j < param.side_points; ++j) {
            double k = static_cast<double>(j) / param.side_points;
            auto pt = mix(begin, end, k);
            auto* mpe_pt = MPE_PolyPushPoint(&poly_ctx);
            mpe_pt->X = pt.x;
            mpe_pt->Y = pt.y;
        }
    }

    MPE_PolyAddEdge(&poly_ctx);

    auto center = Vec2D(1 + adn, cdn) / 3.0;
    double edge_dst = 1.0 / param.side_points;

    auto dist_x = std::uniform_real_distribution<double>(adn / 2.0, 1 - dbn / 2.0);
    auto dist_y = std::uniform_real_distribution<double>(0, cdn);

    for (size_t i = 0; i < param.inner_points; ++i) {
        auto pt = Vec2D(dist_x(gen), dist_y(gen));

        if (pt.y * ad > cd * pt.x) {
            pt.y = cdn - pt.y;
            pt.x = adn - pt.x;
        } else if (pt.y * db > (1 - pt.x) * cd){
            pt.y = cdn - pt.y;
            pt.x = 1 - pt.x + adn;
        }

        pt.mix(center, edge_dst);

        auto* mpe_pt = MPE_PolyPushSteinerPoint(&poly_ctx);
        mpe_pt->X = pt.x;
        mpe_pt->Y = pt.y;
    }

    MPE_PolyTriangulate(&poly_ctx);

    this->terrain_data.reserve(poly_ctx.TriangleCount * 6);

    auto perlin = noise::module::Perlin();
    perlin.SetOctaveCount(8);
    perlin.SetSeed(0);

    auto chunk_center = corners.center();

    auto get_vec = [&](double x, double y) {
        auto v = normalize(corners.a + a_b * x + c_d * y);
        double h = perlin.GetValue(v.x * 500.0, v.y * 500.0, v.z * 500.0) / 1.5;
        h *= h;
        h *= 2'000.0;
        return v * (param.radius + h);
    };

    auto find_opposite = [](const MPEPolyTriangle* tri, const MPEPolyPoint* a, const MPEPolyPoint* b) -> const MPEPolyPoint* {
        if (tri == nullptr)
            return nullptr;
        else if (tri->Points[0] == a && tri->Points[1] == b)
            return tri->Points[2];
        else if (tri->Points[1] == a && tri->Points[2] == b)
            return tri->Points[0];
        else if (tri->Points[2] == a && tri->Points[0] == b)
            return tri->Points[1];
        else
            std::abort();
    };

    for (size_t i = 0; i < poly_ctx.TriangleCount; ++i) {
        const auto* t = poly_ctx.Triangles[i];
        const auto* ta = t->Points[0];
        const auto* tb = t->Points[1];
        const auto* tc = t->Points[2];

        const auto* td = find_opposite(t->Neighbors[2], tb, ta);
        const auto* te = find_opposite(t->Neighbors[0], tc, tb);
        const auto* tf = find_opposite(t->Neighbors[1], ta, tc);

        auto a = get_vec(ta->X, ta->Y);
        auto b = get_vec(tb->X, tb->Y);
        auto c = get_vec(tc->X, tc->Y);

        Vec3F nd, ne, nf;
        auto normal = normalize(cross(c - a, b - a));

        if (td) {
            auto d = get_vec(td->X, td->Y);
            nd = normalize(cross(a - d, b - d));
        }

        if (te) {
            auto e = get_vec(te->X, te->Y);
            ne = normalize(cross(b - e, c - e));
        }

        if (tf) {
            auto f = get_vec(tf->X, tf->Y);
            nf = normalize(cross(c - f, a - f));
        }

        this->terrain_data.emplace_back(a - chunk_center, normal);
        this->terrain_data.emplace_back(Vec3F(), nd);
        this->terrain_data.emplace_back(b - chunk_center, normal);
        this->terrain_data.emplace_back(Vec3F(), ne);
        this->terrain_data.emplace_back(c - chunk_center, normal);
        this->terrain_data.emplace_back(Vec3F(), nf);
    }
}

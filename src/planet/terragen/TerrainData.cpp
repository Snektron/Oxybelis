#include "planet/terragen/TerrainData.h"
#include <random>
#include <array>
#include <utility>
#include <iostream>
#include <unordered_map>
#include "noisepp/Noise.h"
#include "fast-poly2tri/fastpoly2tri.h"

namespace {
    const Vec3F GROUND_ALBEDO = Vec3F(0.15, 0.35, 0.08) * 0.2;
    const Vec3F WATER_ALBEDO = Vec3F(0.06, 0.08, 0.35) * 0.2;

    constexpr const size_t SIDE_POINTS = 200;
    constexpr const size_t INNER_POINTS[] = {1'000, 20'000};
}

TerrainData::TerrainData(const TerrainGenerationParameters& param):
    loc(param.loc) {

    size_t inner_points = INNER_POINTS[static_cast<size_t>(param.lod)];
    size_t total_points = SIDE_POINTS * 3 + inner_points;

    auto gen = std::mt19937(param.loc.id.raw());

    auto mem = MPE_PolyAllocateMem(total_points);
    MPEPolyContext poly_ctx;

    MPE_PolyInitContext(&poly_ctx, mem.get(), total_points);

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

        for (size_t j = 0; j < SIDE_POINTS; ++j) {
            double k = static_cast<double>(j) / SIDE_POINTS;
            auto pt = mix(begin, end, k);
            auto* mpe_pt = MPE_PolyPushPoint(&poly_ctx);
            mpe_pt->X = pt.x;
            mpe_pt->Y = pt.y;
        }
    }

    MPE_PolyAddEdge(&poly_ctx);

    auto center = Vec2D(1 + adn, cdn) / 3.0;
    double edge_dst = 1.0 / SIDE_POINTS;

    auto dist_x = std::uniform_real_distribution<double>(adn / 2.0, 1 - dbn / 2.0);
    auto dist_y = std::uniform_real_distribution<double>(0, cdn);

    for (size_t i = 0; i < inner_points; ++i) {
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

    this->terrain_data.reserve(poly_ctx.TriangleCount * 3);
    auto map = std::unordered_map<const MPEPolyPoint*, Vec3D>(total_points);

    auto module = noisepp::RidgedMultiModule();
    module.setOctaveCount(8);
    module.setSeed(0);
    module.setFrequency(400);

    auto pipeline = noisepp::Pipeline3D();
    auto* noise = pipeline.getElement(module.addToPipe(pipeline));
    auto* cache = pipeline.createCache();

    auto chunk_center = corners.center();

    auto get_vec = [&](const MPEPolyPoint* pt) {
        auto it = map.find(pt);
        if (it != map.end())
            return it->second;

        double x = pt->X;
        double y = pt->Y;

        auto v = normalize(corners.a + a_b * x + c_d * y);
        double h = noise->getValue(v.x, v.y, v.z, cache) * 2'000.0 / 1.5;
        v *= param.radius + h;
        map.emplace_hint(it, pt, v);
        return v;
    };

    auto emit_tri = [&](const Vec3D& a, const Vec3D& b, const Vec3D& c, const Vec3F& color) {
        auto normal = normalize(cross(c - a, b - a));

        this->terrain_data.emplace_back(a - chunk_center, normal, color, Vec3F(0));
        this->terrain_data.emplace_back(b - chunk_center, normal, color, Vec3F(0));
        this->terrain_data.emplace_back(c - chunk_center, normal, color, Vec3F(0));
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

        auto a = get_vec(ta);
        auto b = get_vec(tb);
        auto c = get_vec(tc);

        Vec3F nd, ne, nf;
        auto normal = normalize(cross(c - a, b - a));

        if (td) {
            auto d = get_vec(td);
            nd = normalize(cross(a - d, b - d));
        }

        if (te) {
            auto e = get_vec(te);
            ne = normalize(cross(b - e, c - e));
        }

        if (tf) {
            auto f = get_vec(tf);
            nf = normalize(cross(c - f, a - f));
        }

        bool sa = length(a) < param.radius;
        bool sb = length(b) < param.radius;
        bool sc = length(c) < param.radius;

        size_t n_submerged = sa + sb + sc;

        if (n_submerged > 0) {
            Vec3F a1 = normalize(a) * param.radius;
            Vec3F b1 = normalize(b) * param.radius;
            Vec3F c1 = normalize(c) * param.radius;
            emit_tri(a1, b1, c1, WATER_ALBEDO);

            if (n_submerged < 3) {
                emit_tri(a, b, c, GROUND_ALBEDO);
            }
        } else {
            emit_tri(a, b, c, GROUND_ALBEDO);
        }
    }

    pipeline.freeCache(cache);
}

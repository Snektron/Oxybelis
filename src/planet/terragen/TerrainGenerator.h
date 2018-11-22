#ifndef _OXYBELIS_PLANET_TERRAGEN_TERRAINGENERATOR_H
#define _OXYBELIS_PLANET_TERRAGEN_TERRAINGENERATOR_H

#include <future>
#include <functional>
#include <thread>
#include <iostream>
#include <random>
#include <utility>
#include <unordered_map>
#include <fast-poly2tri/fastpoly2tri.h>
#include "planet/chunk/ChunkId.h"
#include "planet/terragen/TerrainData.h"
#include "planet/terragen/TerrainGeneratorBase.h"
#include "utility/ThreadPool.h"
#include "math/Vec.h"

template <typename PointGenerator, typename TriangleGenerator>
class TerrainGenerator: public TerrainGeneratorBase {
    ThreadPool& pool;
    PointGenerator point_generator;
    TriangleGenerator triangle_generator;

public:
    TerrainGenerator(ThreadPool& pool, PointGenerator&& pg, TriangleGenerator&& tg);
    std::future<TerrainData> generate(const TerrainGenerationParameters& param);

private:
    TerrainData generate_chunk_data(const TerrainGenerationParameters& param);
};

template <typename PG, typename TG>
TerrainGenerator<PG, TG>::TerrainGenerator(ThreadPool& pool, PG&& pg, TG&& tg):
    pool(pool), point_generator(std::move(pg)), triangle_generator(std::move(tg)) {
}

template <typename PG, typename TG>
std::future<TerrainData> TerrainGenerator<PG, TG>::generate(const TerrainGenerationParameters& param) {
    auto task = std::packaged_task<TerrainData()>(std::bind(&TerrainGenerator::generate_chunk_data, this, param));

    auto fut = task.get_future();
    this->pool.schedule(std::move(task));
    return fut;
}

template <typename PG, typename TG>
TerrainData TerrainGenerator<PG, TG>::generate_chunk_data(const TerrainGenerationParameters& param) {
    constexpr const size_t side_points = 200;
    constexpr const size_t inner_points_by_lod[] = {1'000, 20'000};

    const size_t inner_points = inner_points_by_lod[static_cast<size_t>(param.lod)];
    const size_t total_points = side_points * 3 + inner_points;

    auto mem = MPE_PolyAllocateMem(total_points);
    MPEPolyContext poly_ctx;
    MPE_PolyInitContext(&poly_ctx, mem.get(), total_points);
    
    auto point_data = std::unordered_map<const MPEPolyPoint*, PointProperties>(total_points);

    const auto& corners = param.loc.corners;

    // Calculate local triangle points:
    //        c
    //       /|\
    //      / | \
    //     /  |  \
    //    /   |   \
    // a /____|____\ b
    //        d

    const auto a_b = corners.b - corners.a;
    const auto c_d = -cross(corners.face_normal(), a_b);

    const double ab = length(a_b);
    const double ad = dot(a_b, corners.c - corners.a) / ab;
    const double db = ab - ad;
    const double cd = std::sqrt(distance_sq(corners.a, corners.c) - ad * ad);

    // Normalize coordinates so that ab is 1.
    const double adn = ad / ab;
    const double dbn = db / ab;
    const double cdn = cd / ab;

    auto add_pt = [&, this](const Vec2D& pt, MPEPolyPoint* point) {
        const auto surface_coord = normalize(corners.a + a_b * pt.x + c_d * pt.y);
        point_data[point] = this->point_generator(surface_coord);
        point->X = pt.x;
        point->Y = pt.y;
    };

    // Add chunk edge points
    {
        const auto local_corners = std::array<Vec2D, 3>{
            Vec2D(0),
            Vec2D(1, 0),
            Vec2D(adn, cdn)
        };

        for (size_t i = 0; i < local_corners.size(); ++i) {
            const auto& begin = local_corners[i];
            const auto& end = local_corners[(i + 1) % local_corners.size()];

            for (size_t j = 0; j < side_points; ++j) {
                double k = static_cast<double>(j) / side_points;
                auto pt = mix(begin, end, k);
                add_pt(pt, MPE_PolyPushPoint(&poly_ctx));
            }
        }

        MPE_PolyAddEdge(&poly_ctx);
    }

    // Add chunk inner points
    {
        // Average of all local points
        const auto local_center = Vec2D(1 + adn, cdn) / 3.0;
        const double edge_dst = 1.0 / side_points;

        auto gen = std::mt19937(param.loc.id.raw());
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

            pt.mix(local_center, edge_dst);
            add_pt(pt, MPE_PolyPushSteinerPoint(&poly_ctx));
        }
    }

    MPE_PolyTriangulate(&poly_ctx);

    auto mesh = std::vector<TerrainData::VertexData>();
    const auto center = corners.center();

    auto emit = [&](const Vec3D& a, const Vec3D& b, const Vec3D& c, const Vec3F& color) {
        auto normal = normalize(cross(c - a, b - a));

        mesh.emplace_back(a - center, normal, color);
        mesh.emplace_back(b - center, normal, color);
        mesh.emplace_back(c - center, normal, color);
    };

    auto find_opposite = [](const MPEPolyTriangle* tri, const MPEPolyPoint* a, const MPEPolyPoint* b) -> const MPEPolyPoint* {
        if (tri == nullptr)
            return nullptr;
        else if (tri->Points[0] == a && tri->Points[1] == b)
            return tri->Points[2];
        else if (tri->Points[1] == a && tri->Points[2] == b)
            return tri->Points[0];
        else
            return tri->Points[1];
    };

    for (size_t i = 0; i < poly_ctx.TriangleCount; ++i) {
        const auto* t = poly_ctx.Triangles[i];
        const auto* pa = t->Points[0];
        const auto* pb = t->Points[1];
        const auto* pc = t->Points[2];

        const auto* pd = find_opposite(t->Neighbors[2], pb, pa);
        const auto* pe = find_opposite(t->Neighbors[0], pc, pb);
        const auto* pf = find_opposite(t->Neighbors[1], pa, pc);

        TriangleProperties tp = {
            point_data[pa],
            point_data[pb],
            point_data[pc],
        };

        const size_t num_submerged = tp.a.submerged() + tp.b.submerged() + tp.c.submerged();

        // Land triangle
        if (num_submerged < 3) {
            Vec3F a = tp.a.normal * (param.radius + tp.a.land_height);
            Vec3F b = tp.b.normal * (param.radius + tp.b.land_height);
            Vec3F c = tp.c.normal * (param.radius + tp.c.land_height);

            Vec3F d, e, f;
            if (pd && !point_data[pd].submerged())
                d = point_data[pd].normal * point_data[pd].land_height;
            if (pe && !point_data[pe].submerged())
                e = point_data[pe].normal * point_data[pe].land_height;
            if (pf && !point_data[pf].submerged())
                f = point_data[pf].normal * point_data[pf].land_height;

            const auto color = this->triangle_generator(tp, false);
            emit(a, b, c, color);
        }

        // Water triangle
        if (num_submerged > 0) {
            Vec3F a = tp.a.normal * (param.radius + tp.a.water_height);
            Vec3F b = tp.b.normal * (param.radius + tp.b.water_height);
            Vec3F c = tp.c.normal * (param.radius + tp.c.water_height);

            Vec3F d, e, f;
            if (pd && point_data[pd].submerged())
                d = point_data[pd].normal * point_data[pd].land_height;
            if (pe && point_data[pe].submerged())
                e = point_data[pe].normal * point_data[pe].land_height;
            if (pf && point_data[pf].submerged())
                f = point_data[pf].normal * point_data[pf].land_height;

            const auto color = this->triangle_generator(tp, true);
            emit(a, b, c, color);
        }
    }

    return TerrainData(param.loc, std::move(mesh));
}

#endif

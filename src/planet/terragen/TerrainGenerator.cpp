#include "planet/terragen/TerrainGenerator.h"
#include <iostream>
#include <random>
#include <utility>
#include <unordered_map>
#include <fast-poly2tri/fastpoly2tri.h>
#include <noisepp/Noise.h>
#include "math/Vec.h"

namespace {
    const Vec3F GROUND_ALBEDO = Vec3F(0.15, 0.35, 0.08) * 0.2;
    const Vec3F WATER_ALBEDO = Vec3F(0.06, 0.08, 0.35) * 0.2;

    constexpr const size_t SIDE_POINTS = 200;
    constexpr const size_t INNER_POINTS[] = {1'000, 20'000};

    struct PointGenerator {
        size_t seed;
        noisepp::Pipeline3D pipeline;
        noisepp::Cache* cache;
        noisepp::PipelineElement3D* noise;

        explicit PointGenerator(size_t seed):
            seed(seed), cache(this->pipeline.createCache()) {
            auto ridged_multi = noisepp::RidgedMultiModule();
            ridged_multi.setSeed(seed);
            ridged_multi.setOctaveCount(8);
            ridged_multi.setFrequency(400);

            this->noise = this->pipeline.getElement(ridged_multi.addToPipe(this->pipeline));
        }

        PointGenerator(PointGenerator&& other):
            PointGenerator(other.seed) {
        }

        PointGenerator(const PointGenerator& other):
            PointGenerator(other.seed) {
        }

        PointGenerator& operator=(PointGenerator&& other) = delete;
        PointGenerator& operator=(const PointGenerator& other) = delete;

        PointProperties operator()(const Vec3D& pos) {
            this->pipeline.cleanCache(this->cache);
            return {
                this->noise->getValue(pos.x, pos.y, pos.z, this->cache) * 2'000,
                0
            };
        };

        ~PointGenerator() {
            this->pipeline.freeCache(this->cache);
        }
    };

    struct TriangleGenerator {
        Vec3F operator()(const TriangleProperties&, bool submerged) {
            return submerged ? WATER_ALBEDO : GROUND_ALBEDO;
        };
    };
}

bool PointProperties::submerged() const {
    return this->water_height > this->land_height;
}

TerrainGenerator::TerrainGenerator(ThreadPool& pool):
    pool(pool),
    point_callback(PointGenerator(0)),
    triangle_callback(TriangleGenerator{}) {
}

std::future<TerrainData> TerrainGenerator::generate(const TerrainGenerationParameters& param) {
    auto task = std::packaged_task<TerrainData()>(std::bind(&TerrainGenerator::generate_chunk_data, this, param));

    auto fut = task.get_future();
    this->pool.schedule(std::move(task));
    return fut;
}

TerrainData TerrainGenerator::generate_chunk_data(const TerrainGenerationParameters& param) {
    const size_t inner_points = INNER_POINTS[static_cast<size_t>(param.lod)];
    const size_t total_points = SIDE_POINTS * 3 + inner_points;

    auto mem = MPE_PolyAllocateMem(total_points);
    MPEPolyContext poly_ctx;
    MPE_PolyInitContext(&poly_ctx, mem.get(), total_points);
    
    auto point_data = std::unordered_map<const MPEPolyPoint*, PointData>(total_points);

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
        point_data[point] = {
            this->point_callback(surface_coord),
            surface_coord
        };
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

            for (size_t j = 0; j < SIDE_POINTS; ++j) {
                double k = static_cast<double>(j) / SIDE_POINTS;
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
        const double edge_dst = 1.0 / SIDE_POINTS;

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

    for (size_t i = 0; i < poly_ctx.TriangleCount; ++i) {
        const auto* t = poly_ctx.Triangles[i];
        const auto* pa = t->Points[0];
        const auto* pb = t->Points[1];
        const auto* pc = t->Points[2];
        
        TriangleProperties tp = {
            point_data[pa],
            point_data[pb],
            point_data[pc],
        };

        const size_t num_submerged = tp.a.prop.submerged() + tp.b.prop.submerged() + tp.c.prop.submerged();

        if (num_submerged < 3) {
            Vec3F a = tp.a.normal * (param.radius + tp.a.prop.land_height);
            Vec3F b = tp.b.normal * (param.radius + tp.b.prop.land_height);
            Vec3F c = tp.c.normal * (param.radius + tp.c.prop.land_height);

            const auto color = this->triangle_callback(tp, false);
            emit(a, b, c, color);
        }

        if (num_submerged > 0) {
            Vec3F a = tp.a.normal * (param.radius + tp.a.prop.water_height);
            Vec3F b = tp.b.normal * (param.radius + tp.b.prop.water_height);
            Vec3F c = tp.c.normal * (param.radius + tp.c.prop.water_height);

            const auto color = this->triangle_callback(tp, true);
            emit(a, b, c, color);
        }
    }

    return TerrainData(param.loc, std::move(mesh));
}

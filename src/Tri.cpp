#include "Tri.h"
#include <random>
#include <algorithm>
#include <utility>
#include <limits>
#include <iostream>
#include <chrono>
#include <cstddef>
#include <cmath>
#include "math/Vec.h"
#include "math/Mat.h"
#include "graphics/shader/ProgramBuilder.h"
#include "assets.h"

const float TAU = 3.14159264f * 2.f;
size_t NO_TRI = std::numeric_limits<size_t>::max();

enum class Side {
    Left,
    Right
};

Side side(const Vec3F& p0, const Vec3F& p1, const Vec3F& p2) {
    float v = (p2.x - p0.x) * (p1.z - p0.z) - (p1.x - p0.x) * (p2.z - p0.z);
    return v <= 0 ? Side::Left : Side::Right;
}

float det(const Mat3F& m) {
    float d = 0;

    d += m(0, 0) * (m(1, 1) * m(2, 1) - m(1, 2) * m(2, 2));
    d -= m(1, 0) * (m(0, 1) * m(2, 1) - m(0, 2) * m(2, 2));
    d += m(2, 0) * (m(0, 1) * m(1, 1) - m(0, 2) * m(1, 2));

    return d;
}

bool circum_test(const Vec3F& a, const Vec3F& b, const Vec3F& c, const Vec3F& d) {
    float adx = a.x - d.x;
    float adz = a.z - d.z;
    float bdx = b.x - d.x;
    float bdz = b.z - d.z;
    float cdx = c.x - d.x;
    float cdz = c.z - d.z;

    float ad = adx * (a.x + d.x) + adz * (a.z + d.z);
    float bd = bdx * (b.x + d.x) + bdz * (b.z + d.z);
    float cd = cdx * (c.x + d.x) + cdz * (c.z + d.z);

    float det = adx * (bdz * cd - bd * cdz);
    det -= bdx * (adz * cd - ad * cdz);
    det += cdx * (adz * bd - ad * bdz);
    return det > 0;
}

void legalize(const std::vector<Vec3F>& pts, std::vector<Triangle>& tris, size_t current, size_t& flipped) {
    auto& tri = tris[current];

    for (size_t edge = 0; edge < 3; ++edge) {
        size_t other_tri_i = tri.adjacent[edge];
        if (other_tri_i != NO_TRI) {
            auto& other = tris[other_tri_i];
            size_t other_edge = other.edge_of(current);
            size_t a_i = tri.p[(edge + 2) % 3];
            size_t b_i = tri.p[edge];
            size_t c_i = tri.p[(edge + 4) % 3];
            size_t d_i = other.opposing(other_edge);

            const Vec3F& a = pts[a_i];
            const Vec3F& b = pts[b_i];
            const Vec3F& c = pts[c_i];
            const Vec3F& d = pts[d_i];

            if (circum_test(a, b, c, d)) {
                ++flipped;
                auto t0 = Triangle(a_i, d_i, c_i);
                size_t t0_i = current;
                auto t1 = Triangle(d_i, a_i, b_i);
                size_t t1_i = other_tri_i;

                t0.adjacent[0] = t1_i;
                t0.adjacent[1] = other.adjacent[(other_edge + 2) % 3];
                t0.adjacent[2] = tri.adjacent[(edge + 1) % 3];
                if (t0.adjacent[1] != NO_TRI) {
                    auto& y = tris[t0.adjacent[1]];
                    y.adjacent[y.edge_of(other_tri_i)] = t0_i;
                }

                t1.adjacent[0] = t0_i;
                t1.adjacent[1] = tri.adjacent[(edge + 2) % 3];
                t1.adjacent[2] = other.adjacent[(other_edge + 1) % 3];
                if (t1.adjacent[1] != NO_TRI) {
                    auto& y = tris[t1.adjacent[1]];
                    y.adjacent[y.edge_of(current)] = t1_i;
                }

                tri = t0;
                other = t1;
                return;
            }
        }
    }
}

Triangulation::Triangulation(std::vector<Vec3F> points):
    points(points),
    index(3) {

    if (this->points.size() < 3)
        std::abort();

    std::sort(this->points.begin(), this->points.end(), [](const Vec3F& a, const Vec3F& b){
        return std::make_tuple(a.x, a.y) < std::make_tuple(b.x, b.y);
    });

    const Vec3F& p0 = this->points[0];
    const Vec3F& p1 = this->points[1];
    const Vec3F& p2 = this->points[2];

    if (side(p0, p1, p2) == Side::Left) {
        this->tris.emplace_back(0, 1, 2);
        this->hull.emplace_back(0, 1, 0, 0);
        this->hull.emplace_back(1, 2, 0, 1);
        this->hull.emplace_back(2, 0, 0, 2);
    } else {
        this->tris.emplace_back(1, 0, 2);
        this->hull.emplace_back(1, 0, 0, 0);
        this->hull.emplace_back(0, 2, 0, 1);
        this->hull.emplace_back(2, 1, 0, 2);
    }
}

bool Triangulation::advance() {
    auto insert_lower = [&](typename std::list<Edge>::iterator it, size_t from, size_t to, size_t left_tri) {
        // cycle iterator back to the end if it is begin
        auto it0 = --(it == this->hull.begin() ? this->hull.end() : it);

        if (it0->from == to && it0->to == from) {
            this->tris.back().adjacent[1] = it0->left_tri;
            this->tris[it0->left_tri].adjacent[2] = this->tris.size() - 1;
            this->hull.erase(it0);
        } else {
            this->hull.emplace(it, from, to, left_tri, 1);
        }
    };

    auto insert_upper = [&](typename std::list<Edge>::iterator it, size_t from, size_t to, size_t left_tri) {
        // cycle iterator back to begin if it is end.
        auto it0 = ++it == this->hull.end() ? this->hull.begin() : it;

        if (it0->from == to && it0->to == from) {
            this->tris.back().adjacent[2] = it0->left_tri;
            this->tris[it0->left_tri].adjacent[1] = this->tris.size() - 1;
            this->hull.erase(it0);
        } else {
            this->hull.emplace(it, from, to, left_tri, 2);
        }
    };

    if (this->index >= this->points.size()) {
        size_t flipped = 0;
        size_t j = 0;

        while(true) {
            size_t f = flipped;
            for (size_t i = 0; i < this->tris.size(); ++i) {
                legalize(this->points, this->tris, i, flipped);
            }
            std::cout << "Iter flipped: " << (flipped - f) << std::endl;
            if (f == flipped || j > 50)
                break;
            ++j;
        }

        std::cout << "Flipped: " << flipped << std::endl;
        return true;
    }

    const Vec3F& p = points[this->index];

    auto it = this->hull.begin();
    while (it != this->hull.end()) {
        const Vec3F& from = this->points[it->from];
        const Vec3F& to = this->points[it->to];
        if (side(from, to, p) == Side::Right) {
            size_t j = tris.size();
            this->tris.emplace_back(it->to, it->from, this->index);
            this->tris.back().adjacent[0] = it->left_tri;
            this->tris[it->left_tri].adjacent[it->tri_edge] = j;

            insert_lower(it, it->from, this->index, j);
            insert_upper(it, this->index, it->to, j);
            it = this->hull.erase(it);
        } else {
            ++it;
        }
    }

    ++this->index;
    return false;
}

std::vector<Vec3F> generate_points() {
    auto rd = std::random_device();
    auto seed = rd();
    std::cout << "Seed: " << seed << std::endl;
    auto gen = std::mt19937(seed);
    auto dist = std::uniform_real_distribution<float>(0.f, 1.f);

    auto points = std::vector<Vec3F>();

    std::generate_n(std::back_inserter(points), 1'000'000, [&] {
        float theta = dist(gen) * TAU;
        float r = std::sqrt(dist(gen));
        return Vec3F(std::cos(theta) * r, dist(gen) * 0.01f, std::sin(theta) * r);
    });

    return points;
}

std::vector<Vec3F> triangulation_to_mesh(const Triangulation& tr) {
    auto vertices = std::vector<Vec3F>(tr.tris.size() * 3 * 2);

    for (const Triangle& tri : tr.tris) {
        const Vec3F& a = tr.points[tri.p[0]];
        const Vec3F& b = tr.points[tri.p[1]];
        const Vec3F& c = tr.points[tri.p[2]];
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

Program load_shader_() {
    return ProgramBuilder()
        .with(ShaderType::Vertex, assets::passthrough_vs)
        .with(ShaderType::Fragment, assets::passthrough_fs)
        .link();
}

Tri::Tri():
    shader(load_shader_()),
    perspective(this->shader.uniform("uPerspective")),
    model(this->shader.uniform("uModel")),
    tr(generate_points()) {

    auto begin = std::chrono::high_resolution_clock::now();
    auto mid = std::chrono::high_resolution_clock::now();
    while (!this->tr.advance()) {
        if (this->tr.index >= this->tr.points.size())
            mid = std::chrono::high_resolution_clock::now();
        continue;
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::chrono::milliseconds::period> elapsed = end - begin;
    std::chrono::duration<double, std::chrono::milliseconds::period> elapsed1 = mid - begin;
    std::cout << "Phase 1 took " << elapsed1.count() << "ms" << std::endl;
    std::cout << "Generation took " << elapsed.count() << "ms" << std::endl;

    reupload();
}

void Tri::advance() {
    this->tr.advance();
    this->reupload();
}

void add_vector(std::vector<Vec3F>& data, const Vec3F& from, const Vec3F& to) {
    data.push_back(from);
    data.push_back(to);
    auto back = normalize(from - to);
    auto right = cross(Vec3F(0, 1, 0), back);
    data.push_back(to);
    data.push_back(to + back * 0.01f + right * 0.01f);
    data.push_back(to);
    data.push_back(to + back * 0.01f - right * 0.01f);
}

void Tri::reupload() {
    this->tri_vao.bind();
    this->tri_buffer.buffer.bind(GL_ARRAY_BUFFER);
    auto vertices = triangulation_to_mesh(this->tr);
    this->tri_buffer.size = vertices.size();
    Buffer::upload_data(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3F), 0);
    this->tri_vao.enable_attrib(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3F), reinterpret_cast<void*>(sizeof(Vec3F)));
    this->tri_vao.enable_attrib(1);

    this->pts_vao.bind();
    this->pts_buffer.buffer.bind(GL_ARRAY_BUFFER);
    this->pts_buffer.size = this->tr.points.size();
    Buffer::upload_data(GL_ARRAY_BUFFER, GL_STATIC_DRAW, this->tr.points);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3F), 0);
    this->pts_vao.enable_attrib(0);

    this->hull_vao.bind();
    this->hull_buffer.buffer.bind(GL_ARRAY_BUFFER);

    auto hull_data = std::vector<Vec3F>();
    // for (const Edge& e : this->tr.hull) {
    //     const Vec3F& from = this->tr.points[e.from];
    //     const Vec3F& to = this->tr.points[e.to];
    //     add_vector(hull_data, from, to);
    // }

    for (const Triangle& t : this->tr.tris) {
        for (size_t i = 0; i < 3; ++i) {
            if (t.adjacent[i] == NO_TRI)
                continue;

            const Vec3F& a = this->tr.points[t.p[i]];
            const Vec3F& b = this->tr.points[t.p[(i + 1) % 3]];
            auto m = (a + b) / 2.f;

            const Triangle& adj = this->tr.tris[t.adjacent[i]];
            auto target_tri = (this->tr.points[adj.p[0]] + this->tr.points[adj.p[1]] + this->tr.points[adj.p[2]]) / 3.f;
            auto d = (target_tri - m) * 0.9f;

            add_vector(hull_data, m, m + d);
        }
    }

    this->hull_buffer.size = hull_data.size();
    Buffer::upload_data(GL_ARRAY_BUFFER, GL_STATIC_DRAW, hull_data);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3F), 0);
    this->hull_vao.enable_attrib(0);
}

void Tri::render(const Mat4F& proj, const FreeCam& cam) {
    glPointSize(5);
    glLineWidth(1);
    this->shader.use();
    glUniformMatrix4fv(this->model, 1, GL_FALSE, cam.to_view_matrix().data());
    glUniformMatrix4fv(this->perspective, 1, GL_FALSE, proj.data());

    this->tri_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, this->tri_buffer.size / 2);

    // this->pts_vao.bind();
    // glDrawArrays(GL_POINTS, 0, this->pts_buffer.size);

    // glLineWidth(2);
    // this->hull_vao.bind();
    // glDrawArrays(GL_LINES, 0, this->hull_buffer.size);
}
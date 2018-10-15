#ifndef _OXYBELIS_TRI_H
#define _OXYBELIS_TRI_H

#include <vector>
#include <list>
#include <cstdlib>
#include "math/Mat.h"
#include "graphics/camera/FreeCam.h"
#include "graphics/VertexArray.h"
#include "graphics/Buffer.h"
#include "graphics/shader/Program.h"

extern size_t NO_TRI;

struct SizedBuffer {
    Buffer buffer;
    size_t size;
};

struct Edge {
    size_t from, to;
    size_t left_tri; // The triangle left of this edge
    size_t tri_edge;

    Edge(size_t from, size_t to, size_t left_tri, size_t tri_edge):
        from(from), to(to), left_tri(left_tri), tri_edge(tri_edge) {
    }
};

struct Triangle {
    size_t p[3];
    size_t adjacent[3];
    Edge* edges[3];

    Triangle(size_t p0, size_t p1, size_t p2):
        p{p0, p1, p2},
        adjacent{NO_TRI, NO_TRI, NO_TRI},
        edges{nullptr, nullptr, nullptr} {
    }

    size_t opposing(size_t edge) {
        return this->p[(edge + 2) % 3];
    }

    size_t edge_of(size_t tri) {
        for (size_t i = 0; i < 3; ++i) {
            if (this->adjacent[i] == tri)
                return i;
        }

        std::abort();
    }
};

struct Triangulation {
    std::vector<Vec3F> points;
    std::vector<Triangle> tris;
    std::list<Edge> hull;
    size_t index;
    size_t flipped;

    Triangulation(std::vector<Vec3F> points);
    bool advance();
};

class Tri {
    Program shader;
    Uniform perspective;
    Uniform model;

    Triangulation tr;

    VertexArray tri_vao;
    SizedBuffer tri_buffer;

    VertexArray pts_vao;
    SizedBuffer pts_buffer;

    VertexArray hull_vao;
    SizedBuffer hull_buffer;

public:
    Tri();
    void advance();
    void reupload();
    void render(const Mat4F& proj, const FreeCam& cam);
};

#endif

#ifndef _OXYBELIS_GRAPHICS_PLANET_PLANETRENDERER_H
#define _OXYBELIS_GRAPHICS_PLANET_PLANETRENDERER_H

#include <cstddef>
#include <variant>
#include <vector>
#include "graphics/planet/Planet.h"
#include "graphics/camera/FreeCam.h"
#include "graphics/shader/Program.h"
#include "graphics/Buffer.h"
#include "graphics/VertexArray.h"
#include "math/Vec.h"
#include "math/Mat.h"

struct SizedBuffer {
    Buffer buffer;
    size_t size;
};

struct Node;

struct Children {
    Node* children[4];

    Children(Node* a, Node* b, Node* c, Node* d):
        children{a, b, c, d} {
    }

    ~Children();
    void draw(const Vec3F& eye);
};

struct Leaf {
    VertexArray vao;
    SizedBuffer terrain;

    Leaf(const Vec3F& a, const Vec3F& b, const Vec3F& c);
    void draw();
};

struct Node {
    size_t level;
    Vec3F a, b, c;
    std::variant<Children, Leaf> var;

    Node(const Node&) = delete;
    Node(Node&&) = default;
    Node& operator=(const Node&) = delete;
    Node& operator=(Node&&) = default;

    Node(size_t level, const Vec3F& a, const Vec3F& b, const Vec3F& c);
    void draw(const Vec3F& eye);
    void subdivide();
    void unsubdivide();
};

class PlanetRenderer {
    Program shader;
    const Planet& planet;

    std::vector<Node> faces;

    Uniform perspective;
    Uniform model;

public:
    PlanetRenderer(const Planet& planet);

    void render(const Mat4F& proj, const FreeCam& cam);
};

#endif

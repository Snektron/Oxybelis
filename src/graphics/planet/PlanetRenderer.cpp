#include "graphics/planet/PlanetRenderer.h"
#include <cstdlib>
#include <memory>
#include "graphics/shader/ProgramBuilder.h"
#include "assets.h"

const Vec3F VERTICES[] = {
    Vec3F(0, -0.525731, 0.850651),
    Vec3F(0.850651, 0, 0.525731),
    Vec3F(0.850651, 0, -0.525731),
    Vec3F(-0.850651, 0, -0.525731),
    Vec3F(-0.850651, 0, 0.525731),
    Vec3F(-0.525731, 0.850651, 0),
    Vec3F(0.525731, 0.850651, 0),
    Vec3F(0.525731, -0.850651, 0),
    Vec3F(-0.525731, -0.850651, 0),
    Vec3F(0, -0.525731, -0.850651),
    Vec3F(0, 0.525731, -0.850651),
    Vec3F(0, 0.525731, 0.850651)
};

const uint8_t INDICES[] = {
    1, 6, 2,
    1, 2, 7,
    3, 5, 4,
    4, 8, 3,
    6, 11, 5,
    5, 10, 6,
    9, 2, 10,
    10, 3, 9,
    7, 9, 8,
    8, 0, 7,
    11, 1, 0,
    0, 4, 11,
    6, 10, 2,
    1, 11, 6,
    3, 10, 5,
    5, 11, 4,
    2, 9, 7,
    7, 0, 1,
    3, 8, 9,
    4, 0, 8,
};

Program load_shader() {
    return ProgramBuilder()
        .with(ShaderType::Vertex, assets::passthrough_vs)
        .with(ShaderType::Fragment, assets::passthrough_fs)
        .link();
}

void subdivide_ico_sphere(std::vector<Vec3F>& data, const Vec3F& a, const Vec3F& b, const Vec3F& c, float radius, unsigned level) {
    if (level == 0) {
        Vec3F x = normalize(a) * radius;
        Vec3F y = normalize(b) * radius;
        Vec3F z = normalize(c) * radius;

        auto normal = normalize(cross(c - a, b - a));
        data.push_back(x);
        data.push_back(normal);
        data.push_back(y);
        data.push_back(normal);
        data.push_back(z);
        data.push_back(normal);
    } else {
        Vec3F ab = normalize((a + b) / 2);
        Vec3F bc = normalize((b + c) / 2);
        Vec3F ac = normalize((a + c) / 2);

        subdivide_ico_sphere(data, ab, bc, ac, radius, level - 1);
        subdivide_ico_sphere(data, a, ab, ac, radius, level - 1);
        subdivide_ico_sphere(data, b, bc, ab, radius, level - 1);
        subdivide_ico_sphere(data, c, ac, bc, radius, level - 1);
    }
}

SizedBuffer generate_chunk(const Vec3F& a, const Vec3F& b, const Vec3F& c, float radius, size_t subdivs) {
    std::vector<Vec3F> data;

    subdivide_ico_sphere(data, a, b, c, radius, subdivs);

    return {
        Buffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, data),
        data.size()
    };
}

SizedBuffer create_iso_sphere(float radius, unsigned levels) {
    std::vector<Vec3F> data;

    for (unsigned i = 0; i < 20; ++i) {
        const Vec3F& a = VERTICES[INDICES[i * 3 + 0]];
        const Vec3F& b = VERTICES[INDICES[i * 3 + 1]];
        const Vec3F& c = VERTICES[INDICES[i * 3 + 2]];

        subdivide_ico_sphere(data, a, b, c, radius, levels);
    }

    return {
        Buffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, data),
        data.size()
    };
}

Children::~Children() {
    for (unsigned i = 0; i < 4; ++i)
        delete this->children[i];
}

void Children::draw(const Vec3F& eye) {
    for (unsigned i = 0; i < 4; ++i)
        this->children[i]->draw(eye);
}

Leaf::Leaf(const Vec3F& a, const Vec3F& b, const Vec3F& c):
    terrain(generate_chunk(a, b, c, 5, 2)) {

    this->vao.bind();
    this->terrain.buffer.bind(GL_ARRAY_BUFFER);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3F), 0);
    this->vao.enable_attrib(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3F), (void*)(3 * sizeof(GLfloat)));
    this->vao.enable_attrib(1);
}

void Leaf::draw() {
    this->vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, this->terrain.size / 2);
}

Node::Node(size_t level, const Vec3F& a, const Vec3F& b, const Vec3F& c):
    level(level),
    a(a), b(b), c(c),
    var(Leaf(a, b, c)) {
}

void Node::draw(const Vec3F& eye) {
    if (std::holds_alternative<Children>(this->var)) {
        std::get<Children>(this->var).draw(eye);
    } else {
        std::get<Leaf>(this->var).draw();
    }

    Vec3F mid = (this->a + this->b + this->c) / 3.f * 5.f;

    if (distance(eye, mid) < std::pow(this->level, 2.0) / 10.f)
        this->subdivide();
    else
        this->unsubdivide();
}

void Node::subdivide() {
    if (this->level != 0 && std::holds_alternative<Leaf>(this->var)) {
        Vec3F ab = normalize((this->a + this->b) / 2);
        Vec3F bc = normalize((this->b + this->c) / 2);
        Vec3F ac = normalize((this->a + this->c) / 2);

        this->var.emplace<Children>(
            new Node(this->level - 1, ab, bc, ac),
            new Node(this->level - 1, this->a, ab, ac),
            new Node(this->level - 1, this->b, bc, ab),
            new Node(this->level - 1, this->c, ac, bc)
        );
    }
}

void Node::unsubdivide() {
    if (std::holds_alternative<Children>(this->var)) {
        this->var.emplace<Leaf>(this->a, this->b, this->c);
    }
}

std::vector<Node> generate_faces(size_t levels) {
    std::vector<Node> f;

    for (unsigned i = 0; i < 20; ++i) {
        const Vec3F& a = VERTICES[INDICES[i * 3 + 0]];
        const Vec3F& b = VERTICES[INDICES[i * 3 + 1]];
        const Vec3F& c = VERTICES[INDICES[i * 3 + 2]];

        f.emplace_back(levels, a, b, c);
    }

    return f;
}

PlanetRenderer::PlanetRenderer(const Planet& planet):
    shader(load_shader()),
    planet(planet),
    faces(generate_faces(9)),
    perspective(this->shader.uniform("uPerspective")),
    model(this->shader.uniform("uModel")) {
}

void PlanetRenderer::render(const Mat4F& proj, const FreeCam& cam) {
    this->shader.use();

    glUniformMatrix4fv(this->model, 1, GL_FALSE, (cam.to_view_matrix() * this->planet.trans.to_matrix()).data());
    glUniformMatrix4fv(this->perspective, 1, GL_FALSE, proj.data());

    for (Node& n : this->faces) {
        n.draw(cam.translation);
    }
}

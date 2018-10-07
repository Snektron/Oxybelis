#include "graphics/planet/PlanetRenderer.h"
#include <vector>
#include <cstdlib>
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

void subdivide_ico_sphere(std::vector<Vec3F>& data, float r, const Vec3F& a, const Vec3F& b, const Vec3F& c, unsigned level) {
    if (level == 0) {

        Vec3F x = a * r * ((rand() / (float) RAND_MAX) * 0.001f + 1.f);
        Vec3F y = b * r * ((rand() / (float) RAND_MAX) * 0.001f + 1.f);
        Vec3F z = c * r * ((rand() / (float) RAND_MAX) * 0.001f + 1.f);

        auto normal = normalize(x + y + z);
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

        subdivide_ico_sphere(data, r, ab, bc, ac, level - 1);
        subdivide_ico_sphere(data, r, a, ab, ac, level - 1);
        subdivide_ico_sphere(data, r, b, bc, ab, level - 1);
        subdivide_ico_sphere(data, r, c, ac, bc, level - 1);
    }
}

Buffer create_iso_sphere(size_t& num_vertices, float r, unsigned levels) {
    std::vector<Vec3F> data;

    for (unsigned i = 0; i < 20; ++i) {
        const Vec3F& a = VERTICES[INDICES[i * 3 + 0]];
        const Vec3F& b = VERTICES[INDICES[i * 3 + 1]];
        const Vec3F& c = VERTICES[INDICES[i * 3 + 2]];

        subdivide_ico_sphere(data, r, a, b, c, levels);
    }

    num_vertices = data.size() / 2;
    std::cout << (num_vertices / 3) << std::endl;
    return Buffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, data);
}

PlanetRenderer::PlanetRenderer(const Planet& planet):
    shader(load_shader()),
    planet(planet),
    vertices(create_iso_sphere(this->num_vertices, planet.radius, 7)),
    perspective(this->shader.uniform("uPerspective")),
    model(this->shader.uniform("uModel")) {

    this->shader.use();
    Attribute a_vertex = this->shader.attribute("aVertex");
    Attribute a_normal = this->shader.attribute("aNormal");

    this->vao.bind();
    this->vertices.bind(GL_ARRAY_BUFFER);

    glVertexAttribPointer(a_vertex, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3F), 0);
    vao.enable_attrib(a_vertex);
    glVertexAttribPointer(a_normal, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3F), (void*)(3 * sizeof(GLfloat)));
    vao.enable_attrib(a_normal);
}

void PlanetRenderer::render(const Mat4F& proj, const FreeCam& cam) {
    this->shader.use();
    this->vao.bind();

    glUniformMatrix4fv(this->model, 1, GL_FALSE, (cam.to_view_matrix() * this->planet.trans.to_matrix()).data());
    glUniformMatrix4fv(this->perspective, 1, GL_FALSE, proj.data());

    glDrawArrays(GL_TRIANGLES, 0, this->num_vertices);
}

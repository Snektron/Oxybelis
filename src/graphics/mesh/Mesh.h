#ifndef _OXYBELIS_GRAPHICS_MESH_MESH_H
#define _OXYBELIS_GRAPHICS_MESH_MESH_H

#include "graphics/VertexArray.h"
#include "graphics/Buffer.h"

class Mesh {
    VertexArray vao;

public:

    void draw() const {

    }
};

#endif

/*

auto mesh = Mesh(shader);
mesh.add_attribute(shader.attribute("aPosition"), GL_ARRAY_BUFFER, 3, vertices);
mesh.add_attribute(shader.attribute("aNormal"), GL_ARRAY_BUFFER, 3, normals);

auto mesh = MeshBuilder()
    .add_attribute(shader.attribute("aPosition"), GL_ARRAY_BUFFER, 3, vertices)
    .add_attribute(shader.attribute("aNormal"), GL_ARRAY_BUFFER, 3, normals)
    .add_indices(indices)
    .build()

auto mesh = make_mesh(
    Attribute(shader.attribute("aPosition"), GL_ARRAY_BUFFER, 3, vertices),
    Attribute(shader.attribute("aNormal"), GL_ARRAY_BUFFER, 3, normals),
    Indices(indices)
);

shader.bind();
mesh.draw();



auto tess = Tesselator<Vec3F, Vec3F>();
auto add_tri = [&](const Vec3F& a, const Vec3F& b, const Vec3F& c) {
    auto fn = normalize(cross(b - a, c - a));
    tess.add_vertex(a, fn);
    tess.add_vertex(b, fn);
    tess.add_vertex(c, fn);
};

add_tri(x, y, z);

auto buffer = Buffer(GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, tess.data());

*/
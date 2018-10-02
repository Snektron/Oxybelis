#ifndef _OXYBELIS_GRAPHICS_MESH_H
#define _OXYBELIS_GRAPHICS_MESH_H

#include <vector>
#include <tuple>

template <typename... Ts>
class MeshBuilder {
    std::vector<std::tuple<Ts...>> data;

public:
    
};

class Mesh {

};

#endif
 /*
MeshBuilder<Vec3F, Vec3F, Vec2F> mb;
mb.add(Vec3F(0, 0, 0), Vec3F(0, 1, 0), Vec2F(0, 0));
mb.add(Vec3F(1, 0, 0), Vec3F(0, 1, 0), Vec2F(1, 0));
mb.add(Vec3F(1, 0, 1), Vec3F(0, 1, 0), Vec2F(1, 1));

mb.add(Vec3F(1, 0, 0), Vec3F(0, 1, 0), Vec2F(1, 0));
mb.add(Vec3F(1, 0, 1), Vec3F(0, 1, 0), Vec2F(1, 1));
mb.add(Vec3F(0, 0, 1), Vec3F(0, 1, 0), Vec2F(0, 1));

auto mesh = std::move(mb).build(shader, "aVertex", "aFaceNormal", "aTexCoord");
*/

#include "planet/icosahedron.h"

namespace icosahedron {
    extern const Vec3F VERTICES[] = {
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

    extern const uint8_t INDICES[] = {
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

    const Vec3F& vertex(size_t sector, uint8_t triangle_vertex) {
        return VERTICES[INDICES[sector * 3 + triangle_vertex]];
    }

    TriangleF sector(size_t sector) {
        return TriangleF(
            vertex(sector, 0),
            vertex(sector, 1),
            vertex(sector, 2)
        );
    }
}

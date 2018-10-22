#ifndef _OXYBELIS_PLANET_ICOSAHEDRON_H
#define _OXYBELIS_PLANET_ICOSAHEDRON_H

#include <cstdint>
#include "math/Vec.h"
#include "math/Triangle.h"

namespace icosahedron {
    constexpr const size_t NUM_VERTICES = 12;
    constexpr const size_t NUM_FACES = 20;

    extern const Vec3F VERTICES[NUM_VERTICES];
    extern const uint8_t INDICES[NUM_FACES * 3];

    const Vec3F& vertex(size_t face, uint8_t triangle_vertex);
    TriangleF face(size_t face);

    inline size_t face_of(const Vec3F& p) {
        for (size_t i = 0; i < NUM_FACES - 1; ++i) {
            if (face(i).sphere_classify(p) == 0)
                return i;
        }

        return NUM_FACES - 1;
    }
}

#endif

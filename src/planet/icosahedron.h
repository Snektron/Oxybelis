#ifndef _OXYBELIS_PLANET_ICOSAHEDRON_H
#define _OXYBELIS_PLANET_ICOSAHEDRON_H

#include <cstdint>
#include "math/Vec.h"
#include "math/Triangle.h"

namespace icosahedron {
    extern const Vec3F VERTICES[12];
    extern const uint8_t INDICES[20 * 3];

    const Vec3F& vertex(size_t sector, uint8_t triangle_vertex);
    TriangleF sector(size_t sector);
}

#endif

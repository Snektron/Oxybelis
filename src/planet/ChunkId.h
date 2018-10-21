#ifndef _OXYBELIS_PLANET_CHUNKID_H
#define _OXYBELIS_PLANET_CHUNKID_H

#include <cstdint>
#include "math/Vec.h"
#include "math/Triangle.h"
#include "planet/icosahedron.h"

class ChunkId {
    constexpr const static size_t SECTOR_WIDTH = 5; // lg(20) = 5 bits for sector
    constexpr const static size_t SECTOR_MASK = (1 << SECTOR_WIDTH) - 1;

    constexpr const static size_t QUADRANT_WIDTH = 2;
    constexpr const static size_t QUADRANT_MASK = (1 << QUADRANT_WIDTH) - 1;

    uint64_t id;

public:
    ChunkId(uint64_t raw);

    uint64_t raw() const;

    uint8_t sector() const;

    uint8_t quadrant(size_t depth) const;

    TriangleF to_triangle(size_t depth) const;

    template <typename F>
    void walk(size_t depth, F&& f) const;

    ChunkId child(size_t depth, uint8_t quadrant) const;
};

ChunkId::ChunkId(uint64_t raw):
    id(raw) {
}

uint64_t ChunkId::raw() const {
    return this->id;
}

uint8_t ChunkId::sector() const {
    return static_cast<uint8_t>(this->id & SECTOR_MASK);
}

uint8_t ChunkId::quadrant(size_t depth) const {
    return static_cast<uint8_t>((this->id >> ((depth + 1) * QUADRANT_WIDTH + SECTOR_WIDTH)) & QUADRANT_MASK);
}

TriangleF ChunkId::to_triangle(size_t depth) const {
    TriangleF tri = icosahedron::sector(this->sector());

    this->walk(depth, [&](size_t quadrant) {
        Vec3F ab = normalize((tri.a + tri.b) / 2.f);
        Vec3F bc = normalize((tri.b + tri.c) / 2.f);
        Vec3F ac = normalize((tri.a + tri.c) / 2.f);

        switch (quadrant) {
        case 0:
            tri.a = ab;
            tri.b = bc;
            tri.c = ac;
            break;
        case 1:
            tri.b = ab;
            tri.c = ac;
            break;
        case 2:
            tri.a = ab;
            tri.c = bc;
            break;
        case 3:
            tri.a = ac;
            tri.b = bc;
        };
    });

    return tri;
}

template <typename F>
void ChunkId::walk(size_t depth, F&& f) const {
    for (size_t i = 0; i < depth; ++i) {
        f(this->quadrant(i));
    }
}

ChunkId ChunkId::child(size_t depth, uint8_t quadrant) const {
    return ChunkId(this->id | ((quadrant & QUADRANT_MASK) << ((depth + 2) * QUADRANT_WIDTH + SECTOR_WIDTH)));
}

#endif

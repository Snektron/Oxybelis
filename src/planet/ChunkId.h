#ifndef _OXYBELIS_PLANET_CHUNKID_H
#define _OXYBELIS_PLANET_CHUNKID_H

#include <ostream>
#include <iterator>
#include <utility>
#include <functional>
#include <cstdint>
#include <cassert>
#include "math/Vec.h"
#include "math/Triangle.h"
#include "planet/icosahedron.h"
#include "utility/utility.h"

class ChunkId {
    constexpr const static size_t SECTOR_WIDTH = 5; // lg(20) = 5 bits for sector
    constexpr const static size_t SECTOR_MASK = (1 << SECTOR_WIDTH) - 1;

    constexpr const static size_t DEPTH_WIDTH = 5; // lg(32) = 5 bits for depth
    constexpr const static size_t DEPTH_MASK = (1 << DEPTH_WIDTH) - 1;

    constexpr const static size_t QUADRANT_WIDTH = 2;
    constexpr const static size_t QUADRANT_MASK = (1 << QUADRANT_WIDTH) - 1;

    uint64_t id;

public:
    constexpr explicit ChunkId(uint64_t raw):
        id(raw) {
    }

    constexpr ChunkId(uint8_t sector, uint8_t depth):
        id(sector | (depth << SECTOR_WIDTH)) {
    }

    constexpr uint64_t raw() const {
        return this->id;
    }

    constexpr uint8_t sector() const {
        return static_cast<uint8_t>(this->id & SECTOR_MASK);
    }

    constexpr uint8_t depth() const {
        return static_cast<uint8_t>(this->id >> SECTOR_WIDTH) & SECTOR_MASK;
    }

    constexpr uint8_t quadrant(size_t depth) const {
        assert(depth != 0 && depth <= this->depth());
        return static_cast<uint8_t>((this->id >> (depth * QUADRANT_WIDTH + DEPTH_WIDTH + SECTOR_WIDTH)) & QUADRANT_MASK);
    }

    constexpr void set_quadrant(size_t depth, uint64_t quadrant) {
        assert(depth != 0 && depth <= this->depth());
        assert(quadrant >= 0 && quadrant <= 3);
        this->id &= ~(QUADRANT_MASK << (depth * QUADRANT_WIDTH + DEPTH_WIDTH + SECTOR_WIDTH));
        this->id |= quadrant << (depth * QUADRANT_WIDTH + DEPTH_WIDTH + SECTOR_WIDTH);
    }

    template <typename F>
    void walk(F&& f) const {
        for (size_t i = 1; i <= this->depth(); ++i)
            f(this->quadrant(i));
    }

    constexpr ChunkId child(uint8_t quadrant) const {
        assert(quadrant >= 0 && quadrant <= 3);
        auto id = ChunkId(this->sector(), this->depth() + 1);
        id.set_quadrant(this->depth() + 1, quadrant);
        return id;
    }
};

std::ostream& operator<<(std::ostream& os, ChunkId id);

constexpr inline bool operator==(ChunkId lhs, ChunkId rhs) {
    return lhs.raw() == rhs.raw();
}

constexpr inline bool operator!=(ChunkId lhs, ChunkId rhs) {
    return !(lhs == rhs);
}

template<>
struct std::hash<ChunkId> {
    size_t operator()(ChunkId id) {
        return static_cast<size_t>(id.raw());
    }
};

struct ChunkLocation {
    ChunkId id;
    TriangleF corners;

    ChunkLocation(const Vec3F& p, size_t depth):
        id(icosahedron::face_of(p), depth), corners(icosahedron::face(this->id.sector())) {

        for (size_t i = 0; i < depth; ++i) {
            Vec3F ab = normalize(mix(this->corners.a, this->corners.b, 0.5f));
            Vec3F bc = normalize(mix(this->corners.b, this->corners.c, 0.5f));
            Vec3F ac = normalize(mix(this->corners.a, this->corners.c, 0.5f));

            size_t quadrant = TriangleF(ac, ab, bc).sphere_classify(p);
            this->id.set_quadrant(i + 1, quadrant);
            switch (quadrant) {
            case 0:
                this->corners.a = bc;
                this->corners.b = ac;
                this->corners.c = ab;
                break;
            case 1:
                this->corners.b = ab;
                this->corners.c = ac;
                break;
            case 2:
                this->corners.a = ab;
                this->corners.c = bc;
                break;
            case 3:
                this->corners.a = ac;
                this->corners.b = bc;
            };
        }
    } 
};

#endif

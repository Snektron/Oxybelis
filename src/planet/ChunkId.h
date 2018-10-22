#ifndef _OXYBELIS_PLANET_CHUNKID_H
#define _OXYBELIS_PLANET_CHUNKID_H

#include <ostream>
#include <iterator>
#include <utility>
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
    explicit ChunkId(uint64_t raw):
        id(raw) {
    }

    template <typename... Ts>
    ChunkId(uint8_t sector, Ts... quadrants):
        id((sector & SECTOR_MASK) | (sizeof...(quadrants) & DEPTH_MASK) << SECTOR_WIDTH) {

        pack_foreach([&, depth = 0](uint8_t quadrant) mutable {
            assert(quadrant >= 0 && quadrant <= 3);
            this->id |= static_cast<uint64_t>(quadrant & QUADRANT_MASK) << (++depth * QUADRANT_WIDTH + DEPTH_WIDTH + SECTOR_WIDTH);
        }, quadrants...);
    }

    uint64_t raw() const {
        return this->id;
    }

    uint8_t sector() const {
        return static_cast<uint8_t>(this->id & SECTOR_MASK);
    }

    uint8_t depth() const {
        return static_cast<uint8_t>(this->id >> SECTOR_WIDTH) & SECTOR_MASK;
    }

    uint8_t quadrant(size_t depth) const {
        assert(depth != 0 && depth <= this->depth());
        return static_cast<uint8_t>((this->id >> (depth * QUADRANT_WIDTH + DEPTH_WIDTH + SECTOR_WIDTH)) & QUADRANT_MASK);
    }

    TriangleF to_triangle() const {
        TriangleF tri = icosahedron::sector(this->sector());

        this->walk([&, even = false](size_t quadrant) mutable {
            Vec3F ab = normalize((tri.a + tri.b) / 2.f);
            Vec3F bc = normalize((tri.b + tri.c) / 2.f);
            Vec3F ac = normalize((tri.a + tri.c) / 2.f);

            switch (quadrant) {
            case 0:
                tri.c = ab;
                tri.a = bc;
                tri.b = ac;
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

            even ^= true;
        });

        return tri;
    }

    template <typename F>
    void walk(F&& f) const {
        for (size_t i = 1; i <= this->depth(); ++i)
            f(this->quadrant(i));
    }

    ChunkId child(uint8_t quadrant) const {
        assert(quadrant >= 0 && quadrant <= 3);
        uint64_t id = this->id;
        id &= ~(DEPTH_MASK << SECTOR_WIDTH);
        id |= (static_cast<uint64_t>(this->depth() + 1) & DEPTH_MASK) << SECTOR_WIDTH;
        id |= static_cast<uint64_t>(quadrant) << ((this->depth() + 1) * QUADRANT_WIDTH + DEPTH_WIDTH + SECTOR_WIDTH);
        return ChunkId(id);
    }

    ChunkId neighbor(size_t neighbor) const {
        // this->quadrant(this->depth()) == 0;
        
        uint64_t id = this->id;
        id &= ~(QUADRANT_MASK << ((this->depth()) * QUADRANT_WIDTH + DEPTH_WIDTH + SECTOR_WIDTH));
        id |= static_cast<uint64_t>(neighbor + 1) << ((this->depth()) * QUADRANT_WIDTH + DEPTH_WIDTH + SECTOR_WIDTH);
        return ChunkId(id);
    }

    friend bool operator==(ChunkId, ChunkId);
};

std::ostream& operator<<(std::ostream& os, ChunkId id);

bool operator==(ChunkId lhs, ChunkId rhs);

inline bool operator!=(ChunkId lhs, ChunkId rhs) {
    return !(lhs == rhs);
}

#endif

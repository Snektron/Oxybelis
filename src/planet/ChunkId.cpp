#include "planet/ChunkId.h"

std::ostream& operator<<(std::ostream& os, ChunkId id) {
    os << "ChunkId(Sector = " << static_cast<int>(id.sector());
    os << ", Depth = " << static_cast<int>(id.depth());
    os << ", Quadrants = {";

    id.walk([&, first = true](uint8_t quadrant) mutable {
        if (first)
            first = false;
        else
            os << ", ";
        os << static_cast<int>(quadrant);
    });

    return os << "})";
}
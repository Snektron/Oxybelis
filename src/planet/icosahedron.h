#ifndef _OXYBELIS_PLANET_ICOSAHEDRON_H
#define _OXYBELIS_PLANET_ICOSAHEDRON_H

#include <cstdint>
#include <array>
#include "math/Vec.h"
#include "math/Triangle.h"
#include "utility/utility.h"

namespace icosahedron {
    constexpr const double A = 0.525731112; // 1 / sqrt(1 + golden_ratio^2)
    constexpr const double B = 0.850650808; // golden_ratio / sqrt(1 + golden_ratio^2)

    struct Face {
        size_t a, b, c;
    };

    constexpr const std::array<Vec3D, 12> VERTICES = {
        Vec3D(0, -A, B),
        Vec3D(B, 0, A),
        Vec3D(B, 0, -A),
        Vec3D(-B, 0, -A),
        Vec3D(-B, 0, A),
        Vec3D(-A, B, 0),
        Vec3D(A, B, 0),
        Vec3D(A, -B, 0),
        Vec3D(-A, -B, 0),
        Vec3D(0, -A, -B),
        Vec3D(0, A, -B),
        Vec3D(0, A, B)
    };  

    constexpr const std::array<Face, 20> INDICES = {
        Face{1, 6, 2},
        Face{1, 2, 7},
        Face{3, 5, 4},
        Face{4, 8, 3},
        Face{6, 11, 5},
        Face{5, 10, 6},
        Face{9, 2, 10},
        Face{10, 3, 9},
        Face{7, 9, 8},
        Face{8, 0, 7},
        Face{11, 1, 0},
        Face{0, 4, 11},
        Face{6, 10, 2},
        Face{1, 11, 6},
        Face{3, 10, 5},
        Face{5, 11, 4},
        Face{2, 9, 7},
        Face{7, 0, 1},
        Face{3, 8, 9},
        Face{4, 0, 8}
    };

    constexpr const std::array<TriangleD, 20> FACES = initialize_array<20>([](size_t i) {
        const auto& face = INDICES[i];
        return TriangleD(VERTICES[face.a], VERTICES[face.b], VERTICES[face.c]);
    });

    constexpr size_t face_of(const Vec3D& p) {
        for (size_t i = 0; i < FACES.size() - 1; ++i) {
            if (FACES[i].sphere_classify(p) == 0)
                return i;
        }

        return FACES.size() - 1;
    }
}

#endif

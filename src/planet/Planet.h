#ifndef _OXYBELIS_PLANET_PLANET_H
#define _OXYBELIS_PLANET_PLANET_H

#include "math/Vec.h"
#include "planet/ChunkPatch.h"

struct Planet {
    Vec3D translation;
    double radius;

    friend class PlanetRenderer;
};

#endif

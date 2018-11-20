#ifndef _OXYBELIS_PLANET_RENDER_PLANETRENDERER_H
#define _OXYBELIS_PLANET_RENDER_PLANETRENDERER_H

#include "math/Vec.h"
#include "math/Mat.h"
#include "graphics/shader/Program.h"

struct Camera;
class Terrain;
struct Planet;

class TerrainRenderer {
    Program shader;
    Uniform perspective;
    Uniform model;
    Uniform camera_origin;

public:
    TerrainRenderer();
    void render(Terrain& terrain, const Mat4F& proj, const Camera& cam);
};

#endif

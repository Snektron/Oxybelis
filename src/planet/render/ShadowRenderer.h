#ifndef _OXYBELIS_PLANET_RENDER_SHADOWRENDERER_H
#define _OXYBELIS_PLANET_RENDER_SHADOWRENDERER_H

#include <cstddef>
#include "graphics/shader/Program.h"
#include "graphics/camera/Camera.h"
#include "graphics/Buffer.h"
#include "planet/render/Terrain.h"
#include "math/Mat.h"

struct Camera;
class Terrain;
class Chunk;

class ShadowRenderer {
    Program shadow_compute;
    Uniform num_vertices;

    Buffer shadow_volumes;
    Buffer counter;

public:
    ShadowRenderer();
    void begin();
    void dispatch(const Camera& cam, const Chunk& buffer);
    void end(const Mat4F& proj, const Camera& cam);

    void render(Terrain& terrain, const Mat4F& proj, const Camera& cam);

private:
    void reset_counter();
};

#endif

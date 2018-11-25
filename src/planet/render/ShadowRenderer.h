#ifndef _OXYBELIS_PLANET_RENDER_SHADOWRENDERER_H
#define _OXYBELIS_PLANET_RENDER_SHADOWRENDERER_H

#include <cstddef>
#include "graphics/shader/Program.h"
#include "graphics/camera/Camera.h"
#include "graphics/Buffer.h"
#include "graphics/VertexArray.h"
#include "planet/render/Terrain.h"
#include "math/Mat.h"

struct Camera;
class Terrain;
class Chunk;

class ShadowRenderer {
    Program shadow_compute;
    Uniform num_vertices;
    Uniform center;

    Program shadow_draw;
    Uniform perspective;
    Uniform model;

    VertexArray vao;
    Buffer shadow_volumes;
    Buffer counter;

public:
    ShadowRenderer(GLuint normal_distance_texture);
    void begin();
    void dispatch(const Chunk& chunk, const Camera& cam);
    void end(const Mat4F& proj, const Camera& cam);

    void render(Terrain& terrain, const Mat4F& proj, const Camera& cam);

private:
    void reset_counter();
};

#endif

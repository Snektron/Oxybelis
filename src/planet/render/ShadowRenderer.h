#ifndef _OXYBELIS_PLANET_RENDER_SHADOWRENDERER_H
#define _OXYBELIS_PLANET_RENDER_SHADOWRENDERER_H

#include <cstddef>
#include "graphics/shader/Program.h"
#include "graphics/camera/Camera.h"
#include "graphics/Buffer.h"
#include "graphics/VertexArray.h"
#include "graphics/FrameBuffer.h"
#include "graphics/RenderBuffer.h"
#include "graphics/Texture.h"
#include "math/Mat.h"
#include "math/Vec.h"
#include "planet/render/RenderInfo.h"

struct Camera;
class Chunk;
class Planet;

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

    struct FrameBufferState {
        FrameBuffer fb;
        Texture dndz, zminmax;
        RenderBuffer shadow_depth;

        FrameBufferState(const Vec2UI& dim);
    } state;

public:
    ShadowRenderer(const Vec2UI& dim, GLuint normal_distance_texture);
    void resize(const Vec2UI& dim);
    void render(const Planet& planet, const RenderInfo& info);

private:
    void prepare();
    void dispatch(const Chunk& chunk, const Camera& cam);
    void finish(const Mat4F& proj, const Camera& cam);
};

#endif

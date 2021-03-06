#ifndef _OXYBELIS_PLANET_RENDER_TERRAINRENDERER_H
#define _OXYBELIS_PLANET_RENDER_TERRAINRENDERER_H

#include "math/Vec.h"
#include "math/Mat.h"
#include "graphics/shader/Program.h"
#include "graphics/FrameBuffer.h"
#include "graphics/RenderBuffer.h"
#include "graphics/Texture.h"
#include "planet/render/RenderInfo.h"

struct Camera;
class Chunk;
class Planet;

class TerrainRenderer {
    Program shader;
    Uniform perspective;
    Uniform model;
    Uniform camera_origin;

    struct FrameBufferState {
        FrameBuffer fb;
        Texture color, distance;
        RenderBuffer depth;

        FrameBufferState(const Vec2UI& dim);
    } state;

public:
    TerrainRenderer(const Vec2UI& dim);
    void resize(const Vec2UI& dim);
    void render(const Planet& planet, const RenderInfo& info);

private:
    void prepare(const Mat4F& proj);
    void dispatch(const Chunk& chunk, const RenderInfo& info);
};

#endif

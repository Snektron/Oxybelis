#ifndef _OXYBELIS_OXYBELIS_H
#define _OXYBELIS_OXYBELIS_H

#include "graphics/camera/Camera.h"
#include "graphics/camera/Projection.h"
#include "graphics/FrameBuffer.h"
#include "graphics/RenderBuffer.h"
#include "graphics/Texture.h"
#include "input/InputContext.h"
#include "input/device/Mouse.h"
#include "math/Vec.h"
#include "planet/Planet.h"
#include "planet/terragen/earthlike.h"
#include "planet/render/AtmosphereRenderer.h"
#include "planet/render/TerrainRenderer.h"
#include "planet/render/ShadowRenderer.h"
#include "planet/render/Terrain.h"
#include "utility/ThreadPool.h"
#include "OxybelisInput.h"

class Oxybelis {
    struct FrameBufferState {
        FrameBuffer terrain_fb;
        Texture color, distance;
        RenderBuffer depth;

        FrameBuffer shadow_fb;
        Texture dndz, zminmax;
        RenderBuffer shadow_depth;

        FrameBufferState(const Vec2I& dim);
    };

    ThreadPool thread_pool;

    InputContext<Input> input_ctx;
    Mouse<Input>& mouse;
    bool cursor_captured;
    bool quit;

    Perspective projection;
    Camera camera;
    int camera_speed_modifier;
    double camera_speed;

    Planet planet;
    AtmosphereRenderer atmos;

    earthlike::Generator terragen;
    Terrain terrain;
    TerrainRenderer terraren;
    ShadowRenderer shadow;

    FrameBufferState fb_state;

public:
    Oxybelis(Mouse<Input>& mouse, const Vec2I& dim);
    bool update(double dt);
    void resize(const Vec2I& dim);
    void render();

    InputContext<Input>& input_context();

private:
    void toggle_cursor();
    void connect_camera();
    void update_camera_speed();
};

#endif

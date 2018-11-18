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
#include "planet/terragen/TerrainGenerator.h"
#include "planet/render/PrecomputedAtmosphereRenderer.h"
#include "planet/render/TerrainRenderer.h"
#include "utility/ThreadPool.h"
#include "OxybelisInput.h"

struct FrameBufferState {
    FrameBuffer fb;
    Texture color, distance;
    RenderBuffer depth;
};

class Oxybelis {
public:
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
    PrecomputedAtmosphereRenderer atmos;
    TerrainGenerator terragen;
    TerrainRenderer terraren;

    FrameBufferState fb_state;

    Oxybelis(Mouse<Input>& mouse, const Vec2I& dim);
    bool update(double dt);
    void resize(const Vec2I& dim);
    void render();

private:
    void toggle_cursor();
    void connect_camera();
    void update_camera_speed();
};

#endif

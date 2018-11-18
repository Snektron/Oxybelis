#ifndef _OXYBELIS_PLANET_RENDER_SHADOWRENDERER_H
#define _OXYBELIS_PLANET_RENDER_SHADOWRENDERER_H

#include "graphics/shader/Program.h"
#include "graphics/Buffer.h"

class ShadowRenderer {
    Program shadow_compute;
    Buffer shadow_volumes;

public:
    ShadowRenderer();
};

#endif

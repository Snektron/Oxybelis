#ifndef _OXYBELIS_PLANET_RENDER_SKYBOX_H
#define _OXYBELIS_PLANET_RENDER_SKYBOX_H

#include <array>
#include "core/Resource.h"
#include "graphics/Texture.h"

struct Skybox {
    Texture texture;

    Skybox(GLuint binding, const std::array<Resource, 6> faces);
};

#endif

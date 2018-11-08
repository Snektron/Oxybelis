#ifndef _OXYBELIS_GRAPHICS_MODELS_QUAD_H
#define _OXYBELIS_GRAPHICS_MODELS_QUAD_H

#include "graphics/VertexArray.h"
#include "graphics/Buffer.h"
#include "graphics/shader/Program.h"

class Quad {
    VertexArray vao;
    Buffer buffer;

public:
    Quad(Attribute vertices);
    void render() const;
};

#endif

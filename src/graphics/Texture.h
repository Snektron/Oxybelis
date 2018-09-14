#ifndef _OXYBELIS_GRAPHICS_TEXTURE_H
#define _OXYBELIS_GRAPHICS_TEXTURE_H

#include "glad/glad.h"
#include "graphics/GlObject.h"

class Texture {
    globject::Texture id;

    GLuint gen() {
        GLuint id;
        glGenTextures(1, &id);
        return id;
    }

public:
    Texture():
        id(gen()) {
    }

    void bind(GLuint target) {
        glBindBuffer(target, this->id);
    }

    inline operator GLuint() const {
        return this->id;
    }
};

#endif

#ifndef _OXYBELIS_GRAPHICS_TEXTURE_H
#define _OXYBELIS_GRAPHICS_TEXTURE_H

#include <glad/glad.h>
#include "graphics/GlObject.h"

class Texture {
    static GLuint gen_id() {
        GLuint id;
        glGenTextures(1, &id);
        return id;
    }

    static void destroy_id(GLuint texture) {
        glDeleteTextures(1, &texture);
    }

    GlObject<destroy_id> id;
public:
    Texture():
        id(gen_id()) {
    }

    void bind(GLuint target = GL_TEXTURE_2D) const {
        glBindTexture(target, this->id);
    }

    operator GLuint() const {
        return this->id;
    }
};

#endif

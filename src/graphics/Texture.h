#ifndef _OXYBELIS_GRAPHICS_TEXTURE_H
#define _OXYBELIS_GRAPHICS_TEXTURE_H

#include "glad/glad.h"

class Texture {
    GLuint handle;
public:
    Texture(const Texture&) = delete;
    Texture(Texture&&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = delete;

    Texture() {
        glGenTextures(1, &this->handle);
    }

    ~Texture() {
        glDeleteTextures(1, &this->handle);
    }

    inline bind(GLenum target = GL_TEXTURE_2D) {
        glBindTexture(target, this->handle);
    }

    inline operator GLuint() {
        return this->handle;
    }
};

#endif

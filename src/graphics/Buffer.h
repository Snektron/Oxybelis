#ifndef _OXYBELIS_GRAPHICS_BUFFER_H
#define _OXYBELIS_GRAPHICS_BUFFER_H

#include "glad/glad.h"
#include "graphics/GlObject.h"

class Buffer {
    globject::Buffer id;

    GLuint gen() {
        GLuint id;
        glGenBuffers(1, &id);
        return id;
    }

public:
    template <typename T>
    static Buffer make_static(const T* data, const size_t size, GLuint target = GL_ARRAY_BUFFER) {
        Buffer vbo;
        vbo.bind(target);
        glBufferData(target, size * sizeof(T), data, GL_STATIC_DRAW);
        return vbo;
    }

    Buffer():
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

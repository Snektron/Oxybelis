#ifndef _OXYBELIS_GRAPHICS_BUFFER_H
#define _OXYBELIS_GRAPHICS_BUFFER_H

#include <cstddef>
#include "glad/glad.h"
#include "graphics/GlObject.h"

class Buffer {
    static GLuint gen_id() {
        GLuint id;
        glGenBuffers(1, &id);
        return id;
    };

    static void destroy_id(GLuint buffer) {
        glDeleteBuffers(1, &buffer);
    }

    GlObject<destroy_id> buffer;

public:
    Buffer():
        buffer(gen_id()) {
    }

    template <typename T, size_t N>
    Buffer(GLuint target, GLenum usage, T (&data)[N]):
        Buffer() {
        this->bind(target);
        glBufferData(target, N * sizeof(T), data, usage);
    }

    void bind(GLuint target) {
        glBindBuffer(target, this->buffer);
    }
};

#endif

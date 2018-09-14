#ifndef _OXYBELIS_GRAPHICS_VERTEXARRAY_H
#define _OXYBELIS_GRAPHICS_VERTEXARRAY_H

#include "glad/glad.h"
#include "graphics/GlObject.h"

class VertexArray {
    globject::VertexArray id;

    GLuint gen() {
        GLuint id;
        glGenVertexArrays(1, &id);
        return id;
    }

public:
    VertexArray():
        id(gen()) {
    }

    void bind() {
        glBindVertexArray(this->id);
    }

    void enable_attrib(GLuint index) {
        glEnableVertexArrayAttrib(this->id, index);
    }

    void disable_attrib(GLuint index) {
        glDisableVertexArrayAttrib(this->id, index);
    }

    inline operator GLuint() const {
        return this->id;
    }
};

#endif

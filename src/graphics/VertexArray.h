#ifndef _OXYBELIS_GRAPHICS_VERTEXARRAY_H
#define _OXYBELIS_GRAPHICS_VERTEXARRAY_H

#include "glad/glad.h"
#include "graphics/GlObject.h"

class VertexArray {
    static GLuint gen_id() {
        GLuint id;
        glGenVertexArrays(1, &id);
        return id;
    }

    static void destroy_id(GLuint vao) {
        glDeleteVertexArrays(1, &vao);
    }

    GlObject<destroy_id> vao;

public:
    VertexArray():
        vao(gen_id()) {
    }

    void bind() {
        glBindVertexArray(this->vao);
    }

    void enable_attrib(GLuint index) {
        glEnableVertexArrayAttrib(this->vao, index);
    }

    void disable_attrib(GLuint index) {
        glDisableVertexArrayAttrib(this->vao, index);
    }
};

#endif

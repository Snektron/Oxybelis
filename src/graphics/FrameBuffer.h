#ifndef _OXYBELIS_GRAPHICS_FRAMEBUFFER_H
#define _OXYBELIS_GRAPHICS_FRAMEBUFFER_H

#include "glad/glad.h"
#include "graphics/GlObject.h"

class FrameBuffer {
    static GLuint gen_id() {
        GLuint id;
        glGenFramebuffers(1, &id);
        return id;
    }

    static void destroy_id(GLuint fbo) {
        glDeleteFramebuffers(1, &fbo);
    }

    GlObject<destroy_id> fbo;

    FrameBuffer(GLuint id):
        fbo(id) {
    }

public:
    FrameBuffer():
        fbo(gen_id()) {
    }

    static FrameBuffer screen() {
        return FrameBuffer(0);
    }

    void bind(GLuint target = GL_FRAMEBUFFER) const {
        glBindFramebuffer(target, this->fbo);
    }

    operator GLuint() const {
        return this->fbo;
    }
};

#endif

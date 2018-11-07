#ifndef _OXYBELIS_GRAPHICS_RENDERBUFFER_H
#define _OXYBELIS_GRAPHICS_RENDERBUFFER_H

class RenderBuffer {
    static GLuint gen_id() {
        GLuint id;
        glGenRenderbuffers(1, &id);
        return id;
    }

    static void destroy_id(GLuint texture) {
        glDeleteRenderbuffers(1, &texture);
    }

    GlObject<destroy_id> id;
public:
    RenderBuffer():
        id(gen_id()) {
    }

    RenderBuffer(GLenum format, GLsizei width, GLsizei height):
        RenderBuffer(0, format, width, height) {
    }

    RenderBuffer(GLsizei samples, GLenum format, GLsizei width, GLsizei height):
        RenderBuffer() {
        this->bind();
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);
    }

    void bind() const {
        glBindRenderbuffer(GL_RENDERBUFFER, this->id);
    }

    operator GLuint() const {
        return this->id;
    }
};


#endif

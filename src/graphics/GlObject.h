#ifndef _OXYBELIS_GRAPHICS_GLOBJECT_H
#define _OXYBELIS_GRAPHICS_GLOBJECT_H

#include <utility>
#include "glad/glad.h"

namespace globject {
    using DeleteFn = void(GLuint);

    template <DeleteFn Delete>
    class GlObject {
        GLuint handle;

    public:
        GlObject(const GlObject&) = delete;
        GlObject& operator=(const GlObject&) = delete;

        explicit GlObject(GLuint handle):
            handle(handle) {
        }

        GlObject(GlObject&& other):
            handle(other.handle) {
            other.handle = 0;
        }

        GlObject& operator=(GlObject&& other) {
            std::swap(this->handle, other.handle);
        }

        ~GlObject() {
            Delete(this->handle);
        }

        inline operator GLuint() const {
            return this->handle;
        }
    };

    inline void delete_shader(GLuint shader) {
        glDeleteShader(shader);
    }

    using Shader = GlObject<delete_shader>;

    inline void delete_program(GLuint program) {
        glDeleteProgram(program);
    }

    using Program = GlObject<delete_program>;

    inline void delete_texture(GLuint texture) {
        glDeleteTextures(1, &texture);
    }

    using Texture = GlObject<delete_texture>;

    inline void delete_buffer(GLuint buffer) {
        glDeleteBuffers(1, &buffer);
    }

    using Buffer = GlObject<delete_buffer>;

    inline void delete_vertex_array(GLuint array) {
        glDeleteVertexArrays(1, &array);
    }

    using VertexArray = GlObject<delete_vertex_array>;
}

#endif

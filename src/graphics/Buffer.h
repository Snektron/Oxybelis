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
        upload_data(target, usage, data);
    }

    template <typename T>
    Buffer(GLuint target, GLenum usage, const T& container):
        Buffer() {
        this->bind(target);
        upload_data(target, usage, container);
    }

    void bind(GLuint target) const {
        glBindBuffer(target, this->buffer);
    }

    operator GLuint() const {
        return this->buffer;
    }

    template <typename T, size_t N>
    static void upload_data(GLuint target, GLenum usage, T(&data)[N]) {
        glBufferData(target, N * sizeof(T), data, usage);
    }

    template <typename T>
    static void upload_data(GLuint target, GLenum usage, const T& container) {
        glBufferData(target, container.size() * sizeof(typename T::value_type), container.data(), usage);
    }

    template <typename T, size_t N>
    static void upload_sub_data(GLuint target, GLintptr offset, T (&data)[N]) {
        glBufferSubData(target, offset, N * sizeof(T), data);
    }
};

#endif

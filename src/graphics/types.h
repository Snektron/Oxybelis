#ifndef _OXYBELIS_GRAPHICS_TYPES_H
#define _OXYBELIS_GRAPHICS_TYPES_H

#include <cstdint>

template <typename T>
struct GlType;

template <>
struct GlType<GLbyte> {
    constexpr const static GLenum type_id = GL_BYTE;
};

template <>
struct GlType<GLubyte> {
    constexpr const static GLenum type_id = GL_UNSIGNED_BYTE;
};

template <>
struct GlType<GLshort> {
    constexpr const static GLenum type_id = GL_SHORT;
};

template <>
struct GlType<GLushort> {
    constexpr const static GLenum type_id = GL_UNSIGNED_SHORT;
};

template <>
struct GlType<GLint> {
    constexpr const static GLenum type_id = GL_INT;
};

template <>
struct GlType<GLuint> {
    constexpr const static GLenum type_id = GL_UNSIGNED_INT;
};

template <>
struct GlType<GLfixed> {
    constexpr const static GLenum type_id = GL_FIXED;
};

template <>
struct GlType<GLhalf> {
    constexpr const static GLenum type_id = GL_HALF_FLOAT;
};

template <>
struct GlType<GLfloat> {
    constexpr const static GLenum type_id = GL_FLOAT;
};

template <>
struct GlType<GLdouble> {
    constexpr const static GLenum type_id = GL_DOUBLE;
};

#endif

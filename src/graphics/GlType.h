#ifndef _OXYBELIS_GRAPHICS_GLTYPE_H
#define _OXYBELIS_GRAPHICS_GLTYPE_H

#include "glad/glad.h"

template <typename T>
struct GlType;

template <>
struct GlType<GLbyte> {
    constexpr const static GLenum gl_type_id = GL_BYTE;
};

#endif

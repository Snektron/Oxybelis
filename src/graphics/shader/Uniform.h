#ifndef _OXYBELIS_GRAPHICS_SHADER_UNIFORM_H
#define _OXYBELIS_GRAPHICS_SHADER_UNIFORM_H

#include "glad/glad.h"

class Uniform {
    const GLint uniform_location;

public:
    Uniform(GLint location):
        uniform_location(location) {
    }

    GLint location() const {
        return this->uniform_location;
    }

    inline operator GLint() const {
        return this->location();
    }
};

#endif

#ifndef _OXYBELIS_GRAPHICS_SHADER_ATTRIBUTE_H
#define _OXYBELIS_GRAPHICS_SHADER_ATTRIBUTE_H

#include "glad/glad.h"

class Attribute {
    const GLint attrib_location;

public:
    Attribute(GLint location):
        attrib_location(location) {
    }

    GLint location() const {
        return this->attrib_location;
    }

    inline operator GLint() const {
        return this->location();
    }
};

#endif

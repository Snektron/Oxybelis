#ifndef _OXYBELIS_GRAPHICS_SHADER_PROGRAM_H
#define _OXYBELIS_GRAPHICS_SHADER_PROGRAM_H

#include "experimental/string_view"
#include "graphics/GlObject.h"
#include "utility/CStringView.h"

using Uniform = GLint;
using Attribute = GLint;

class Program {
    globject::Program program;

    Program(globject::Program&& program):
        program(std::move(program)) {
    }
public:
    void use() const {
        glUseProgram(this->program);
    }

    Uniform uniform(CStringView name) const {
        return glGetUniformLocation(*this, name.data());
    }

    Attribute attribute(CStringView name) const {
        return glGetAttribLocation(*this, name.data());
    }

    inline operator GLuint() const {
        return this->program;
    }

    friend class ProgramBuilder;
};

#endif

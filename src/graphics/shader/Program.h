#ifndef _OXYBELIS_GRAPHICS_SHADER_PROGRAM_H
#define _OXYBELIS_GRAPHICS_SHADER_PROGRAM_H

#include "graphics/GlObject.h"

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

    Uniform uniform(const GLchar* name) const {
        return glGetUniformLocation(*this, name);
    }

    Uniform uniform(const std::string& name) const {
        return glGetUniformLocation(*this, name.c_str());
    }

    Attribute attribute(const GLchar* name) const {
        return glGetAttribLocation(*this, name);
    }

    Attribute attribute(const std::string& name) const {
        return glGetAttribLocation(*this, name.c_str());
    }

    inline operator GLuint() const {
        return this->program;
    }

    friend class ProgramBuilder;
};

#endif

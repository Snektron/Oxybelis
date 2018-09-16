#ifndef _OXYBELIS_GRAPHICS_SHADER_PROGRAMBUILDER_H
#define _OXYBELIS_GRAPHICS_SHADER_PROGRAMBUILDER_H

#include <vector>
#include <string>
#include <stdexcept>
#include "graphics/GlObject.h"
#include "graphics/shader/Shader.h"
#include "graphics/shader/Program.h"
#include "utility/CStringView.h"

class ProgramLinkException: public std::runtime_error {
public:
    ProgramLinkException(const std::string message):
        std::runtime_error(message) {
    }
};

class ProgramBuilder {
    globject::Program program;
    std::vector<GLuint> attached_shaders;

public:
    ProgramBuilder():
        program(glCreateProgram()) {
    }

    auto with(const Shader& shader) {
        glAttachShader(this->program, shader);
        attached_shaders.push_back(shader);
        return std::forward<ProgramBuilder>(*this);
    }

    template <typename... Sources>
    auto with(ShaderType type, const Sources&... sources) {
        return std::forward<ProgramBuilder>(this->with(Shader(type, sources...)));
    }

    inline auto bind_attrib_location(const GLuint index, CStringView name) {
        glBindAttribLocation(this->program, index, name.data());
        return std::forward<ProgramBuilder>(*this);
    }

    inline auto bind_frag_data_location(GLuint color_number, CStringView name) {
        glBindFragDataLocation(this->program, color_number, name.data());
        return std::forward<ProgramBuilder>(*this);
    }

    Program link() && {
        glLinkProgram(this->program);
        if (this->parameter(GL_LINK_STATUS) != GL_TRUE)
            throw ProgramLinkException(this->info_log());
        
        for (auto shader : this->attached_shaders)
            glDetachShader(this->program, shader);
        
        return std::move(this->program);
    }

    std::string info_log() const {
        GLint length = this->parameter(GL_INFO_LOG_LENGTH);
        GLchar log[length];
        glGetProgramInfoLog(this->program, length, nullptr, log);
        return std::string(log, log + length);
    }

    inline GLint parameter(GLenum parameter) const {
        GLint result;
        glGetProgramiv(this->program, parameter, &result);
        return result;
    }

    inline operator GLuint() const {
        return this->program;
    }
};

#endif

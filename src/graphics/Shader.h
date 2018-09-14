#ifndef _OXYBELIS_GRAPHICS_SHADER_H
#define _OXYBELIS_GRAPHICS_SHADER_H

#include <array>
#include <vector>
#include <string>
#include <cstddef>
#include <stdexcept>
#include "core/Resource.h"
#include "graphics/GlObject.h"

enum class ShaderType: GLenum {
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
    Geometry = GL_GEOMETRY_SHADER,
    TessEvaluation = GL_TESS_EVALUATION_SHADER,
    TessControl = GL_TESS_CONTROL_SHADER,
    Compute = GL_COMPUTE_SHADER
};

class ShaderCompilationException: public std::runtime_error {
public:
    ShaderCompilationException(const std::string message):
        std::runtime_error(message) {
    }
};

class ProgramLinkException: public std::runtime_error {
public:
    ProgramLinkException(const std::string message):
        std::runtime_error(message) {
    }
};

class Shader {
    globject::Shader shader;

public:
    template <typename... Sources>
    Shader(ShaderType type, const Sources&... sources):
        shader(glCreateShader(static_cast<GLenum>(type))) {

        constexpr const size_t num_sources = sizeof...(Sources);

        auto get_source = make_overload(
            [](const Resource& src) -> const GLchar* { return src.data(); },
            [](const std::string& src) -> const GLchar* { return src.data(); },
            [](const char* src) -> const GLchar* { return src; }
        );

        auto get_size = make_overload(
            [](const Resource& src) -> GLint { return src.size(); },
            [](const std::string& src) -> GLint { return src.size(); },
            [](const char*) -> GLint { return -1; }
        );

        const GLchar* source_ptrs[] = {get_source(sources)...};
        GLint source_sizes[] = {get_size(sources)...};

        glShaderSource(this->shader, static_cast<GLsizei>(num_sources), source_ptrs, source_sizes);
        glCompileShader(this->shader);

        if (this->parameter(GL_COMPILE_STATUS) != GL_TRUE)
            throw ShaderCompilationException(this->info_log());
    }

    std::string info_log() const {
        GLint length = this->parameter(GL_INFO_LOG_LENGTH);
        GLchar log[length];
        glGetShaderInfoLog(this->shader, length, nullptr, log);
        return std::string(log, log + length);
    }

    inline GLint parameter(GLenum parameter) const {
        GLint result;
        glGetShaderiv(this->shader, parameter, &result);
        return result;
    }

    inline operator GLuint() const {
        return this->shader;
    }
};

class Program {
    globject::Program program;

    Program(globject::Program&& program):
        program(std::move(program)) {
    }
public:

    friend class ProgramBuilder;
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

    auto bind_attrib_location(const GLuint index, const GLchar* name) {
        glBindAttribLocation(this->program, index, name);
        return std::forward<ProgramBuilder>(*this);
    }

    auto bind_attrib_location(const GLuint index, const std::string& name) {
        glBindAttribLocation(this->program, index, static_cast<const GLchar*>(name.c_str()));
        return std::forward<ProgramBuilder>(*this);
    }

    auto bind_frag_data_location(GLuint color_number, const char* name) {
        glBindFragDataLocation(this->program, color_number, name);
        return std::forward<ProgramBuilder>(*this);
    }

    auto bind_frag_data_location(GLuint color_number, const std::string& name) {
        glBindFragDataLocation(this->program, color_number, static_cast<const GLchar*>(name.c_str()));
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

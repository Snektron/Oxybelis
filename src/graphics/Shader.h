#ifndef _OXYBELIS_GRAPHICS_SHADER_H
#define _OXYBELIS_GRAPHICS_SHADER_H

#include <array>
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
};

#endif

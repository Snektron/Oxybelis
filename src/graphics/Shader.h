#ifndef _OXYBELIS_GRAPHICS_SHADER_H
#define _OXYBELIS_GRAPHICS_SHADER_H

#include <vector>
#include <string>
#include <algorithm>
#include <cstddef>
#include "core/Resource.h"

class Shader {
    GLuint shader;
    std::vector<const char*> sources;
    std::vector<GLint> lengths;

public:
    Shader(GLenum type):
        shader(glCreateShader(type)) {
    }

    Shader(GLenum type, const Resource& source):
        shader(glCreateShader(type)) {
        this->add_source(source);
    }

    Shader(GLenum type, const std::string& source):
        shader(glCreateShader(type)) {
        this->add_source(source);
    }

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    
    Shader(Shader&& other):
        shader(other.shader) {
        other.shader = 0;
    }

    Shader& operator=(Shader&& other) {
        glDeleteShader(this->shader);
        this->shader = other.shader;
        other.shader = 0;
        return *this;
    }

    Shader& add_source(const Resource& source) {
        this->sources.push_back(source.data);
        this->lengths.push_back(static_cast<GLint>(source.size));
        return *this;
    }

    Shader& add_source(const std::string& source) {
        this->sources.push_back(source.data());
        this->lengths.push_back(static_cast<GLint>(source.size()));
        return *this;
    }

    GLuint compile() && {
        glShaderSource(this->shader, this->sources.size(), sources.data(), lengths.data());
        glCompileShader(this->shader);

        GLuint shader = this->shader;
        this->shader = 0;
        return shader;
    }

    ~Shader() {
        glDeleteShader(this->shader);
    }
};

class Program {
    GLuint program;

public:
    Program(const Program&) = delete;
    Program(Program&&) = delete;
    Program& operator=(const Program&) = delete;
    Program& operator=(Program&&) = delete;

    Program():
        program(glCreateProgram()) {

    }

    ~Program() {
        glDeleteProgram(this->program);
    }

    inline void use() {
        glUseProgram(this->program);
    }
};

#endif

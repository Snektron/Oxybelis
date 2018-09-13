#ifndef _OXYBELIS_GRAPHICS_SHADER_H
#define _OXYBELIS_GRAPHICS_SHADER_H

#include <vector>

class Shader {
    GLuint shader;
    std::vector<>;

public:
    Shader(GLenum type):
        shader(glCreateShader(type)) {
    }

    Shader(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&&) = delete;


    ~Shader() {
        gLDeleteShader(this->shader);
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

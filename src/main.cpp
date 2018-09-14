#include <iostream>
#include <GLFW/glfw3.h>
#include "glad/glad.h"
#include "core/Window.h"
#include "math/Matrix.h"
#include "graphics/Error.h"
#include "graphics/GlObject.h"
#include "graphics/shader/ProgramBuilder.h"
#include "assets.h"


int main() {
    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "Failed to initializ GLFW" << std::endl;
        return 1;
    }

    auto _ = defer([](){
        glfwTerminate();
    });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Window window(640, 480, "Test Window");
    window.make_context_current();

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    
    glClearColor(.97f, .97f, .97f, .97f);

    auto program = ProgramBuilder()
        .with(ShaderType::Vertex, assets::passthrough_vs)
        .with(ShaderType::Fragment, assets::passthrough_fs)
        .link();

    float vertices[] = {
        -.5f, -.5f,
        0.f, .5f,
        .5f, -.5f
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //const auto cam = make_orthographic(-1.f, 1.f, 1.f, -1.f, 0.1f, 1.f);
    auto cam = make_perspective(1.f, 1.17f, 0.1f, 10.f);

    cam *= make_translation(0.f, 0.f, 0.f);

    program.use();
    Attribute aVertex = program.attribute("aVertex");
    Uniform uMvp = program.uniform("uMvp");

    glUniformMatrix4fv(uMvp, 1, GL_FALSE, cam.data());

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(aVertex, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(aVertex);

    while (!window.should_close())
    {
        auto dim = window.dimensions();
        glViewport(0, 0, dim.x(), dim.y());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        window.swap_buffers();
        glfwPollEvents();
        assert_gl();
    }

    return 0;
}
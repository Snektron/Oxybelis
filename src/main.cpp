#include <iostream>
#include <GLFW/glfw3.h>
#include "glad/glad.h"
#include "core/Window.h"
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

    while (!window.should_close())
    {
        auto dim = window.dimensions();
        glViewport(0, 0, dim.x(), dim.y());
        glClear(GL_COLOR_BUFFER_BIT);

        window.swap_buffers();
        glfwPollEvents();
        assert_gl();
    }

    return 0;
}
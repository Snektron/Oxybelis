#include <iostream>
#include <GLFW/glfw3.h>
#include "glad/glad.h"
#include "core/Window.h"
#include "graphics/Error.h"
#include "graphics/Shader.h"

int main() {
    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "Failed to initializ GLFW" << std::endl;
        return 1;
    }

    auto _ = defer([](){
        glfwTerminate();
    });

    Window window(640, 480, "Test Window");
    window.make_context_current();

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    
    glClearColor(.97f, .97f, .97f, .97f);

    while (!window.should_close())
    {
        auto dim = window.dimensions();
        glViewport(0, 0, dim.x(), dim.y());
        glClear(GL_COLOR_BUFFER_BIT);

        glColor3f(.8f, 0.f, 0.f);

        glBegin(GL_TRIANGLES);
            glVertex2f(-.5f, -.5f);
            glVertex2f(0, .5f);
            glVertex2f(.5f, -.5f);
        glEnd();

        window.swap_buffers();
        glfwPollEvents();
        assert_gl();
    }

    return 0;
}
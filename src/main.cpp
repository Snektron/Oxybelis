#include <iostream>
#include <GLFW/glfw3.h>
#include "glad/glad.h"
#include "core/Window.h"

int main() {
    glfwInit();

    {
        Window window(640, 480, "Test Window");
        window.make_context_current();

        gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
        
        glClearColor(.97f, .97f, .97f, .97f);

        while (!window.should_close())
        {
            glClear(GL_COLOR_BUFFER_BIT);

            glColor3f(.8f, 0.f, 0.f);

            glBegin(GL_TRIANGLES);
                glVertex2f(-.5f, -.5f);
                glVertex2f(0, .5f);
                glVertex2f(.5f, -.5f);
            glEnd();

            window.swap_buffers();
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}
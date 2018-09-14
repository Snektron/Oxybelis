#ifndef _OXYBELIS_CORE_WINDOW_H
#define _OXYBELIS_CORE_WINDOW_H

#include <GLFW/glfw3.h>
#include "math/Vector.h"
#include "utility/utility.h"

class Window {
    CPtr<GLFWwindow, glfwDestroyWindow> handle;

public:
    Window(size_t width, size_t height, const char* title):
        handle(glfwCreateWindow(width, height, title, nullptr, nullptr)) {
    }

    inline void make_context_current() {
        glfwMakeContextCurrent(this->handle.get());
    }

    inline bool should_close() {
        return glfwWindowShouldClose(this->handle.get());
    }

    inline void swap_buffers() {
        glfwSwapBuffers(this->handle.get());
    }

    inline Vector2<int> dimensions() {
        Vector2<int> dim;
        glfwGetWindowSize(this->handle.get(), &dim.x(), &dim.y());
        return dim;
    } 
};

#endif

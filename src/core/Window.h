#ifndef _OXYBELIS_CORE_WINDOW_H
#define _OXYBELIS_CORE_WINDOW_H

#include <GLFW/glfw3.h>
#include <functional>
#include "math/Vector.h"
#include "utility/utility.h"

class Window {
    using KeyCallback = std::function<void(Window&, int, int, int, int)>;

    CPtr<GLFWwindow, glfwDestroyWindow> handle;

public:
    KeyCallback kcbk;

    Window(size_t width, size_t height, const char* title):
        handle(glfwCreateWindow(width, height, title, nullptr, nullptr)) {
        glfwSetWindowUserPointer(this->handle.get(), this);
        glfwSetKeyCallback(this->handle.get(), key_callback);
        glfwSetCursorPosCallback(this->handle.get(), mouse_callback);
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

private:
    static void key_callback(GLFWwindow* w, int key, int scancode, int action, int mods) {
        auto& win = *reinterpret_cast<Window*>(glfwGetWindowUserPointer(w));
        if (win.kcbk)
            win.kcbk(win, key, scancode, action, mods);
    }

    static void mouse_callback(GLFWwindow*, double, double) {
    }
};

#endif

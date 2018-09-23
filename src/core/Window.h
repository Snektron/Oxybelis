#ifndef _OXYBELIS_CORE_WINDOW_H
#define _OXYBELIS_CORE_WINDOW_H

#include <functional>
#include <GLFW/glfw3.h>
#include "math/Vec.h"
#include "utility/utility.h"

using KeyCallback = std::function<void(int, int, int, int)>;
using MouseCallback = std::function<void(double, double)>;
using MouseButtonCallback = std::function<void(int, int, int)>;
using ResizeCallback = std::function<void(int, int)>;

class Window {
    CPtr<GLFWwindow, glfwDestroyWindow> handle;

public:
    KeyCallback key_callback;
    MouseCallback mouse_callback;
    MouseButtonCallback mouse_button_callback;
    ResizeCallback resize_callback;

    Window(size_t width, size_t height, const char* title):
        handle(glfwCreateWindow(width, height, title, nullptr, nullptr)) {
        glfwSetWindowUserPointer(this->handle.get(), reinterpret_cast<void*>(this));
        glfwSetKeyCallback(this->handle.get(), glfw_key_callback);
        glfwSetCursorPosCallback(this->handle.get(), glfw_cursor_callback);
        glfwSetMouseButtonCallback(this->handle.get(), glfw_mouse_button_callback);
        glfwSetWindowSizeCallback(this->handle.get(), glfw_resize_callback);
    }

    void make_context_current() {
        glfwMakeContextCurrent(this->handle.get());
    }

    bool should_close() {
        return glfwWindowShouldClose(this->handle.get());
    }

    void swap_buffers() {
        glfwSwapBuffers(this->handle.get());
    }

    Vec2I dimensions() {
        Vec2I dim;
        glfwGetWindowSize(this->handle.get(), &dim.x, &dim.y);
        return dim;
    }

    GLFWwindow* get() {
        return this->handle.get();
    }

    const GLFWwindow* get() const {
        return this->handle.get();
    }

private:
    static void glfw_key_callback(GLFWwindow* w, int key, int scancode, int action, int mods) {
        auto& win = *reinterpret_cast<Window*>(glfwGetWindowUserPointer(w));
        if (win.key_callback)
            win.key_callback(key, scancode, action, mods);
    }

    static void glfw_cursor_callback(GLFWwindow* w, double x, double y) {
        auto& win = *reinterpret_cast<Window*>(glfwGetWindowUserPointer(w));
        if (win.mouse_callback)
            win.mouse_callback(x, y);
    }

    static void glfw_mouse_button_callback(GLFWwindow* w, int mb, int action, int mods) {
        auto& win = *reinterpret_cast<Window*>(glfwGetWindowUserPointer(w));
        if (win.mouse_button_callback)
            win.mouse_button_callback(mb, action, mods);
    }

    static void glfw_resize_callback(GLFWwindow* w, int width, int height) {
        auto& win = *reinterpret_cast<Window*>(glfwGetWindowUserPointer(w));
        if (win.resize_callback)
            win.resize_callback(width, height);
    }
};

#endif

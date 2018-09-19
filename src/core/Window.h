#ifndef _OXYBELIS_CORE_WINDOW_H
#define _OXYBELIS_CORE_WINDOW_H

#include <functional>
#include <GLFW/glfw3.h>
#include "math/Vector.h"
#include "utility/utility.h"

using KeyCallback = std::function<void(int, int, int, int)>;
using MouseCallback = std::function<void(double, double)>;
using MouseButtonCallback = std::function<void(int, int, int)>;

class Window {
    CPtr<GLFWwindow, glfwDestroyWindow> handle;
    KeyCallback key_callback;
    MouseCallback mouse_callback;
    MouseButtonCallback mouse_button_callback;

public:
    Window(size_t width, size_t height, const char* title):
        handle(glfwCreateWindow(width, height, title, nullptr, nullptr)) {
        glfwSetWindowUserPointer(this->handle.get(), reinterpret_cast<void*>(this));
        glfwSetKeyCallback(this->handle.get(), glfw_key_callback);
        glfwSetCursorPosCallback(this->handle.get(), glfw_cursor_callback);
        glfwSetMouseButtonCallback(this->handle.get(), glfw_mouse_button_callback);
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

    inline GLFWwindow* get() {
        return this->handle.get();
    }

    inline const GLFWwindow* get() const {
        return this->handle.get();
    }

    inline void connect_key(KeyCallback cbk) {
        this->key_callback = cbk;
    }

    inline void connect_mouse(MouseCallback cbk) {
        this->mouse_callback = cbk;
    }

    inline void connect_mouse_button(MouseButtonCallback cbk) {
        this->mouse_button_callback = cbk;
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
};

#endif

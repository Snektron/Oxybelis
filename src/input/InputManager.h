#ifndef _OXYBELIS_INPUT_INPUTMANAGER_H
#define _OXYBELIS_INPUT_INPUTMANAGER_H

#include <unordered_map>
#include <unordered_set>
#include <cstddef>
#include <algorithm>
#include <GLFW/glfw3.h>
#include "core/Window.h"
#include "input/InputContext.h"
#include "utility/Option.h"

enum class MouseAxis {
    Horizontal,
    Vertical
};

enum class MouseButton {
    Left = GLFW_MOUSE_BUTTON_LEFT,
    Right = GLFW_MOUSE_BUTTON_RIGHT
};

using GlfwKey = int;

template <typename T = size_t>
class InputManager {
public:
    using Context = InputContext<T>;
    using ContextRef = std::reference_wrapper<InputContext<T>>;

private:
    struct AxisInfo {
        std::unordered_map<GlfwKey, double> keys;
        std::unordered_map<MouseAxis, double> mice;
        std::unordered_map<MouseButton, double> mouse_buttons;

        double calculate_value(Window& win) {
            return std::accumulate(this->keys.begin(), this->keys.end(), 0, [&](double x, auto& key){
                    if (glfwGetKey(win.get(), key.first) == GLFW_PRESS)
                        x += key.second;
                    return x;
                })
                + std::accumulate(this->mouse_buttons.begin(), this->mouse_buttons.end(), 0, [&](double x, auto& button){
                    auto glfw_button = static_cast<int>(button.first);
                    if (glfwGetMouseButton(win.get(), glfw_button) == GLFW_PRESS)
                        x += button.second;
                    return x;
                });
        }
    };

    struct ActionInfo {
        std::unordered_set<GlfwKey> keys;
        std::unordered_set<MouseButton> mouse_buttons;
    };

    struct MappingInfo {
        ActionInfo action;
        AxisInfo axis;
    };

    Option<ContextRef> current_context;

    std::unordered_map<T, MappingInfo> mapping;
    Window& win;

public:
    InputManager(Window& win):
        win(win) {
    }

    Option<ContextRef> switch_context(Context& new_context) {
        Option<ContextRef> ctx = this->current_context;
        this->current_context = new_context;
        return ctx;
    }

    Option<ContextRef> remove_context() {
        Option<ContextRef> ctx = this->current_context;
        this->current_context = NONE;
        return ctx;
    }

    void bind_action(GlfwKey key, const T& action) {
        this->mapping[action].action.keys.insert(key);
    }

    void bind_action(MouseButton mb, const T& action) {
        this->mapping[action].action.mouse_buttons.insert(mb);
    }

    void bind_axis(GlfwKey key, const T& axis, double scale) {
        this->mapping[axis].axis.keys[key] = scale;
    }

    void bind_axis(MouseAxis mouse_axis, const T& axis, double scale) {
        this->mapping[axis].axis.mice[mouse_axis] = scale;
    }

    void bind_axis(MouseButton mb, const T& axis, double scale) {
        this->mapping[axis].axis.mouse_buttons[mb] = scale;
    }

    void update() {
        if (!this->current_context)
            return;

        for (auto i : this->mapping) {
            const auto& name = i.first;
            auto& mapping = i.second;

            double value = mapping.axis.calculate_value(this->win);
            this->current_context->get().dispatch_axis(name, value);
        }
    }
};

#endif

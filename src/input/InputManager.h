#ifndef _OXYBELIS_INPUT_INPUTMANAGER_H
#define _OXYBELIS_INPUT_INPUTMANAGER_H

#include <unordered_map>
#include <functional>
#include <experimental/optional>
#include <GLFW/glfw3.h>
#include "input/InputContext.h"
#include "utility/Option.h"

enum class MouseAxis {
    Horizontal,
    Vertical
};

using GlfwKey = int;

template <typename T = size_t>
class InputManager {
public:
    using Context = InputContext<T>;
    using ContextRef = std::reference_wrapper<InputContext<T>>;

private:
    struct Action {
        T action;
        double scale;
        double previous;
    };

    std::unordered_multimap<GlfwKey, T> key_map;
    std::unordered_multimap<MouseAxis, T> mouse_map;

    Option<ContextRef> current_context;

public:
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

    void bind(GlfwKey key, const T& action, double scale) {
        this->key_map.emplace(key, {action, scale, 0});
    }

    void bind(MouseAxis axis, const T& action, double scale) {
        this->mouse_map.emplace(axis, {action, scale, 0});
    }

    void dispatch(GlfwKey key, double value) {
        if (!this->current_context)
            return;

        auto it = this->key_map.find(key);
        auto end = this->key_map.end();
        for (; it != end; ++it) {
            double prev = it->second.previous;
            it->second.previous = value * it->second.scale;
            this->current_context->dispatch(it->second.action, it->second.previous, 0);
        }
    }

    void dispatch(MouseAxis axis, double value) {
        if (!this->current_context)
            return;

        auto it = this->mouse_map.find(axis);
        auto end = this->mouse_map.end();
        for (; it != end; ++it) {
            this->current_context->dispatch(it->second.action, value * it->second.scale, 0);
        }
    }
};

#endif

#ifndef _OXYBELIS_INPUT_INPUTCONTEXT_H
#define _OXYBELIS_INPUT_INPUTCONTEXT_H

#include <unordered_map>
#include <functional>
#include <initializer_list>
#include <cstddef>
#include <GLFW/glfw3.h>

using Action = int;
using ActionCallback = std::function<void(Action)>;

using AxisCallback = std::function<void(double)>;

template <typename T = size_t>
class InputContext {
    std::unordered_map<T, AxisCallback> axes;
    std::unordered_map<T, ActionCallback> actions;

public:
    void connect_action(const T& action, ActionCallback cbk) {
        this->actions.emplace(action, cbk);
    }

    void connect_axis(const T& axis, AxisCallback cbk) {
        this->axes.emplace(axis, cbk);
    }

    void dispatch_action(const T& action, Action value) {
        auto it = this->actions.find(action);
        auto end = this->actions.end();
        if (it != end)
            it->second(value);
    }

    void dispatch_axis(const T& axis, double value) {
        auto it = this->axes.find(axis);
        auto end = this->axes.end();
        if (it != end)
            it->second(value);
    }
};

#endif

#ifndef _OXYBELIS_INPUT_INPUTCONTEXT_H
#define _OXYBELIS_INPUT_INPUTCONTEXT_H

#include <unordered_map>
#include <functional>
#include <initializer_list>
#include <cstddef>
#include <GLFW/glfw3.h>
#include "input/Keyboard.h"

using ActionCallback = std::function<void(KeyAction)>;

using AxisCallback = std::function<void(double)>;

template <typename A>
class InputContext {
    std::unordered_map<A, AxisCallback> axes;
    std::unordered_map<A, ActionCallback> actions;

public:
    void connect_action(const A& action, ActionCallback cbk) {
        this->actions.emplace(action, cbk);
    }

    void connect_axis(const A& axis, AxisCallback cbk) {
        this->axes.emplace(axis, cbk);
    }

    void dispatch_action(const A& action, KeyAction value) {
        auto it = this->actions.find(action);
        auto end = this->actions.end();
        if (it != end)
            it->second(value);
    }

    void dispatch_axis(const A& axis, double value) {
        auto it = this->axes.find(axis);
        auto end = this->axes.end();
        if (it != end)
            it->second(value);
    }
};

#endif

#ifndef _OXYBELIS_INPUT_INPUTCONTEXT_H
#define _OXYBELIS_INPUT_INPUTCONTEXT_H

#include <unordered_map>
#include <functional>
#include <initializer_list>
#include <cstddef>
#include <GLFW/glfw3.h>
#include "input/Action.h"

using ActionCallback = std::function<void(Action)>;
using AxisCallback = std::function<void(double)>;

template <typename I>
class InputContext {
    std::unordered_map<I, ActionCallback> actions;
    std::unordered_map<I, AxisCallback> axes;

public:
    void connect_action(const I& input, ActionCallback cbk) {
        this->actions.emplace(input, cbk);
    }

    void connect_axis(const I& input, AxisCallback cbk) {
        this->axes.emplace(input, cbk);
    }

    void dispatch_action(const I& input, Action action) {
        auto it = this->actions.find(input);
        auto end = this->actions.end();
        if (it != end)
            it->second(action);
    }

    void dispatch_axis(const I& input, double value) {
        auto it = this->axes.find(input);
        auto end = this->axes.end();
        if (it != end)
            it->second(value);
    }
};

#endif

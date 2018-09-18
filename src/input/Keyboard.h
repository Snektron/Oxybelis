#ifndef _OXYBELIS_INPUT_KEYBOARD_H
#define _OXYBELIS_INPUT_KEYBOARD_H

#include <unordered_map>
#include <algorithm>
#include <GLFW/glfw3.h>
#include "input/Action.h"
#include "input/Input.h"

template <typename I>
class InputManager;

using Key = int;

template <typename I>
class Keyboard {
    struct AxisBinding {
        AxisInput<I>& axis_input;
        typename AxisInput<I>::Mapping& mapping;
    };

    InputManager<I>& manager;
    std::unordered_multimap<Key, ActionInput<I>&> actions;
    std::unordered_multimap<Key, AxisBinding> axes;

public:
    Keyboard(InputManager<I>& manager):
        manager(manager) {
    }

    inline void dispatch(Key key, Action action) {
        this->dispatch_action(key, action);
        this->dispatch_axis(key, action);
    }

    void bind_action(ActionInput<I>& action_input, Key key) {
        auto range = this->actions.equal_range(key);
        auto it = std::find_if(range.first, range.second, [=](auto& entry) {
            return &entry.second == &action_input;
        });

        if (it == range.second)
            this->actions.emplace(key, action_input);
    }

    inline void bind_action(const I& input, Key key) {
        this->bind_action(this->manager.action(input), key);
    }

    void bind_axis(AxisInput<I>& axis_input, Key key, double scale) {
        auto range = this->axes.equal_range(key);
        auto it = std::find_if(range.first, range.second, [=](auto& entry) {
            return &entry.second.axis_input == &axis_input;
        });

        if (it == range.second) {
            auto& mapping = axis_input.add_mapping(scale);
            this->axes.emplace(key, AxisBinding{axis_input, mapping});
        }
    }

    inline void bind_axis(const I& input, Key key, double scale) {
        this->bind_axis(this->manager.axis(input), key, scale);
    }

private:
    void dispatch_action(Key key, Action action) {
        auto range = this->actions.equal_range(key);

        for (auto& it = range.first; it != range.second; ++it) {
            if (it->second.update(action))
                this->manager.dispatch_action(it->second.input, action);
        }
    }

    void dispatch_axis(Key key, Action action) {
        double action_value = action == Action::Press ? 1.0 : 0.0;
        auto range = this->axes.equal_range(key);

        for (auto& it = range.first; it != range.second; ++it)
            it->second.mapping.value = action_value;
    }
};

#endif

#ifndef _OXYBELIS_INPUT_KEYBOARD_H
#define _OXYBELIS_INPUT_KEYBOARD_H

#include <unordered_map>
#include <algorithm>
#include <GLFW/glfw3.h>
#include "input/Action.h"

template <typename A>
class InputManager;

using Key = int;
using KeyAction = int;

template <typename A>
class Keyboard {
    InputManager<A>& manager;
    std::unordered_multimap<Key, ActionInfo<A>&> actions;

public:
    Keyboard(InputManager<A>& manager):
        manager(manager) {
    }

    void dispatch(Key key, KeyAction action) {
        auto range = this->actions.equal_range(key);

        for (auto& it = range.first; it != range.second; ++it) {
            bool& pressed = it->second.is_pressed;
            if ((action == GLFW_PRESS && !pressed) ||
                (action == GLFW_RELEASE && pressed)) {
                pressed = action == GLFW_PRESS;
                this->manager.dispatch_action(it->second.action, action);
            }
        }
    }

    void bind(ActionInfo<A>& action_info, Key key) {
        auto range = this->actions.equal_range(key);
        auto it = std::find_if(range.first, range.second, [=](auto& entry) {
            return &entry.second == &action_info;
        });

        if (it == range.second)
            this->actions.emplace(key, action_info);
    }

    void bind(const A& action, Key key) {
        this->bind(this->manager.action(action), key);
    }

    friend class InputManager<A>;
};

#endif

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
    InputManager<I>& manager;
    std::unordered_multimap<Key, ActionInput<I>&> actions;

public:
    Keyboard(InputManager<I>& manager):
        manager(manager) {
    }

    void dispatch_action(Key key, Action action) {
        auto range = this->actions.equal_range(key);

        for (auto& it = range.first; it != range.second; ++it) {
            bool& pressed = it->second.is_pressed;
            if ((action == Action::Press && !pressed) ||
                (action == Action::Release && pressed)) {
                pressed = action == Action::Press;
                this->manager.dispatch_action(it->second.input, action);
            }
        }
    }

    void bind_action(ActionInput<I>& action_input, Key key) {
        auto range = this->actions.equal_range(key);
        auto it = std::find_if(range.first, range.second, [=](auto& entry) {
            return &entry.second == &action_input;
        });

        if (it == range.second)
            this->actions.emplace(key, action_input);
    }

    void bind_action(const I& input, Key key) {
        this->bind_action(this->manager.action(input), key);
    }
};

#endif

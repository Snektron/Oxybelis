#ifndef _OXYBELIS_INPUT_MOUSE_H
#define _OXYBELIS_INPUT_MOUSE_H

#include <unordered_map>
#include "input/Action.h"
#include "input/Input.h"

template <typename I>
class InputManager;

enum class MouseAxis {
    Vertical,
    Horizontal
};

enum class MouseButton {
    Button1 = GLFW_MOUSE_BUTTON_1,
    Button2 = GLFW_MOUSE_BUTTON_2,
    Button3 = GLFW_MOUSE_BUTTON_3,
    Button4 = GLFW_MOUSE_BUTTON_4,
    Button5 = GLFW_MOUSE_BUTTON_5,
    Button6 = GLFW_MOUSE_BUTTON_6,
    Button7 = GLFW_MOUSE_BUTTON_7,
    Button8 = GLFW_MOUSE_BUTTON_8,
    Left = GLFW_MOUSE_BUTTON_LEFT,
    Right = GLFW_MOUSE_BUTTON_RIGHT,
    Middle = GLFW_MOUSE_BUTTON_MIDDLE
};

template <typename I>
class Mouse {
    InputManager<I>& manager;
    std::unordered_multimap<MouseButton, ActionInput<I>&> actions;

public:
    Mouse(InputManager<I>& manager):
        manager(manager) {
    }

    void dispatch_button(MouseButton mb, Action action) {
        auto range = this->actions.equal_range(mb);

        for (auto& it = range.first; it != range.second; ++it) {
            if (it->second.update(action))
                this->manager.dispatch_action(it->second.input, action);
        }
    }

    void bind_action(ActionInput<I>& action_input, MouseButton mb) {
        auto range = this->actions.equal_range(mb);
        auto it = std::find_if(range.first, range.second, [=](auto& entry) {
            return &entry.second == &action_input;
        });

        if (it == range.second)
            this->actions.emplace(mb, action_input);
    }

    inline void bind_action(const I& input, MouseButton mb) {
        this->bind_action(this->manager.action(input), mb);
    }
};

#endif

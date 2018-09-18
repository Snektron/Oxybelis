#ifndef _OXYBELIS_INPUT_DEVICE_MOUSE_H
#define _OXYBELIS_INPUT_DEVICE_MOUSE_H

#include "input/Action.h"
#include "input/Input.h"
#include "input/ActionMap.h"
#include "input/AxisMap.h"

constexpr const double MB_PRESS_VALUE = 1.0;
constexpr const double MB_RELEASE_VALUE = 0.0;

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
    ActionMap<I, MouseButton> action_map;
    AxisMap<I, MouseButton> axis_map;

public:
    Mouse(InputManager<I>& manager):
        action_map(manager), axis_map(manager) {
    }

    inline void dispatch_button(MouseButton mb, Action action) {
        this->action_map.dispatch(mb, action);
        this->axis_map.dispatch(mb, action == Action::Press ? MB_PRESS_VALUE : MB_RELEASE_VALUE);
    }

    void bind_action(ActionInput<I>& action_input, MouseButton mb) {
        this->action_map.bind(action_input, mb);
    }

    inline void bind_action(const I& input, MouseButton mb) {
        this->action_map.bind(input, mb);
    }

    void bind_axis(AxisInput<I>& axis_input, MouseButton mb, double scale) {
        this->axis_map.bind(axis_input, mb, scale);
    }

    inline void bind_axis(const I& input, MouseButton mb, double scale) {
        this->axis_map.bind(input, mb, scale);
    }
};

#endif

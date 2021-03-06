#ifndef _OXYBELIS_INPUT_DEVICE_MOUSE_H
#define _OXYBELIS_INPUT_DEVICE_MOUSE_H

#include <functional>
#include <GLFW/glfw3.h>
#include "input/Action.h"
#include "input/Input.h"
#include "input/ActionMap.h"
#include "input/AxisMap.h"
#include "core/Window.h"
#include "math/Vec.h"
#include "utility/derive_enum_hash.h"

constexpr const double MB_PRESS_VALUE = 1.0;
constexpr const double MB_RELEASE_VALUE = 0.0;

template <typename I>
class InputManager;

enum class MouseAxis {
    Vertical,
    Horizontal
};

DERIVE_ENUM_HASH(MouseAxis)

enum class MouseButton: int {
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

DERIVE_ENUM_HASH(MouseButton)

template <typename I>
class Mouse {
    ActionMap<I, MouseButton> action_map;
    AxisMap<I, MouseButton> mb_axis_map;
    AxisMap<I, MouseAxis> axis_map;
    Window& win;
    Vec2D cursor;
    Vec2D delta;

public:
    Mouse(InputManager<I>& manager, Window& win):
        action_map(manager), mb_axis_map(manager),
        axis_map(manager),
        win(win),
        cursor(0, 0), delta(0, 0) {
        glfwGetCursorPos(win.get(), &this->cursor.x, &this->cursor.y);
        
        win.mouse_callback = [this](double x, double y) {
            this->update_cursor(x, y);
        };

        win.mouse_button_callback = [this](int button, int action, int) {
            this->dispatch_button(static_cast<MouseButton>(button), static_cast<Action>(action));
        };
    }

    ~Mouse() {
        win.mouse_callback = nullptr;
        win.mouse_button_callback = nullptr;
    }

    void dispatch_button(MouseButton mb, Action action) {
        this->action_map.dispatch(mb, action);
        this->mb_axis_map.dispatch(mb, action == Action::Press ? MB_PRESS_VALUE : MB_RELEASE_VALUE);
    }

    void update_cursor(double x, double y) {
        Vec2D new_pos(x, y);
        delta = new_pos - this->cursor;
        this->cursor = new_pos;
        this->dispatch_cursor(MouseAxis::Horizontal, this->delta.x);
        this->dispatch_cursor(MouseAxis::Vertical, this->delta.y);
    }

    void dispatch_cursor(MouseAxis axis, double value) {
        this->axis_map.dispatch(axis, value);
    }

    void bind_action(ActionInput<I>& action_input, MouseButton mb) {
        this->action_map.bind(action_input, mb);
    }

    void bind_action(const I& input, MouseButton mb) {
        this->action_map.bind(input, mb);
    }

    void bind_axis(AxisInput<I>& axis_input, MouseButton mb, double scale, bool reset_after_update = false) {
        this->mb_axis_map.bind(axis_input, mb, scale, reset_after_update);
    }

    void bind_axis(const I& input, MouseButton mb, double scale, bool reset_after_update = false) {
        this->mb_axis_map.bind(input, mb, scale, reset_after_update);
    }

    void bind_axis(AxisInput<I>& axis_input, MouseAxis axis, double scale, bool reset_after_update = true) {
        this->axis_map.bind(axis_input, axis, scale, reset_after_update);
    }

    void bind_axis(const I& input, MouseAxis axis, double scale, bool reset_after_update = true) {
        this->axis_map.bind(input, axis, scale, reset_after_update);
    }

    void disable_cursor(bool disabled = true) {
        glfwSetInputMode(this->win.get(), GLFW_CURSOR, disabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
};

#endif

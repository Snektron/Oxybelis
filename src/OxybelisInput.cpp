#include "OxybelisInput.h"
#include "math/Vec.h"

namespace {
    constexpr const Vec2D MOUSE_SENSIVITY(0.008);
    constexpr const double ROLL_SENSIVITY = 0.008;
}

void initialize_input(Mouse<Input>& mouse, Keyboard<Input>& kb) {
    kb.bind_action(Input::Quit, GLFW_KEY_ESCAPE);
    kb.bind_axis(Input::Strafe, GLFW_KEY_D, 1.0);
    kb.bind_axis(Input::Strafe, GLFW_KEY_A, -1.0);
    kb.bind_axis(Input::Forward, GLFW_KEY_W, 1.0);
    kb.bind_axis(Input::Forward, GLFW_KEY_S, -1.0);
    kb.bind_axis(Input::Fly, GLFW_KEY_SPACE, 1.0);
    kb.bind_axis(Input::Fly, GLFW_KEY_LEFT_SHIFT, -1.0);
    kb.bind_axis(Input::Rotate, GLFW_KEY_Q, ROLL_SENSIVITY);
    kb.bind_axis(Input::Rotate, GLFW_KEY_E, -ROLL_SENSIVITY);
    kb.bind_action(Input::SpeedUp, GLFW_KEY_UP);
    kb.bind_action(Input::SpeedDown, GLFW_KEY_DOWN);

    mouse.bind_axis(Input::Horizontal, MouseAxis::Horizontal, MOUSE_SENSIVITY.x);
    mouse.bind_axis(Input::Vertical, MouseAxis::Vertical, MOUSE_SENSIVITY.y);
    mouse.bind_action(Input::ToggleMouse, MouseButton::Left);
}

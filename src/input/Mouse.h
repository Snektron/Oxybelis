#ifndef _OXYBELIS_INPUT_MOUSE_H
#define _OXYBELIS_INPUT_MOUSE_H

#include "math/Vector.h"

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

class Mouse {
    InputManager<A>& manager;

public:
    Mouse(InputManager<A>& manager):
        manager(manager) {
    }
};

#endif

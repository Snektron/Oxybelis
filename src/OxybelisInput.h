#ifndef _OXYBELIS_OXYBELISINPUT_H
#define _OXYBELIS_OXYBELISINPUT_H

#include <functional>
#include "input/device/Mouse.h"
#include "input/device/Keyboard.h"

enum class Input {
    Quit,
    Vertical,
    Horizontal,
    Strafe,
    Forward,
    Fly,
    Rotate,
    SpeedUp,
    SpeedDown,
    ToggleMouse,
};

namespace std {
    template <>
    struct hash<Input> {
        size_t operator()(Input i) const {
            return static_cast<size_t>(i);
        }
    };
}

void initialize_input(Mouse<Input>& mouse, Keyboard<Input>& kb);

#endif

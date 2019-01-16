#ifndef _OXYBELIS_OXYBELISINPUT_H
#define _OXYBELIS_OXYBELISINPUT_H

#include <functional>
#include "input/device/Mouse.h"
#include "input/device/Keyboard.h"
#include "utility/derive_enum_hash.h"

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

DERIVE_ENUM_HASH(Input)

void initialize_input(Mouse<Input>& mouse, Keyboard<Input>& kb);

#endif

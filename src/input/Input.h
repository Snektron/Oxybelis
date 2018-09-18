#ifndef _OXYBELIS_INPUT_INPUT_H
#define _OXYBELIS_INPUT_INPUT_H

#include "input/Action.h"

template <typename I>
struct ActionInput {
    I input;
    bool is_pressed;

    ActionInput(const I& input):
        input(input),
        is_pressed(false) {
    }

    bool update(Action action) {
        if ((action == Action::Press && !this->is_pressed) ||
            (action == Action::Release && this->is_pressed)) {
            this->is_pressed = action == Action::Press;
            return true;
        }

        return false;
    }
};

template <typename I>
struct AxisInput {
    I input;

    AxisInput(const I& input):
        input(input) {
    }
};

#endif
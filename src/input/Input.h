#ifndef _OXYBELIS_INPUT_INPUT_H
#define _OXYBELIS_INPUT_INPUT_H

template <typename I>
struct ActionInput {
    I input;
    bool is_pressed;

    ActionInput(const I& input):
        input(input),
        is_pressed(false) {
    }
};

#endif
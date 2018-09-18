#ifndef _OXYBELIS_INPUT_ACTION_H
#define _OXYBELIS_INPUT_ACTION_H

template <typename A>
struct ActionInfo {
    A action;
    bool is_pressed;

    ActionInfo(const A& action):
        action(action),
        is_pressed(false) {
    }
};

#endif

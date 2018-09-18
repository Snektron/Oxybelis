#ifndef _OXYBELIS_INPUT_DEVICE_KEYBOARD_H
#define _OXYBELIS_INPUT_DEVICE_KEYBOARD_H

#include "input/Action.h"
#include "input/Input.h"
#include "input/ActionMap.h"
#include "input/AxisMap.h"

template <typename I>
class InputManager;

using Key = int;

template <typename I>
class Keyboard {
    ActionMap<I, Key> action_map;
    AxisMap<I, Key> axis_map;

public:
    Keyboard(InputManager<I>& manager):
        action_map(manager), axis_map(manager) {
    }

    inline void dispatch(Key key, Action action) {
        this->action_map.dispatch(key, action);
        this->axis_map.dispatch(key, action);
    }

    void bind_action(ActionInput<I>& action_input, Key key) {
        this->action_map.bind(action_input, key);
    }

    inline void bind_action(const I& input, Key key) {
        this->action_map.bind(input, key);
    }

    void bind_axis(AxisInput<I>& axis_input, Key key, double scale) {
        this->axis_map.bind(axis_input, key, scale);
    }

    inline void bind_axis(const I& input, Key key, double scale) {
        this->axis_map.bind(input, key, scale);
    }
};

#endif

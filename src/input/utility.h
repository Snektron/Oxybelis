#ifndef _OXYBELIS_INPUT_UTILITY_H
#define _OXYBELIS_INPUT_UTILITY_H

#include "input/Action.h"

template <typename F>
constexpr auto filter_fn(Action filter, F f) {
    return [=](Action action) {
        if (filter == action) {
            f();
        }
    };
}

#endif

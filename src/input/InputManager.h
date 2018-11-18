#ifndef _OXYBELIS_INPUT_INPUTMANAGER_H
#define _OXYBELIS_INPUT_INPUTMANAGER_H

#include <algorithm>
#include <deque>
#include "input/InputContext.h"
#include "input/Action.h"
#include "input/Input.h"
#include "utility/Option.h"

namespace detail {
    template <typename V, typename F, typename... Args>
    auto find_or_emplace(V& v, F f, Args&&... args) -> typename V::reference& {
        auto end = v.end();
        auto it = std::find_if(v.begin(), end, f);

        if (it != end) {
            return *it;
        } else {
            v.emplace_back(std::forward<Args>(args)...);
            return v.back();
        }
    }
}

template <typename I>
class InputManager {
    using Context = InputContext<I>;
    using ContextRef = std::reference_wrapper<Context>;

    std::deque<ActionInput<I>> actions;
    std::deque<AxisInput<I>> axes;

    Option<ContextRef> context;

public:
    void switch_context(Context& ctx) {
        this->context.emplace(ctx);
    }

    void clear_context() {
        this->context.reset();
    }

    ActionInput<I>& action(const I& input) {
        return detail::find_or_emplace(this->actions, [=](auto& entry){
           return entry.input == input;
        }, input);
    }

    AxisInput<I>& axis(const I& input) {
        return detail::find_or_emplace(this->axes, [=](auto& entry){
           return entry.input == input;
        }, input);
    }

    void dispatch_action(const I& input, Action action) {
        if (this->context)
            this->context->get().dispatch_action(input, action);
    }

    void update(double dt) {
        if (!this->context)
            return;

        for (auto& axis_input : this->axes) {
            double value = axis_input.update();
            this->context->get().dispatch_axis(axis_input.input, value, dt);
        }
    }
};

#endif

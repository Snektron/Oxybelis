#ifndef _OXYBELIS_INPUT_INPUTMANAGER_H
#define _OXYBELIS_INPUT_INPUTMANAGER_H

#include <vector>
#include <algorithm>
#include <functional>
#include <GLFW/glfw3.h>
#include "input/InputContext.h"
#include "input/Mouse.h"
#include "input/Keyboard.h"
#include "input/Action.h"
#include "input/Input.h"
#include "utility/Option.h"

template <typename I>
class InputManager {
    using Context = InputContext<I>;
    using ContextRef = std::reference_wrapper<Context>;

    std::vector<ActionInput<I>> actions;
    Option<ContextRef> context;

public:
    void switch_context(Context& ctx) {
        this->context.emplace(ctx);
    }

    void clear_context() {
        this->context.reset();
    }

    ActionInput<I>& action(const I& input) {
        auto end = this->actions.end();
        auto it = std::find_if(this->actions.begin(), this->actions.end(), [=](auto& entry){
           return entry.input == input;
        });

        if (it != end) {
            return *it;
        } else {
            this->actions.emplace_back(input);
            return this->actions[this->actions.size() - 1];
        }
    }

    void dispatch_action(const I& input, Action action) {
        if (this->context)
            this->context->get().dispatch_action(input, action);
    }
};

#endif

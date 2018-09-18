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
#include "utility/Option.h"

template <typename A = size_t>
class InputManager {
    using Context = InputContext<A>;
    using ContextRef = std::reference_wrapper<Context>;

    std::vector<ActionInfo<A>> actions;
    Option<ContextRef> context;

public:
    void switch_context(Context& ctx) {
        this->context.emplace(ctx);
    }

    void clear_context() {
        this->context.reset();
    }

    ActionInfo<A>& action(const A& action) {
        auto end = this->actions.end();
        auto it = std::find_if(this->actions.begin(), this->actions.end(), [=](auto& i){
           return i.action == action;
        });

        if (it != end) {
            return *it;
        } else {
            this->actions.push_back(action);
            return this->actions[this->actions.size() - 1];
        }
    }

    void dispatch_action(const A& action, KeyAction value) {
        if (this->context)
            this->context->get().dispatch_action(action, value);
    }
};

#endif

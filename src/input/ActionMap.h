#ifndef _OXYBELIS_INPUT_ACTIONMAP_H
#define _OXYBELIS_INPUT_ACTIONMAP_H

#include <unordered_map>
#include "input/Action.h"
#include "input/Input.h"

template <typename I>
class InputManager;

template <typename I, typename T>
class ActionMap {
    InputManager<I>& manager;
    std::unordered_multimap<T, ActionInput<I>&> actions;

public:
    ActionMap(InputManager<I>& manager):
        manager(manager) {
    }

    void dispatch(T key, Action action) {
        auto range = this->actions.equal_range(key);

        for (auto& it = range.first; it != range.second; ++it) {
            if (it->second.update(action))
                this->manager.dispatch_action(it->second.input, action);
        }
    }

    void bind(ActionInput<I>& action_input, T key) {
        auto range = this->actions.equal_range(key);
        auto it = std::find_if(range.first, range.second, [=](auto& entry) {
            return &entry.second == &action_input;
        });

        if (it == range.second)
            this->actions.emplace(key, action_input);
    }

    void bind(const I& input, T key) {
        this->bind(this->manager.action(input), key);
    }
};

#endif

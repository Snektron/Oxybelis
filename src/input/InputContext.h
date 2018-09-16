#ifndef _OXYBELIS_INPUT_INPUTCONTEXT_H
#define _OXYBELIS_INPUT_INPUTCONTEXT_H

#include <unordered_map>
#include <functional>
#include <initializer_list>
#include <cstddef>
#include <GLFW/glfw3.h>

using InputCallback = std::function<void(double, double)>;

template <typename T = size_t>
class InputContext {
    std::unordered_multimap<T, InputCallback> callbacks;

public:
    void connect(const T& action, InputCallback cbk) {
        this->callbacks.emplace(action, cbk);
    }

    void dispatch(const T& action, double value, double previous) {
        auto it = this->callbacks.find(action);
        auto end = this->callbacks.end();
        for (; it != end; ++it)
            it->second(value, previous);
    }

private:
    template <typename U>
    friend class InputManager;
};

#endif

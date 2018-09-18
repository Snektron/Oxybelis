#ifndef _OXYBELIS_INPUT_AXIS_MAP_H
#define _OXYBELIS_INPUT_AXIS_MAP_H

#include <unordered_map>
#include "input/Input.h"

template <typename I>
class InputManager;

template <typename I, typename T>
class AxisMap {
    struct AxisBinding {
        AxisInput<I>& axis_input;
        typename AxisInput<I>::Mapping& mapping;
    };

    InputManager<I>& manager;
    std::unordered_multimap<T, AxisBinding> axes;

public:
    AxisMap(InputManager<I>& manager):
        manager(manager) {
    }

    void bind(AxisInput<I>& axis_input, T key, double scale) {
        auto range = this->axes.equal_range(key);
        auto it = std::find_if(range.first, range.second, [=](auto& entry) {
            return &entry.second.axis_input == &axis_input;
        });

        if (it == range.second) {
            auto& mapping = axis_input.add_mapping(scale);
            this->axes.emplace(key, AxisBinding{axis_input, mapping});
        }
    }

    inline void bind(const I& input, T key, double scale) {
        this->bind(this->manager.axis(input), key, scale);
    }

    void dispatch(T key, double value) {
        auto range = this->axes.equal_range(key);

        for (auto& it = range.first; it != range.second; ++it)
            it->second.mapping.value = value;
    }
};

#endif

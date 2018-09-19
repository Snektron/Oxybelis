#ifndef _OXYBELIS_INPUT_INPUT_H
#define _OXYBELIS_INPUT_INPUT_H

#include <vector>
#include <algorithm>
#include "input/Action.h"

template <typename I>
struct ActionInput {
    I input;
    bool is_pressed;

    ActionInput(const I& input):
        input(input),
        is_pressed(false) {
    }

    bool update(Action action) {
        if ((action == Action::Press && !this->is_pressed) ||
            (action == Action::Release && this->is_pressed)) {
            this->is_pressed = action == Action::Press;
            return true;
        }

        return false;
    }
};

template <typename I>
struct AxisInput {
    struct Mapping {
        double value;
        double scale;

        Mapping(double scale):
            value(0), scale(scale) {
        }
    };

    using MappingId = size_t;

    I input;
    std::vector<Mapping> mappings;

    AxisInput(const I& input):
        input(input) {
    }

    MappingId add_mapping(double scale) {
        this->mappings.emplace_back(scale);
        return this->mappings.size() - 1;
    }

    void update(MappingId id, double value) {
        this->mappings[id].value = value;
    }

    double value() const {
        return std::accumulate(
            this->mappings.begin(),
            this->mappings.end(),
            0.0, [](double total, auto& mapping){
                return total + mapping.value * mapping.scale;
            });
    }
};

#endif

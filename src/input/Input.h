#ifndef _OXYBELIS_INPUT_INPUT_H
#define _OXYBELIS_INPUT_INPUT_H

#include <vector>
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
        bool reset_after_update;

        Mapping(double scale, bool reset_after_update):
            value(0), scale(scale),
            reset_after_update(reset_after_update) {
        }
    };

    using MappingId = size_t;

    I input;
    std::vector<Mapping> mappings;

    AxisInput(const I& input):
        input(input) {
    }

    MappingId add_mapping(double scale, bool reset_after_update) {
        this->mappings.emplace_back(scale, reset_after_update);
        return this->mappings.size() - 1;
    }

    void update_mapping_value(MappingId id, double value) {
        this->mappings[id].value = value;
    }

    double update() {
        double value = 0;
        for (auto& mapping : this->mappings) {
            value += mapping.value * mapping.scale;
            if (mapping.reset_after_update)
                mapping.value = 0;
        }
        return value;
    }
};

#endif

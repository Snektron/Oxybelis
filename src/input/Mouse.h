#ifndef _OXYBELIS_INPUT_MOUSE_H
#define _OXYBELIS_INPUT_MOUSE_H

#include "math/Vector.h"

class Mouse {
    Vector<double, 2> mouse_pos;
    Vector<double, 2> mouse_delta;

public:
    Mouse():
        mouse_pos(0, 0),
        mouse_delta(0, 0) {
    }

    void update(double x, double y) {
        Vector<double, 2> new_pos(x, y);
        this->delta = new_pos - this->pos;
        this->mouse_pos = new_pos;
    }

    Vector<double, 2>& pos() const {
        return this->mouse_pos;
    }

    Vector<double, 2> delta() const {
        return this->mouse_delta;
    }
};

#endif

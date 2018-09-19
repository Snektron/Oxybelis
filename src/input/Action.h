#ifndef _OXYBELIS_INPUT_ACTION_H
#define _OXYBELIS_INPUT_ACTION_H

#include <GLFW/glfw3.h>

enum class Action: int {
    Press = GLFW_PRESS,
    Release = GLFW_RELEASE
};

#endif

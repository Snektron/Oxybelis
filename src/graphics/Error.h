#ifndef _OXYBELIS_GRAPHICS_ERROR_H
#define _OXYBELIS_GRAPHICS_ERROR_H

#include <stdexcept>
#include <sstream>
#include "glad/glad.h"

std::string get_gl_error_msg(GLenum error) {
    std::stringstream ss;
    ss << "OpenGL error: " << error;
    return ss.str();
}

class GlException: public std::runtime_error {
    GLenum error;

public:
    GlException(GLenum error):
        std::runtime_error(get_gl_error_msg(error)),
        error(error) {
    }

    inline GLenum error_code() const {
        return this->error;
    }
};

void assert_gl() {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
        throw GlException(err);
}

#endif

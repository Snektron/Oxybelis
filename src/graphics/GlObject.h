#ifndef _OXYBELIS_GRAPHICS_GLOBJECT_H
#define _OXYBELIS_GRAPHICS_GLOBJECT_H

#include <utility>

using DeleteFn = void(GLuint);

template <DeleteFn Delete>
class GlObject {
    GLuint handle;

public:
    GlObject(const GlObject&) = delete;
    GlObject& operator=(const GlObject&) = delete;

    explicit GlObject(GLuint handle):
        handle(handle) {
    }

    GlObject(GlObject&& other):
        handle(other.handle) {
        other.handle = 0;
    }

    GlObject& operator=(GlObject&& other) {
        std::swap(this->handle, other.handle);
    }

    ~GlObject() {
        Delete(this->handle);
    }

    operator GLuint() const {
        return this->handle;
    }
};

#endif

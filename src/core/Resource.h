#ifndef _OXYBELIS_CORE_RESOURCE_H
#define _OXYBELIS_CORE_RESOURCE_H

class Resource {
    const char* const ptr;
    const size_t sz;

public:
    constexpr Resource(const char* data, size_t size):
        ptr(data), sz(size) {
    }

    constexpr const char* data() const {
        return this->ptr;
    }

    constexpr size_t size() const {
        return this->sz;
    }
};

#endif

#ifndef _OXYBELIS_CORE_RESOURCE_H
#define _OXYBELIS_CORE_RESOURCE_H

struct Resource {
    const char* const data;
    const size_t size;

    Resource(const char* data, size_t size):
        data(data), size(size) {
    }
};

#endif

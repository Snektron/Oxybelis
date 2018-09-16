#ifndef _OXYBELIS_CORE_RESOURCE_H
#define _OXYBELIS_CORE_RESOURCE_H

#include <experimental/string_view>

class Resource {
    std::experimental::string_view resource;

public:
    constexpr Resource(const char* data, size_t size):
        resource(data, size) {
    }

    const char* data() const {
        return resource.data();
    }

    size_t size() const {
        return this->resource.size();
    }

    const std::experimental::string_view& view() const {
        return this->resource;
    }
};

#endif

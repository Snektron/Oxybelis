#ifndef _OXYBELIS_UTILITY_CSTRING_H
#define _OXYBELIS_UTILITY_CSTRING_H

#include <string>

class CStringView {
    const char* str;

public:
    constexpr CStringView(const std::string& str):
        str(str.c_str()) {
    }

    constexpr CStringView(const char* str):
        str(str) {
    }

    const char* data() {
        return this->str;
    }
};

#endif

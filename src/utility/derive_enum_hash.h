#ifndef _OXYBELIS_UTILITY_DERIVE_ENUM_HASH_H
#define _OXYBELIS_UTILITY_DERIVE_ENUM_HASH_H

#include <cstddef>

#define DERIVE_ENUM_HASH(enum_name) \
    namespace std { \
        template <> \
        struct hash<enum_name> { \
            size_t operator()(enum_name e) const { \
                return static_cast<size_t>(e); \
            } \
        }; \
    }

#endif

#ifndef _OXYBELIS_UTILITY_OPTION_H
#define _OXYBELIS_UTILITY_OPTION_H

#include <experimental/optional>

template <typename T>
using Option = std::experimental::optional<T>;

constexpr auto NONE = std::experimental::nullopt;

#endif

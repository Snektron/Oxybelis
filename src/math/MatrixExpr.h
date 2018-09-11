#ifndef _OXYBELIS_MATH_MATRIXEXPR_H
#define _OXYBELIS_MATH_MATRIXEXPR_H

#include <utility>
#include <type_traits>

template <typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename L, typename R>
class Sum {
    const L& left;
    const R& right;

    static_assert(L::Rows == R::Rows && L::Cols == R::Cols, "Matrix addition must have same dimensions");

public:
    using Result = remove_cvref_t<decltype(std::declval<typename L::Type>() + std::declval<typename R::Type>())>;

    Sum(const L& left, const R& right):
        left(left),
        right(right)
    {}

    Result operator()(size_t m, size_t n) const {
        return this->left(m, n) + this->right(m, n);
    }
};

#endif

#ifndef _OXYBELIS_MATH_STORAGE_H
#define _OXYBELIS_MATH_STORAGE_H

#include <array>
#include <utility>

template <typename T, size_t R, size_t C>
struct ColumnMajor {
    std::array<T, R * C> elements;

    T& operator()(size_t m, size_t n) {
        return this->elements[n * C + m];
    }

    const T& operator()(size_t m, size_t n) const {
        return this->elements[n * C + m];
    }

    T& operator[](size_t i) {
        return this->elements[i];
    }

    const T& operator[](size_t i) const {
        return this->elements[i];
    }
};

template <typename T, size_t R, size_t C>
struct RowMajor {
    std::array<T, R * C> elements;

    T& operator()(size_t m, size_t n) {
        return this->elements[m * R + n];
    }

    const T& operator()(size_t m, size_t n) const {
        return this->elements[m * R + n];
    }

    T& operator[](size_t i) {
        return this->elements[i];
    }

    const T& operator[](size_t i) const {
        return this->elements[i];
    }
};

#endif

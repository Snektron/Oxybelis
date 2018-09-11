#ifndef _OXYBELIS_MATH_STORAGE_H
#define _OXYBELIS_MATH_STORAGE_H

#include <array>
#include <utility>

template <typename T, size_t R, size_t C>
class ColumnMajor {
    std::array<T, R * C> elements;

public:
    ColumnMajor() = default;

    ColumnMajor(const std::array<T, R * C>& elements):
        elements(elements) {
    }

    ColumnMajor(std::array<T, R * C>&& elements):
        elements(std::forward<std::array<T, R * C>>(elements)) {
    }

    template <typename U>
    ColumnMajor(const U& other) {
        for (size_t i = 0; i < C; ++i)
            for (size_t j = 0; j < R; ++j)
                (*this)(j, i) = other(j, i);
    }

    template <typename U>
    ColumnMajor(U&& other) {
        for (size_t i = 0; i < C; ++i)
            for (size_t j = 0; j < R; ++j)
                (*this)(j, i) = std::move(other(j, i));
    }

    T& operator()(size_t m, size_t n) {
        return this->elements[n * C + m];
    }

    const T& operator()(size_t m, size_t n) const {
        return this->elements[n * C + m];
    }
};

template <typename T, size_t R, size_t C>
class RowMajor {
    std::array<T, R * C> elements;

public:
    RowMajor() = default;

    RowMajor(const std::array<T, R * C>& elements):
        elements(elements) {
    }

    RowMajor(std::array<T, R * C>&& elements):
        elements(std::forward<std::array<T, R * C>>(elements)) {
    }

    template <typename U>
    RowMajor(const U& other) {
        for (size_t i = 0; i < R; ++i)
            for (size_t j = 0; j < C; ++j)
                (*this)(i, j) = other(i, j);
    }

    template <typename U>
    RowMajor(U&& other) {
        for (size_t i = 0; i < R; ++i)
            for (size_t j = 0; j < C; ++j)
                (*this)(i, j) = std::move(other(i, j));
    }

    T& operator()(size_t m, size_t n) {
        return this->elements[m * R + n];
    }

    const T& operator()(size_t m, size_t n) const {
        return this->elements[m * R + n];
    }
};

#endif

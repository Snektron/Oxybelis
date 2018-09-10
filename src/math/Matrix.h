#ifndef _OXYBELIS_MATH_MATRIX_H
#define _OXYBELIS_MATH_MATRIX_H

#include <cstddef>
#include <algorithm>

enum StorageType {
    ColumnMajor,
    RowMajor
};

template <StorageType S, size_t M, size_t N>
struct Index;

template <size_t M, size_t N>
struct Index<ColumnMajor, M, N> {
    constexpr static size_t index(size_t m, size_t n) {
        return n * M + m;
    }
};

template <size_t M, size_t N>
struct Index<RowMajor, M, N> {
    constexpr static size_t index(size_t m, size_t n) {
        return m * N + n;
    }
};

template <typename T, size_t M, size_t N, StorageType S>
class Storage {
    constexpr const static size_t Size = M * N;
    T elements[Size];

public:
    Storage() = default;

    Storage(const T(&init)[Size]) {
        std::copy(init, init + Size, this->elements);
    }

    Storage(T(&&init)[Size]) {
        std::move(init, init + Size, this->elements);
    }

    constexpr static StorageType storage_type() {
        return S;
    }

    T& get(size_t m, size_t n) {
        return this->elements[Index<S, M, N>::index(m, n)];
    }

    const T& get(size_t m, size_t n) const {
        return this->elements[Index<S, M, N>::index(m, n)];
    }

    T& operator[](size_t i) {
        return this->elements[i];
    }

    const T& operator[](size_t i) const {
        return this->elements[i];
    }

    T* data() {
        return this->elements;
    }

    const T* data() const {
        return this->elements;
    }
};

template <typename T, size_t M, size_t N, StorageType S, typename Derived>
class MatrixBase: public Storage<T, M, N, S> {
    static_assert(M >= 1 && N >= 1, "Cannot create a matrix of 0 size");
public:
    using Type = T;

    constexpr const static size_t Height = M;
    constexpr const static size_t Width = N;

    MatrixBase() = default;

    MatrixBase(const T(&init)[M * N]):
        Storage<T, M, N, S>(init)
    {}

    MatrixBase(T(&&init)[M * N]):
        Storage<T, M, N, S>(std::forward(init))
    {}
};

template <typename T, size_t M, size_t N, StorageType S>
class Matrix: public MatrixBase<T, M, N, S, Matrix<T, M, N, S>> {
};

template <typename T, StorageType S = ColumnMajor>
using Matrix44 = Matrix<T, 4, 4, S>;

using Matrix44f = Matrix44<float>;
using Matrix44d = Matrix44<double>;

#endif

#ifndef _OXYBELIS_MATH_SIMD_H
#define _OXYBELIS_MATH_SIMD_H

template <typename T, size_t N>
struct SimdVecHelper {
    constexpr const static bool valid = false;
};

#define DEFINE_SIMD_VEC(type, size) \
    template <> \
    struct SimdVecHelper<type, (size)> { \
        constexpr const static bool valid = true; \
        using Type [[gnu::vector_size((size) * sizeof(type))]] = type; \
    }

#define DEFINE_SIMD_VEC_COMMON_SIZES(type) \
    DEFINE_SIMD_VEC(type, 4); \
    DEFINE_SIMD_VEC(type, 8)

DEFINE_SIMD_VEC_COMMON_SIZES(float);
DEFINE_SIMD_VEC_COMMON_SIZES(double);
DEFINE_SIMD_VEC_COMMON_SIZES(int);

template <typename T, size_t N>
using SimdVec = typename SimdVecHelper<T, N>::Type;

#if defined(__clang__)
    template <typename T, typename U, size_t N>
    SimdVec<T, N> shuffle(const SimdVec<T, N>& v, const SimdVec<U, N>& mask) {
        static_assert(std::is_integral<U>::value, "Shuffle mask should be intergral vector");
        return __builtin_shufflevector(v, mask);
    }

    template <typename T, typename U, size_t N>
    SimdVec<T, N> shuffle(const SimdVec<T, N>& v, const SimdVec<T, N>& w, const SimdVec<U, N>& mask) {
        static_assert(std::is_integral<U>::value, "Shuffle mask should be intergral vector");
        return __builtin_shufflevector(v, w, mask);
    }
#elif defined(__GNUG__)
    template <typename T, typename U, size_t N>
    SimdVec<T, N> shuffle(const SimdVec<T, N>& v, const SimdVec<U, N>& mask) {
        static_assert(std::is_integral<U>::value, "Shuffle mask should be intergral vector");
        return __builtin_shuffle(v, mask);
    }

    template <typename T, typename U, size_t N>
    SimdVec<T, N> shuffle(const SimdVec<T, N>& v, const SimdVec<T, N>& w, const SimdVec<U, N>& mask) {
        static_assert(std::is_integral<U>::value, "Shuffle mask should be intergral vector");
        return __builtin_shuffle(v, w, mask);
    }
#else

using Float4 = SimdVec<float, 4>;
using Float8 = SimdVec<float, 8>;

using Double4 = SimdVec<double, 4>;
using Double8 = SimdVec<double, 8>;

using Int4 = SimdVec<int, 4>;
using Int8 = SimdVec<int, 8>;

#endif

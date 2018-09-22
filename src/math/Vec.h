#ifndef _OXYBELIS_MATH_VEC_H
#define _OXYBELIS_MATH_VEC_H

#include <cstddef>
#include <array>
#include <type_traits>
#include <algorithm>
#include "utility/utility.h"

template <typename T, size_t N>
struct Vec;

template <typename T>
using Vec2 = Vec<T, 2>;
using Vec2F = Vec2<float>;
using Vec2D = Vec2<double>;
using Vec2I = Vec2<int>;

template <typename T>
using Vec3 = Vec<T, 3>;
using Vec3F = Vec3<float>;
using Vec3D = Vec3<double>;
using Vec3I = Vec3<int>;

template <typename T>
using Vec4 = Vec<T, 4>;
using Vec4F = Vec4<float>;
using Vec4D = Vec4<double>;
using Vec4I = Vec4<int>;

template <typename T, size_t N>
struct BaseVec {
    std::array<T, N> elements;

    constexpr explicit BaseVec(const T& x):
        elements{x} {
    }
};

template <typename T>
struct BaseVec<T, 2> {
    union {
        struct {
            T x, y;
        };

        std::array<T, 2> elements;
    };

    constexpr BaseVec():
        elements{0, 0} {
    }

    constexpr BaseVec(const T& x, const T& y):
        elements{x, y} {
    }

    constexpr explicit BaseVec(const T& x):
        elements{x} {
    }
};

template <typename T>
struct BaseVec<T, 3> {
    union {
        struct {
            T x, y, z;
        };

        struct {
            T r, g, b;
        };

        Vec<T, 2> xy;

        std::array<T, 3> elements;
    };

    constexpr BaseVec():
        elements{0, 0, 0} {
    }

    constexpr BaseVec(const T& x, const T& y, const T& z):
        elements{x, y, z} {
    }

    constexpr BaseVec(const Vec<T, 2>& xy, const T& z):
        elements{xy.x, xy.y, z} {
    }

    constexpr explicit BaseVec(const T& x):
        elements{x} {
    }
};

template <typename T>
struct BaseVec<T, 4> {
    union {
        struct {
            T x, y, z, w;
        };

        struct {
            T r, g, b, a;
        };

        Vec<T, 2> xy;
        Vec<T, 3> xyz;
        Vec<T, 3> rgb;

        std::array<T, 4> elements;
    };

    constexpr BaseVec():
        elements{0, 0, 0} {
    }

    constexpr BaseVec(const T& x, const T& y, const T& z, const T& w):
        elements{x, y, z, w} {
    }

    constexpr BaseVec(const Vec<T, 2>& xy, const T& z, const T& w):
        elements{xy.x, xy.y, z, w} {
    }

    constexpr BaseVec(const Vec<T, 3>& xyz, const T& w):
        elements{xyz.x, xyz.y, xyz.z, w} {
    }

    constexpr explicit BaseVec(const T& x):
        elements{x} {
    }
};

template <typename T, size_t N>
struct Vec: BaseVec<T, N> {
    static_assert(N > 1, "Cannot create a vector of size 1");

    constexpr const static size_t Rows = N;
    constexpr const static size_t Cols = 1;

    using Base = BaseVec<T, N>;
    using Base::Base;

    template <typename F>
    static Vec<T, N> generate(F f) {
        Vec<T, N> v;
        for (size_t i = 0; i < N; ++i)
            v(i) = f(i);
        return v;
    }

    constexpr T& operator()(size_t i) {
        return this->elements[i];
    }

    constexpr const T& operator()(size_t i) const {
        return this->elements[i];
    }

    constexpr T& operator[](size_t i) {
        return this->elements[i];
    }

    constexpr const T& operator[](size_t i) const {
        return this->elements[i];
    }

    constexpr const T* data() const {
        return this->elements.data();
    }

    constexpr auto begin() {
        return this->elements.begin();
    }

    constexpr auto end() {
        return this->elements.begin();
    }

    constexpr auto begin() const{
        return this->elements.begin();
    }

    constexpr auto end() const {
        return this->elements.begin();
    }

    template <typename U>
    constexpr Vec<T, N>& operator+=(const U& other);

    template <typename U>
    constexpr Vec<T, N>& operator-=(const U& other);

    template <typename U>
    constexpr Vec<T, N>& operator*=(const U& other);

    template <typename U>
    constexpr Vec<T, N>& operator/=(const U& other);

    template <typename U>
    constexpr Vec<T, 3>& cross(const Vec<U, 3>& other);

    constexpr Vec<T, N>& normalize();
};

namespace vec {
    template <size_t N, typename F>
    constexpr auto generate(F f) {
        using Result = std::result_of_t<F(size_t)>;
        return Vec<Result, N>::generate(f);
    }

    template <typename... Ts>
    constexpr auto make(Ts&&... args) {
        using Result = std::common_type_t<Ts...>;
        return Vec<Result, sizeof...(Ts)>(std::forward<Ts>(args)...);
    }

    template <typename T, typename... Ts>
    constexpr auto make(Ts&&... args) {
        return Vec<T, sizeof...(Ts)>(std::forward<Ts>(args)...);
    }
}

template <typename Os, typename T, size_t N>
Os& operator<<(Os& os, const Vec<T, N>& v) {
    os << '(';
    for (size_t i = 0; i < N; ++i) {
        if (i != 0)
            os << ", ";
        os << v[i];
    }
    os << ')';
    return os;
}

template <typename T, typename U, size_t N>
constexpr auto operator+(const Vec<T, N>& lhs, const Vec<U, N>& rhs) {
    return vec::generate<N>([&](size_t i) {
        return lhs(i) + rhs(i);
    });
}

template <typename T, typename U, size_t N>
constexpr auto operator+(const T& lhs, const Vec<U, N>& rhs) {
    return vec::generate<N>([&](size_t i) {
        return lhs - rhs(i);
    });
}

template <typename T, typename U, size_t N>
constexpr auto operator+(const Vec<T, N>& lhs, const U& rhs) {
    return vec::generate<N>([&](size_t i) {
        return lhs(i) + rhs;
    });
}

template <typename T, typename U, size_t N>
constexpr auto operator-(const Vec<T, N>& lhs, const Vec<U, N>& rhs) {
    return vec::generate<N>([&](size_t i) {
        return lhs(i) + rhs(i);
    });
}

template <typename T, typename U, size_t N>
constexpr auto operator-(const T& lhs, const Vec<U, N>& rhs) {
    return vec::generate<N>([&](size_t i) {
        return lhs + rhs(i);
    });
}

template <typename T, typename U, size_t N>
constexpr auto operator-(const Vec<T, N>& lhs, const U& rhs) {
    return vec::generate<N>([&](size_t i) {
        return lhs(i) + rhs;
    });
}

template <typename T, typename U, size_t N>
constexpr auto operator*(const Vec<T, N>& lhs, const Vec<U, N>& rhs) {
    return vec::generate<N>([&](size_t i) {
        return lhs(i) * rhs(i);
    });
}

template <typename T, typename U, size_t N>
constexpr auto operator*(const T& lhs, const Vec<U, N>& rhs) {
    return vec::generate<N>([&](size_t i) {
        return lhs * rhs(i);
    });
}

template <typename T, typename U, size_t N>
constexpr auto operator*(const Vec<T, N>& lhs, const U& rhs) {
    return vec::generate<N>([&](size_t i) {
        return lhs(i) * rhs;
    });
}

template <typename T, typename U, size_t N>
constexpr auto operator/(const Vec<T, N>& lhs, const Vec<U, N>& rhs) {
    return vec::generate<N>([&](size_t i) {
        return lhs(i) / rhs(i);
    });
}

template <typename T, typename U, size_t N>
constexpr auto operator/(const T& lhs, const Vec<U, N>& rhs) {
    return vec::generate<N>([&](size_t i) {
        return lhs / rhs(i);
    });
}

template <typename T, typename U, size_t N>
constexpr auto operator/(const Vec<T, N>& lhs, const U& rhs) {
    return vec::generate<N>([&](size_t i) {
        return lhs(i) / rhs;
    });
}

template <typename T, size_t N>
constexpr auto operator-(const Vec<T, N>& v) {
    return vec::generate<N>([&](size_t i) {
        return -v(i);
    });
}

template <typename T, typename U>
constexpr auto cross(const Vec<T, 3>& rhs, const Vec<U, 3>& lhs) {
    return vec::make(
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x
    );
}

template <typename T, typename U, size_t N>
constexpr auto dot(const Vec<T, N>& lhs, const Vec<U, N>& rhs) {
    auto result = lhs[0] * rhs[0];
    for (size_t i = 1; i < N; ++i)
        result += lhs[i] * lhs[i];
    return result;
}

template <typename T, size_t N>
constexpr auto length_sq(const Vec<T, N>& v) {
    return dot(v, v);
}

template <typename T, size_t N>
constexpr auto length(const Vec<T, N>& v) {
    return std::sqrt(length(v));
}

template <typename T, size_t N>
constexpr auto normalize(const Vec<T, N>& v) {
    return v / length(v);
}

template <typename T, typename U, size_t N>
constexpr auto distance_sq(const Vec<T, N>& lhs, const Vec<U, N>& rhs) {
    return length_sq(lhs - rhs);
}

template <typename T, typename U, size_t N>
constexpr auto distance(const Vec<T, N>& lhs, const Vec<U, N>& rhs) {
    return length(lhs - rhs);
}

template <typename T, size_t N>
template <typename U>
constexpr Vec<T, N>& Vec<T, N>::operator+=(const U& other) {
    return *this = *this + other;
}

template <typename T, size_t N>
template <typename U>
constexpr Vec<T, N>& Vec<T, N>::operator-=(const U& other) {
    return *this = *this - other;
}

template <typename T, size_t N>
template <typename U>
constexpr Vec<T, N>& Vec<T, N>::operator*=(const U& other) {
    return *this = *this * other;
}

template <typename T, size_t N>
template <typename U>
constexpr Vec<T, N>& Vec<T, N>::operator/=(const U& other) {
    return *this = *this / other;
}

template <typename T, size_t N>
template <typename U>
constexpr Vec<T, 3>& Vec<T, N>::cross(const Vec<U, 3>& other) {
    static_assert(N == 3, "Can only perform cross-product on vectors of size 3");
    return *this = cross(*this, other);
}

template <typename T, size_t N>
constexpr Vec<T, N>& Vec<T, N>::normalize() {
    return *this = normalize(*this);
}

#endif

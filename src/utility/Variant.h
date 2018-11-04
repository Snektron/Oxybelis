#ifndef _OXYBELIS_UTILITY_VARIANT_H
#define _OXYBELIS_UTILITY_VARIANT_H

#include <iostream>
#include <type_traits>
#include <utility>
#include <ostream>
#include <stdexcept>
#include <cstdint>
#include <climits>
#include <tuple>
#include "utility/utility.h"

class BadVariantAccess: public std::runtime_error {
public:
    BadVariantAccess():
        runtime_error("Bad Variant Access") {
    }
};

template <typename... Ts>
class Variant;

namespace detail {
    using VariantId = uint8_t;

    template <typename V, size_t N>
    struct VariantElement;

    template <typename... Ts, size_t N>
    struct VariantElement<Variant<Ts...>, N> {
        using Type = std::tuple_element_t<N - 1, std::tuple<Ts...>>;
    };

    template <typename V, typename T>
    struct VariantIdOf;

    template <typename H, typename... Ts, typename T>
    struct VariantIdOf<Variant<H, Ts...>, T> {
        constexpr const static VariantId id = VariantIdOf<Variant<Ts...>, T>::id + 1;
    };

    template <typename... Ts, typename T>
    struct VariantIdOf<Variant<T, Ts...>, T> {
        constexpr const static VariantId id = 1;
    };

    template <typename T>
    void destroy(T* ptr) {
        ptr->~T();
    }

    template <typename... Ts>
    using VariantData = std::aligned_union_t<0, Ts...>;

    template <VariantId Current, typename... Ts>
    struct VariantInvoke {
        using Type = typename VariantElement<Variant<Ts...>, Current>::Type;
        using Data = VariantData<Ts...>;

        template <typename F>
        constexpr static void invoke(F&& f, Data& data, VariantId target) {
            if (Current == target)
                f(*reinterpret_cast<Type*>(&data));
            else
                VariantInvoke<Current + 1, Ts...>::invoke(std::forward<F>(f), data, target);
        }

        template <typename F>
        constexpr static void invoke(F&& f, const Data& data, VariantId target) {
            if (Current == target)
                f(*reinterpret_cast<const Type*>(&data));
            else
                VariantInvoke<Current + 1, Ts...>::invoke(std::forward<F>(f), data, target);
        }
    };

    template <typename... Ts>
    struct VariantInvoke<sizeof...(Ts) + 1, Ts...> {
        using Data = VariantData<Ts...>;

        template <typename F>
        constexpr static void invoke(F&&, const Data&, VariantId) {
            throw BadVariantAccess();
        }
    };
}

template <typename V, size_t N>
using VariantElement = typename detail::VariantElement<V, N>::Type;

template <typename... Ts>
class Variant {
    static_assert(sizeof...(Ts) > 0, "At least one Variant type is required");
    static_assert((1 << (sizeof(detail::VariantId) * CHAR_BIT)) >= sizeof...(Ts) + 1, "Variant Id type not big enough");

    constexpr const static detail::VariantId DISENGAGED = 0;

    detail::VariantData<Ts...> data;
    detail::VariantId id;

public:
    constexpr Variant():
        id(DISENGAGED) {
    }

    constexpr Variant(const Variant& other):
        id(DISENGAGED) {
        this->invoke([&](auto& item) {
            using T = std::remove_reference_t<decltype(item)>;
            // &item is uninitialized at this point
            new (&item) T(other.get_unchecked<T>());
        }, other.id);
        this->id = other.id;
    }

    constexpr Variant(Variant&& other):
        id(DISENGAGED) {
        this->invoke([&](auto& item) {
            using T = std::remove_reference_t<decltype(item)>;
            // &item is uninitialized at this point
            new (&item) T(std::move(other.get_unchecked<T>()));
        }, other.id);
        this->id = other.id;
    }

    ~Variant() {
        this->clear();
    }

    constexpr Variant& operator=(const Variant& other) {
        this->clear();
        this->invoke([&](auto& item) {
            using T = std::remove_reference_t<decltype(item)>;
            // &item is uninitialized at this point
            new (&item) T(other.get_unchecked<T>());
        }, other.id);
        this->id = other.id;

        return *this;
    }

    constexpr Variant& operator=(Variant&& other) {
        this->clear();
        this->invoke([&](auto& item) {
            using T = std::remove_reference_t<decltype(item)>;
            // &item is uninitialized at this point
            new (&item) T(std::move(other.get_unchecked<T>()));
        }, other.id);
        this->id = other.id;

        return *this;
    }

    template <typename T>
    constexpr Variant& operator=(T&& item) {
        this->set(std::forward<T>(item));
        return *this;
    }

    constexpr bool has_value() const {
        return this->id != DISENGAGED;
    }

    template <typename T>
    constexpr bool has_type() const {
        return this->id == this->id_of<T>();
    }

    constexpr void clear() {
        this->invoke([this](auto& item) {
            detail::destroy(&item);
            this->id = DISENGAGED;
        });
    }

    template <typename T>
    constexpr T& get() {
        if (!this->has_type<T>())
            throw BadVariantAccess();
        return this->get_unchecked<T>();
    }

    template <typename T>
    constexpr const T& get() const {
        if (!this->has_type<T>())
            throw BadVariantAccess();
        return this->get_unchecked<T>();
    }

    template <typename T>
    constexpr T& set(T&& item) {
        this->clear();
        new (&this->data) T(std::forward<T>(item));
        this->set_id<T>();
        return this->get_unchecked<T>();
    }

    template <typename T, typename... Args>
    constexpr T& emplace(Args&&... args) {
        this->clear();
        new (&this->data) T(std::forward<Args>(args)...);
        this->set_id<T>();
        return this->get_unchecked<T>();
    }

    template <typename F>
    constexpr bool invoke(F&& f) {
        return this->invoke(std::forward<F>(f), this->id);
    }

    template <typename F>
    constexpr bool invoke(F&& f) const {
        return this->invoke(std::forward<F>(f), this->id);
    }

    template <typename T>
    constexpr T& get_unchecked() {
        return *reinterpret_cast<T*>(&this->data);
    }

    template <typename T>
    constexpr const T& get_unchecked() const {
        return *reinterpret_cast<const T*>(&this->data);
    }

private:
    template <typename F>
    constexpr bool invoke(F&& f, detail::VariantId target) {
        if (!this->has_value())
            return false;

        detail::VariantInvoke<1, Ts...>::invoke(std::forward<F>(f), data, target);
        return true;
    }

    template <typename F>
    constexpr bool invoke(F&& f, detail::VariantId target) const {
        if (!this->has_value())
            return false;

        detail::VariantInvoke<1, Ts...>::invoke(std::forward<F>(f), data, target);
        return true;
    }

    template <typename T>
    constexpr detail::VariantId id_of() const {
        return detail::VariantIdOf<Variant, T>::id;
    }

    template <typename T>
    constexpr void set_id() {
        this->id = this->id_of<T>();
    }
};

template <typename H, typename... Ts>
std::ostream& operator<<(std::ostream& os, const Variant<H, Ts...>& var) {
    os << "Variant(";
    bool printed = var.invoke([&](const auto& item){
        os << item;
    });

    if (!printed)
        os << "None)";
    else
        os << ')';

    return os;
}

#endif

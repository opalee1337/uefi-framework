#pragma once

#include <cstdint>
#include "fw/core/stl/utility/utility.h"

namespace stl {

    struct nullopt_t { explicit constexpr nullopt_t() = default; };
    inline constexpr nullopt_t nullopt{};

    template<typename T>
    class optional {
    public:
        using value_type = T;

        constexpr optional() : has_{ false } {}
        constexpr optional(nullopt_t) : has_{ false } {}

        constexpr optional(const T& val) : has_{ true } { construct(val); }
        constexpr optional(T&& val) : has_{ true } { construct(stl::move(val)); }

        optional(const optional& other) : has_{ other.has_ } {
            if (has_) construct(*other);
        }

        optional(optional&& other) : has_{ other.has_ } {
            if (has_) { construct(stl::move(*other)); other.reset(); }
        }

        ~optional() { reset(); }

        optional& operator=(nullopt_t) { reset(); return *this; }

        optional& operator=(const optional& other) {
            if (this != &other) {
                reset();
                if (other.has_) { construct(*other); has_ = true; }
            }
            return *this;
        }

        optional& operator=(optional&& other) {
            if (this != &other) {
                reset();
                if (other.has_) { construct(stl::move(*other)); has_ = true; other.reset(); }
            }
            return *this;
        }

        template<typename U>
        optional& operator=(U&& val) {
            reset();
            construct(stl::forward<U>(val));
            has_ = true;
            return *this;
        }

        bool has_value() const { return has_; }
        explicit operator bool() const { return has_; }

        T& value() { return *reinterpret_cast<T*>(&storage_); }
        const T& value() const { return *reinterpret_cast<const T*>(&storage_); }

        T& operator*() { return value(); }
        const T& operator*() const { return value(); }
        T* operator->() { return &value(); }
        const T* operator->() const { return &value(); }

        template<typename U>
        T value_or(U&& fallback) const {
            return has_ ? value() : static_cast<T>(stl::forward<U>(fallback));
        }

        void reset() {
            if (has_) { value().~T(); has_ = false; }
        }

        template<typename... Args>
        T& emplace(Args&&... args) {
            reset();
            new(&storage_) T{ stl::forward<Args>(args)... };
            has_ = true;
            return value();
        }

    private:
        template<typename U>
        void construct(U&& val) {
            new(&storage_) T{ stl::forward<U>(val) };
        }

        alignas(T) unsigned char storage_[sizeof(T)]{};
        bool has_{ false };
    };

}
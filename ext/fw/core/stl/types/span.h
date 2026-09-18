#pragma once

#include <cstdint>

namespace stl {

    template<typename T>
    class span {
    public:
        using element_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator = T*;

        constexpr span() : data_{ nullptr }, size_{ 0 } {}
        constexpr span(pointer ptr, std::uintptr_t size) : data_{ ptr }, size_{ size } {}
        constexpr span(pointer first, pointer last) : data_{ first }, size_{ static_cast<std::uintptr_t>(last - first) } {}

        template<std::uintptr_t N>
        constexpr span(T(&arr)[N]) : data_{ arr }, size_{ N } {}

        constexpr pointer        data()  const { return data_; }
        constexpr std::uintptr_t size()  const { return size_; }
        constexpr bool           empty() const { return size_ == 0; }

        constexpr reference operator[](std::uintptr_t i) const { return data_[i]; }

        constexpr reference front() const { return data_[0]; }
        constexpr reference back()  const { return data_[size_ - 1]; }

        constexpr iterator begin() const { return data_; }
        constexpr iterator end()   const { return data_ + size_; }

        constexpr span subspan(std::uintptr_t offset, std::uintptr_t count = ~std::uintptr_t(0)) const {
            if (offset >= size_) return {};
            count = count > size_ - offset ? size_ - offset : count;
            return { data_ + offset, count };
        }

        constexpr span first(std::uintptr_t count) const { return { data_, count }; }
        constexpr span last(std::uintptr_t count)  const { return { data_ + size_ - count, count }; }

        constexpr std::uintptr_t size_bytes() const { return size_ * sizeof(T); }

    private:
        pointer data_;
        std::uintptr_t size_;
    };

}
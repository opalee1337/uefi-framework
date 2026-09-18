#pragma once

#include <cstddef>
#include <cstdint>

#include "fw/core/crt/string.h"
#include "fw/core/crt/memory/memory.h"

#include "fw/core/stl/utility/utility.h"

namespace stl {

    template<typename CharT>
    struct char_traits;

    template<>
    struct char_traits<char> {
        using char_type = char;
        using int_type = int;

        static constexpr void assign(char_type& c1, const char_type& c2) { c1 = c2; }
        static constexpr bool eq(char_type c1, char_type c2) { return c1 == c2; }
        static constexpr bool lt(char_type c1, char_type c2) { return c1 < c2; }

        static int compare(const char_type* s1, const char_type* s2, std::size_t n) {
            return crt::strncmp(s1, s2, n);
        }

        static constexpr std::size_t length(const char_type* s) {
            return crt::strlen(s);
        }

        static const char_type* find(const char_type* s, std::size_t n, const char_type& a) {
            return static_cast<const char_type*>(crt::memchr(s, static_cast<std::uint8_t>(a), n));
        }

        static char_type* move(char_type* s1, const char_type* s2, std::size_t n) {
            return static_cast<char_type*>(crt::memmove(s1, s2, n));
        }

        static char_type* copy(char_type* s1, const char_type* s2, std::size_t n) {
            return static_cast<char_type*>(crt::memcpy(s1, s2, n));
        }

        static char_type* assign(char_type* s, std::size_t n, char_type a) {
            return static_cast<char_type*>(crt::memset(s, static_cast<std::uint8_t>(a), n));
        }
    };

    template<>
    struct char_traits<char16_t> {
        using char_type = char16_t;
        using int_type = int;

        static constexpr void assign(char_type& c1, const char_type& c2) { c1 = c2; }
        static constexpr bool eq(char_type c1, char_type c2) { return c1 == c2; }
        static constexpr bool lt(char_type c1, char_type c2) { return c1 < c2; }

        static int compare(const char_type* s1, const char_type* s2, std::size_t n) {
            return crt::wcsncmp(s1, s2, n);
        }

        static constexpr std::size_t length(const char_type* s) {
            return crt::wcslen(s);
        }

        static const char_type* find(const char_type* s, std::size_t n, const char_type& a) {
            for (std::size_t i = 0; i < n; ++i)
                if (eq(s[i], a)) return s + i;
            return nullptr;
        }

        static char_type* move(char_type* s1, const char_type* s2, std::size_t n) {
            return static_cast<char_type*>(crt::memmove(s1, s2, n * sizeof(char_type)));
        }

        static char_type* copy(char_type* s1, const char_type* s2, std::size_t n) {
            return static_cast<char_type*>(crt::memcpy(s1, s2, n * sizeof(char_type)));
        }

        static char_type* assign(char_type* s, std::size_t n, char_type a) {
            for (std::size_t i = 0; i < n; ++i) s[i] = a;
            return s;
        }
    };

    template<typename CharT, typename Traits = char_traits<CharT>>
    class basic_string_view {
    public:
        using traits_type = Traits;
        using value_type = CharT;
        using pointer = CharT*;
        using const_pointer = const CharT*;
        using reference = CharT&;
        using const_reference = const CharT&;
        using const_iterator = const CharT*;
        using iterator = const_iterator;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        static constexpr size_type npos = static_cast<size_type>(-1);

        constexpr basic_string_view() : data_{ nullptr }, size_{ 0 } {}
        constexpr basic_string_view(const basic_string_view&) = default;
        constexpr basic_string_view& operator=(const basic_string_view&) = default;

        constexpr basic_string_view(const CharT* s, size_type count) : data_{ s }, size_{ count } {}
        constexpr basic_string_view(const CharT* s) : data_{ s }, size_{ Traits::length(s) } {}

        constexpr const_iterator begin()  const { return data_; }
        constexpr const_iterator end()    const { return data_ + size_; }
        constexpr const_iterator cbegin() const { return data_; }
        constexpr const_iterator cend()   const { return data_ + size_; }

        constexpr const_reference operator[](size_type pos) const { return data_[pos]; }
        constexpr const_reference front() const { return data_[0]; }
        constexpr const_reference back()  const { return data_[size_ - 1]; }
        constexpr const_pointer   data()  const { return data_; }
        constexpr size_type       size()  const { return size_; }
        constexpr size_type       length() const { return size_; }
        constexpr bool            empty() const { return size_ == 0; }

        constexpr void remove_prefix(size_type n) { data_ += n; size_ -= n; }
        constexpr void remove_suffix(size_type n) { size_ -= n; }

        constexpr void swap(basic_string_view& v) {
            stl::swap(data_, v.data_);
            stl::swap(size_, v.size_);
        }

        constexpr size_type copy(CharT* dest, size_type count, size_type pos = 0) const {
            if (pos > size_) return 0;
            const size_type rlen = count > size_ - pos ? size_ - pos : count;
            Traits::copy(dest, data_ + pos, rlen);
            return rlen;
        }

        constexpr basic_string_view substr(size_type pos = 0, size_type count = npos) const {
            if (pos > size_) return {};
            const size_type rlen = count > size_ - pos ? size_ - pos : count;
            return { data_ + pos, rlen };
        }

        constexpr int compare(basic_string_view v) const {
            const size_type rlen = size_ < v.size_ ? size_ : v.size_;
            int result = Traits::compare(data_, v.data_, rlen);
            if (result == 0) {
                if (size_ < v.size_) return -1;
                if (size_ > v.size_) return  1;
            }
            return result;
        }

        constexpr int compare(size_type pos1, size_type count1, basic_string_view v) const {
            return substr(pos1, count1).compare(v);
        }

        constexpr int compare(const CharT* s) const {
            return compare(basic_string_view(s));
        }

        constexpr bool starts_with(basic_string_view x) const {
            return size_ >= x.size_ && Traits::compare(data_, x.data_, x.size_) == 0;
        }

        constexpr bool starts_with(CharT x) const {
            return !empty() && Traits::eq(front(), x);
        }

        constexpr bool starts_with(const CharT* x) const {
            return starts_with(basic_string_view(x));
        }

        constexpr bool ends_with(basic_string_view x) const {
            return size_ >= x.size_ && Traits::compare(data_ + size_ - x.size_, x.data_, x.size_) == 0;
        }

        constexpr bool ends_with(CharT x) const {
            return !empty() && Traits::eq(back(), x);
        }

        constexpr bool ends_with(const CharT* x) const {
            return ends_with(basic_string_view(x));
        }

        constexpr size_type find(basic_string_view v, size_type pos = 0) const {
            if (pos > size_ || v.size_ > size_ - pos) return npos;
            if (v.size_ == 0) return pos;

            for (size_type i = pos; i <= size_ - v.size_; ++i) {
                if (Traits::compare(data_ + i, v.data_, v.size_) == 0)
                    return i;
            }
            return npos;
        }

        constexpr size_type find(CharT c, size_type pos = 0) const {
            if (pos >= size_) return npos;
            const CharT* p = Traits::find(data_ + pos, size_ - pos, c);
            return p ? static_cast<size_type>(p - data_) : npos;
        }

    private:
        const CharT* data_;
        size_type    size_;
    };

    template<typename CharT, typename Traits>
    constexpr bool operator==(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs) {
        return lhs.compare(rhs) == 0;
    }

    template<typename CharT, typename Traits>
    constexpr bool operator==(basic_string_view<CharT, Traits> lhs, const CharT* rhs) {
        return lhs == basic_string_view<CharT, Traits>(rhs);
    }

    template<typename CharT, typename Traits>
    constexpr bool operator==(const CharT* lhs, basic_string_view<CharT, Traits> rhs) {
        return basic_string_view<CharT, Traits>(lhs) == rhs;
    }

    template<typename CharT, typename Traits>
    constexpr bool operator!=(basic_string_view<CharT, Traits> lhs, basic_string_view<CharT, Traits> rhs) {
        return !(lhs == rhs);
    }

    template<typename CharT, typename Traits>
    constexpr bool operator!=(basic_string_view<CharT, Traits> lhs, const CharT* rhs) {
        return !(lhs == rhs);
    }

    template<typename CharT, typename Traits>
    constexpr bool operator!=(const CharT* lhs, basic_string_view<CharT, Traits> rhs) {
        return !(lhs == rhs);
    }

    using string_view = basic_string_view<char>;
    using u16string_view = basic_string_view<char16_t>;

}
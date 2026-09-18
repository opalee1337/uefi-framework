#pragma once

#include <cstdint>

// thanks to @ shmurkio (https://github.com/Shmurkio)

extern "C" {
    void* memcpy(void* dst, const void* src, std::uintptr_t n);
    void* memmove(void* dst, const void* src, std::uintptr_t n);
    void* memset(void* dst, int val, std::uintptr_t n);
    int   memcmp(const void* a, const void* b, std::uintptr_t n);
}

namespace crt {
    inline void* memcpy(void* dst, const void* src, std::uintptr_t n) { return ::memcpy(dst, src, n); }
    inline void* memmove(void* dst, const void* src, std::uintptr_t n) { return ::memmove(dst, src, n); }
    inline void* memset(void* dst, std::uint8_t val, std::uintptr_t n) { return ::memset(dst, static_cast<int>(val), n); }
    inline int   memcmp(const void* a, const void* b, std::uintptr_t n) { return ::memcmp(a, b, n); }
    inline void  memzero(void* dst, std::uintptr_t n) { ::memset(dst, 0, n); }

    inline const void* memchr(const void* src, std::uint8_t val, std::uintptr_t n) {
        auto* s = static_cast<const std::uint8_t*>(src);
        while (n--) {
            if (*s == val) return s;
            ++s;
        }
        return nullptr;
    }
}
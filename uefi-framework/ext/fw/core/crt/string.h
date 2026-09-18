#pragma once

#include "memory/memory.h"
#include "fw/efi/abi.h"

namespace crt {

    inline constexpr std::uintptr_t strlen(const char* s) {
        const char* p = s;
        while (*p) ++p;
        return p - s;
    }

    inline char* strcpy(char* dst, const char* src) {
        char* d = dst;
        while ((*d++ = *src++));
        return dst;
    }

    inline char* strncpy(char* dst, const char* src, std::uintptr_t n) {
        char* d = dst;
        while (n && (*d++ = *src++)) --n;
        memset(d, 0, n);
        return dst;
    }

    inline int strcmp(const char* a, const char* b) {
        while (*a && *a == *b) { ++a; ++b; }
        return static_cast<unsigned char>(*a) - static_cast<unsigned char>(*b);
    }

    inline int strncmp(const char* a, const char* b, std::uintptr_t n) {
        for (std::uintptr_t i = 0; i < n; ++i) {
            unsigned char ca = static_cast<unsigned char>(a[i]);
            unsigned char cb = static_cast<unsigned char>(b[i]);

            if (ca != cb)
                return ca < cb ? -1 : 1;

            if (ca == '\0')
                return 0;
        }

        return 0;
    }

    inline const char* strchr(const char* s, char c) {
        while (*s) {
            if (*s == c) return s;
            ++s;
        }
        return c == '\0' ? s : nullptr;
    }

    inline const char* strstr(const char* hay, const char* needle) {
        if (!*needle) return hay;
        for (; *hay; ++hay) {
            const char* h = hay, * n = needle;
            while (*h && *n && *h == *n) { ++h; ++n; }
            if (!*n) return hay;
        }
        return nullptr;
    }

    // wide
    inline constexpr std::uintptr_t wcslen(const efi::char16* s) {
        const efi::char16* p = s;
        while (*p) ++p;
        return p - s;
    }

    inline efi::char16* wcscpy(efi::char16* dst, const efi::char16* src) {
        efi::char16* d = dst;
        while ((*d++ = *src++));
        return dst;
    }

    inline efi::char16* wcsncpy(efi::char16* dst, const efi::char16* src, std::uintptr_t n) {
        efi::char16* d = dst;
        while (n && (*d++ = *src++)) --n;
        memzero(d, n * sizeof(efi::char16));
        return dst;
    }

    inline int wcscmp(const efi::char16* a, const efi::char16* b) {
        while (*a && *a == *b) { ++a; ++b; }
        return static_cast<int>(*a) - static_cast<int>(*b);
    }

    inline int wcsncmp(const efi::char16* a, const efi::char16* b, std::uintptr_t n) {
        for (std::uintptr_t i = 0; i < n; ++i) {
            efi::char16 ca = a[i];
            efi::char16 cb = b[i];

            if (ca != cb)
                return ca < cb ? -1 : 1;

            if (ca == u'\0')
                return 0;
        }

        return 0;
    }

    inline const efi::char16* wcschr(const efi::char16* s, efi::char16 c) {
        while (*s) {
            if (*s == c) return s;
            ++s;
        }
        return c == u'\0' ? s : nullptr;
    }

    inline const efi::char16* wcsstr(const efi::char16* hay, const efi::char16* needle) {
        if (!*needle) return hay;
        for (; *hay; ++hay) {
            const efi::char16* h = hay, * n = needle;
            while (*h && *n && *h == *n) { ++h; ++n; }
            if (!*n) return hay;
        }
        return nullptr;
    }


    // addition
    inline void wcstombs(char* dst, const efi::char16* src, std::uintptr_t max) {
        std::uintptr_t i = 0;
        if (src)
            for (; i < max - 1 && src[i]; ++i)
                dst[i] = (src[i] < 0x80) ? static_cast<char>(src[i]) : '?';
        dst[i] = '\0';
    }

}
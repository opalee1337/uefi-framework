#include "pattern_scan.h"

namespace {

    std::uint8_t hex_nibble(char c) {
        if (c >= '0' && c <= '9') return static_cast<std::uint8_t>(c - '0');
        if (c >= 'A' && c <= 'F') return static_cast<std::uint8_t>(c - 'A' + 10);
        if (c >= 'a' && c <= 'f') return static_cast<std::uint8_t>(c - 'a' + 10);
        return 0;
    }

    bool match_at(const std::uint8_t* candidate, std::uintptr_t remaining, stl::string_view pattern) {
        const char* p = pattern.data();
        const char* end = p + pattern.size();
        std::uintptr_t off = 0;

        while (p < end) {
            while (p < end && *p == ' ') ++p;
            if (p >= end) break;

            if (off >= remaining) return false;

            if (*p == '?') {
                ++p;
                if (p < end && *p == '?') ++p;
            }
            else {
                if (p + 2 > end) return false;
                const std::uint8_t byte = static_cast<std::uint8_t>((hex_nibble(p[0]) << 4) | hex_nibble(p[1]));
                if (candidate[off] != byte) return false;
                p += 2;
            }
            ++off;
        }

        return true;
    }

}

namespace ztils {

    void* scan(void* base, std::uintptr_t size, stl::string_view pattern) {
        if (!base || !size || pattern.empty()) return nullptr;

        auto* p = static_cast<std::uint8_t*>(base);
        for (std::uintptr_t i = 0; i < size; ++i) {
            if (match_at(p + i, size - i, pattern))
                return p + i;
        }
        return nullptr;
    }

    void* scan(stl::span<std::uint8_t> region, stl::string_view pattern) {
        return scan(region.data(), region.size(), pattern);
    }

}
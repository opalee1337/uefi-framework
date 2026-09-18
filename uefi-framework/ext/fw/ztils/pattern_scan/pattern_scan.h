#pragma once

#include <cstdint>

#include "fw/core/stl/string/string_view.h"
#include "fw/core/stl/types/span.h"

// ida style pattern scanner
namespace ztils {

	void* scan(void* base, std::uintptr_t size, stl::string_view pattern);
	void* scan(stl::span<std::uint8_t> region, stl::string_view pattern);

    template <typename T = void>
    T* scan_as(void* base, std::uintptr_t size, stl::string_view pattern) {
        return static_cast<T*>(scan(base, size, pattern));
    }

    template <typename T = void>
    T* scan_as(stl::span<std::uint8_t> region, stl::string_view pattern) {
        return static_cast<T*>(scan(region, pattern));
    }
}
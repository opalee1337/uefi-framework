#pragma once

#include <cstdint>

#include "status.h"
#include "guid.h"

#if defined(__x86_64__)
    #define EFIAPI __attribute__((ms_abi))
#elif defined(__i386__)
    #define EFIAPI __cdecl
#else
    #define EFIAPI
#endif

namespace efi {
    enum class tpl : std::uintptr_t {
        application = 4,
        callback = 8,
        notify = 16,
        high_level = 31
    };

    using handle = void*;
    using event  = void*;

    using physical_addr = std::uint64_t;
    using virtual_addr  = std::uint64_t;

    using char16 = char16_t;
}

using efi_handle = efi::handle;
using efi_event  = efi::event;
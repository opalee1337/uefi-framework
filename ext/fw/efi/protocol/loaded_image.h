#pragma once

#include "fw/efi/abi.h"
#include "fw/efi/service/boot_services.h"

#include "device_path.h"

// https://uefi.org/specs/UEFI/2.11/09_Protocols_EFI_Loaded_Image.html

namespace efi {

    struct system_table;
    struct boot_services;

    using image_unload_fn = efi_status_t(EFIAPI*)(handle image_handle);

    constexpr std::uint32_t efi_loaded_image_revision = 0x1000;

    struct loaded_image {
        std::uint32_t revision;
        handle parent_handle;

        system_table* system_table;

        handle device_handle;

        device_path* file_path;
        void* reserved;

        std::uint32_t load_options_size;
        void* load_options;

        void* image_base;
        std::uint64_t image_size;

        memory_type image_code_type;
        memory_type image_data_type;

        image_unload_fn unload;

        [[nodiscard]] constexpr bool has_load_options() const {
            return load_options != nullptr && load_options_size > 0;
        }

        [[nodiscard]] constexpr const char16* load_options_text() const {
            return static_cast<const char16*>(load_options);
        }
    };

    inline constexpr efi_guid efi_loaded_image_protocol_guid = efi::make_guid(0x5B1B31A1, 0x9562, 0x11d2, 0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B);
}
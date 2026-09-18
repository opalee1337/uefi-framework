#pragma once

#include "fw/efi/abi.h"

// https://uefi.org/specs/UEFI/2.11/12_Protocols_Console_Support.html#efi-graphics-output-protocol

namespace efi {

    struct pixel_bitmask {
        std::uint32_t red;
        std::uint32_t green;
        std::uint32_t blue;
        std::uint32_t reserved;
    };

    enum class pixel_format : std::uint32_t {
        rgb_reserved_8bpc,
        bgr_reserved_8bpc,
        bit_mask,
        blt_only,
        max
    };

    struct graphics_output_mode_info {
        std::uint32_t version;

        std::uint32_t horizontal_resolution;
        std::uint32_t vertical_resolution;

        pixel_format  pixel_format;
        pixel_bitmask pixel_information;
        std::uint32_t pixels_per_scan_line;
    };

    struct graphics_output_mode {
        std::uint32_t max_mode;
        std::uint32_t mode;

        graphics_output_mode_info* info;

        std::uintptr_t size_of_info;
        physical_addr  frame_buffer_base;
        std::uintptr_t frame_buffer_size;
    };

    struct blt_pixel {
        std::uint8_t blue;
        std::uint8_t green;
        std::uint8_t red;
        std::uint8_t reserved;
    };

    enum class blt_operation : std::uint32_t {
        video_fill,
        video_to_blt_buffer,
        blt_buffer_to_video,
        video_to_video,
        max
    };

    struct graphics_output;

    using graphics_output_query_mode_fn = efi_status_t(EFIAPI*)(graphics_output* self, std::uint32_t mode_number, std::uintptr_t* size_of_info, graphics_output_mode_info** info);
    using graphics_output_set_mode_fn   = efi_status_t(EFIAPI*)(graphics_output* self, std::uint32_t mode_number);
    using graphics_output_blt_fn        = efi_status_t(EFIAPI*)(graphics_output* self, blt_pixel* blt_buffer, blt_operation operation, std::uintptr_t src_x, std::uintptr_t src_y, std::uintptr_t dst_x, std::uintptr_t dst_y, std::uintptr_t width, std::uintptr_t height, std::uintptr_t delta);

    struct graphics_output {
        graphics_output_query_mode_fn query_mode;
        graphics_output_set_mode_fn set_mode;

        graphics_output_blt_fn blt;

        graphics_output_mode* mode;
    };

    inline constexpr efi_guid efi_graphics_output_protocol_guid = efi::make_guid(0x9042A9DE, 0x23DC, 0x4A38, 0x96, 0xFB, 0x7A, 0xDE, 0xD0, 0x80, 0x51, 0x6A);
}
#pragma once

#include "fw/efi/abi.h"

// https://uefi.org/specs/UEFI/2.11/12_Protocols_Console_Support.html

namespace efi {

    namespace text_color {
        constexpr std::uintptr_t black         = 0x00;
        constexpr std::uintptr_t blue          = 0x01;
        constexpr std::uintptr_t green         = 0x02;
        constexpr std::uintptr_t cyan          = 0x03;
        constexpr std::uintptr_t red           = 0x04;
        constexpr std::uintptr_t magenta       = 0x05;
        constexpr std::uintptr_t brown         = 0x06;
        constexpr std::uintptr_t light_gray    = 0x07;
        constexpr std::uintptr_t dark_gray     = 0x08;
        constexpr std::uintptr_t light_blue    = 0x09;
        constexpr std::uintptr_t light_green   = 0x0A;
        constexpr std::uintptr_t light_cyan    = 0x0B;
        constexpr std::uintptr_t light_red     = 0x0C;
        constexpr std::uintptr_t light_magenta = 0x0D;
        constexpr std::uintptr_t yellow        = 0x0E;
        constexpr std::uintptr_t white         = 0x0F;

        namespace bg {
            constexpr std::uintptr_t black      = 0x00;
            constexpr std::uintptr_t blue       = 0x10;
            constexpr std::uintptr_t green      = 0x20;
            constexpr std::uintptr_t cyan       = 0x30;
            constexpr std::uintptr_t red        = 0x40;
            constexpr std::uintptr_t magenta    = 0x50;
            constexpr std::uintptr_t brown      = 0x60;
            constexpr std::uintptr_t light_gray = 0x70;
        }

        constexpr std::uintptr_t make(std::uintptr_t fg, std::uintptr_t bg) {
            return fg | bg;
        }
    }

    struct simple_text_output_mode {
        std::int32_t max_mode;
        std::int32_t mode;

        std::int32_t attribute;
        std::int32_t cursor_column;
        std::int32_t cursor_row;
        bool cursor_visible;
    };

    struct simple_text_output;

    using text_reset_fn          = efi_status_t(EFIAPI*)(simple_text_output* self, bool extended_verification);
    using text_string_fn         = efi_status_t(EFIAPI*)(simple_text_output* self, const char16* string);
    using text_test_string_fn    = efi_status_t(EFIAPI*)(simple_text_output* self, const char16* string);
    using text_query_mode_fn     = efi_status_t(EFIAPI*)(simple_text_output* self, std::uintptr_t mode_number, std::uintptr_t* columns, std::uintptr_t* rows);
    using text_set_mode_fn       = efi_status_t(EFIAPI*)(simple_text_output* self, std::uintptr_t mode_number);
    using text_set_attribute_fn  = efi_status_t(EFIAPI*)(simple_text_output* self, std::uintptr_t attribute);
    using text_clear_screen_fn   = efi_status_t(EFIAPI*)(simple_text_output* self);
    using text_set_cursor_pos_fn = efi_status_t(EFIAPI*)(simple_text_output* self, std::uintptr_t column, std::uintptr_t row);
    using text_enable_cursor_fn  = efi_status_t(EFIAPI*)(simple_text_output* self, bool visible);

    struct simple_text_output {
        text_reset_fn reset;

        text_string_fn output_string;
        text_test_string_fn test_string;

        text_query_mode_fn query_mode;

        text_set_mode_fn set_mode;
        text_set_attribute_fn set_attribute;

        text_clear_screen_fn clear_screen;

        text_set_cursor_pos_fn set_cursor_position;
        text_enable_cursor_fn enable_cursor;

        simple_text_output_mode* mode;
    };

    inline constexpr efi_guid efi_simple_text_output_protocol_guid = efi::make_guid(0x387477C2, 0x69C7, 0x11D2, 0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B);
}
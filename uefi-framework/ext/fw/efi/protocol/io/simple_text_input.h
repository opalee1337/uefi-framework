#pragma once

#include "fw/efi/abi.h"

// https://uefi.org/specs/UEFI/2.11/12_Protocols_Console_Support.html

namespace efi {

    namespace scan_code {
        constexpr std::uint16_t null      = 0x0000;
        constexpr std::uint16_t up        = 0x0001;
        constexpr std::uint16_t down      = 0x0002;
        constexpr std::uint16_t right     = 0x0003;
        constexpr std::uint16_t left      = 0x0004;
        constexpr std::uint16_t home      = 0x0005;
        constexpr std::uint16_t end       = 0x0006;
        constexpr std::uint16_t insert    = 0x0007;
        constexpr std::uint16_t del       = 0x0008;
        constexpr std::uint16_t page_up   = 0x0009;
        constexpr std::uint16_t page_down = 0x000A;
        constexpr std::uint16_t f1        = 0x000B;
        constexpr std::uint16_t f2        = 0x000C;
        constexpr std::uint16_t f3        = 0x000D;
        constexpr std::uint16_t f4        = 0x000E;
        constexpr std::uint16_t f5        = 0x000F;
        constexpr std::uint16_t f6        = 0x0010;
        constexpr std::uint16_t f7        = 0x0011;
        constexpr std::uint16_t f8        = 0x0012;
        constexpr std::uint16_t f9        = 0x0013;
        constexpr std::uint16_t f10       = 0x0014;
        constexpr std::uint16_t esc       = 0x0017;
    }

    struct input_key {
        std::uint16_t scan_code;
        char16 unicode_char;

        [[nodiscard]] constexpr bool has_unicode() const { return unicode_char != 0; }
        [[nodiscard]] constexpr bool has_scan_code() const { return scan_code != 0; }
        [[nodiscard]] constexpr bool empty() const { return scan_code == 0 && unicode_char == 0; }

        [[nodiscard]] constexpr bool is_escape() const { return scan_code == scan_code::esc; }
    };

    struct simple_text_input;

    using input_reset_fn    = efi_status_t(EFIAPI*)(simple_text_input* self, bool extended_verification);
    using input_read_key_fn = efi_status_t(EFIAPI*)(simple_text_input* self, input_key* key);

    struct simple_text_input {
        input_reset_fn reset;
        input_read_key_fn read_key_stroke;

        efi_event wait_for_key;
    };

    inline constexpr efi_guid efi_simple_text_input_protocol_guid = efi::make_guid(0x387477C1, 0x69C7, 0x11D2, 0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B);
}
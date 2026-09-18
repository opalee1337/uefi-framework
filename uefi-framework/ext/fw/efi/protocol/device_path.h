#pragma once

#include "fw/efi/abi.h"

// https://uefi.org/specs/UEFI/2.11/10_Protocols_Device_Path_Protocol.html

namespace efi {

    namespace device_path_type {
        constexpr std::uint8_t hardware  = 0x01;
        constexpr std::uint8_t acpi      = 0x02;
        constexpr std::uint8_t messaging = 0x03;
        constexpr std::uint8_t media     = 0x04;
        constexpr std::uint8_t bbs       = 0x05;
        constexpr std::uint8_t end       = 0x7F;
    }

    namespace device_path_subtype {
        constexpr std::uint8_t end_instance = 0x01;
        constexpr std::uint8_t end_entire   = 0xFF;
    }

    struct device_path {
        std::uint8_t type;
        std::uint8_t sub_type;
        std::uint8_t length[2];

        [[nodiscard]] constexpr std::uint16_t node_size() const {
            return static_cast<std::uint16_t>(length[0]) | static_cast<std::uint16_t>(length[1]) << 8;
        }

        [[nodiscard]] device_path* next() const {
            return reinterpret_cast<device_path*>(reinterpret_cast<std::uintptr_t>(this) + node_size());
        }

        [[nodiscard]] constexpr bool is_end()          const { return type == device_path_type::end; }
        [[nodiscard]] constexpr bool is_end_entire()   const { return is_end() && sub_type == device_path_subtype::end_entire; }
        [[nodiscard]] constexpr bool is_end_instance() const { return is_end() && sub_type == device_path_subtype::end_instance; }
    };

    inline constexpr efi_guid efi_device_path_protocol_guid = efi::make_guid(0x09576E91, 0x6D3F, 0x11D2, 0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B);
}
#pragma once

#include "fw/efi/abi.h"

// https://uefi.org/specs/UEFI/2.11/13_Protocols_Media_Access.html

namespace efi {

    namespace file_revision {
        constexpr std::uint64_t v1 = 0x00010000;
        constexpr std::uint64_t v2 = 0x00020000;
    }

    namespace simple_file_system_revision {
        constexpr std::uint64_t v1 = 0x00010000;
    }

    namespace file_open_mode {
        constexpr std::uint64_t read   = 0x0000000000000001ULL;
        constexpr std::uint64_t write  = 0x0000000000000002ULL;
        constexpr std::uint64_t create = 0x8000000000000000ULL;
    }

    namespace file_attribute {
        constexpr std::uint64_t read_only  = 0x0000000000000001ULL;
        constexpr std::uint64_t hidden     = 0x0000000000000002ULL;
        constexpr std::uint64_t system     = 0x0000000000000004ULL;
        constexpr std::uint64_t reserved   = 0x0000000000000008ULL;
        constexpr std::uint64_t directory  = 0x0000000000000010ULL;
        constexpr std::uint64_t archive    = 0x0000000000000020ULL;
        constexpr std::uint64_t valid_attr = 0x0000000000000037ULL;
    }

    struct file_info {
        std::uint64_t size;
        std::uint64_t file_size;
        std::uint64_t physical_size;

        std::uint8_t  _time_padding[24]; // efi_time x3
        std::uint64_t attribute;

        char16 file_name[1];
    };

    struct file;
    struct simple_file_system;

    using file_open_fn         = efi_status_t(EFIAPI*)(file* self, file** new_handle, const char16* file_name, std::uint64_t open_mode, std::uint64_t attributes);
    using file_close_fn        = efi_status_t(EFIAPI*)(file* self);
    using file_delete_fn       = efi_status_t(EFIAPI*)(file* self);
    using file_read_fn         = efi_status_t(EFIAPI*)(file* self, std::uintptr_t* buffer_size, void* buffer);
    using file_write_fn        = efi_status_t(EFIAPI*)(file* self, std::uintptr_t* buffer_size, const void* buffer);
    using file_get_position_fn = efi_status_t(EFIAPI*)(file* self, std::uint64_t* position);
    using file_set_position_fn = efi_status_t(EFIAPI*)(file* self, std::uint64_t position);
    using file_get_info_fn     = efi_status_t(EFIAPI*)(file* self, const guid* information_type, std::uintptr_t* buffer_size, void* buffer);
    using file_set_info_fn     = efi_status_t(EFIAPI*)(file* self, const guid* information_type, std::uintptr_t buffer_size, const void* buffer);
    using file_flush_fn        = efi_status_t(EFIAPI*)(file* self);

    struct file {
        std::uint64_t revision;

        file_open_fn  open;
        file_close_fn close;

        file_delete_fn del;
        file_read_fn   read;
        file_write_fn  write;

        file_get_position_fn get_position;
        file_set_position_fn set_position;

        file_get_info_fn get_info;
        file_set_info_fn set_info;

        file_flush_fn flush;
    };

    inline constexpr guid file_info_guid        = make_guid(0x09576E92, 0x6D3F, 0x11D2, 0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B);
    inline constexpr guid file_system_info_guid = make_guid(0x09576E93, 0x6D3F, 0x11D2, 0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B);

    // simple_file_system

    using simple_file_system_open_volume_fn = efi_status_t(EFIAPI*)(simple_file_system* self, file** root);

    struct simple_file_system {
        std::uint64_t revision;

        simple_file_system_open_volume_fn open_volume;
    };


    inline constexpr guid simple_file_system_protocol_guid = make_guid(0x964E5B22, 0x6459, 0x11D2, 0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B);
}
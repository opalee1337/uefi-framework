#pragma once

#include "fw/efi/abi.h"
#include "boot_services.h"

// https://uefi.org/specs/UEFI/2.11/08_Services_Runtime_Services.html

namespace efi {

    enum class reset_type : std::uint32_t {
        cold,
        warm,
        shutdown,
        platform_specific
    };

    struct time {
        std::uint16_t year;
        std::uint8_t month;
        std::uint8_t day;

        std::uint8_t hour;
        std::uint8_t minute;
        std::uint8_t second;

        std::uint8_t pad1;

        std::uint32_t nanosecond;

        std::int16_t timezone;
        std::uint8_t daylight;

        std::uint8_t pad2;
    };

    struct time_capabilities {
        std::uint32_t resolution;
        std::uint32_t accuracy;

        bool sets_to_zero;
    };

    struct capsule_header {
        efi_guid capsule_guid;

        std::uint32_t header_size;
        std::uint32_t flags;
        std::uint32_t capsule_image_size;
    };

    using efi_get_time_fn                      = efi_status_t(EFIAPI*)(time* time, time_capabilities* capabilities);
    using efi_set_time_fn                      = efi_status_t(EFIAPI*)(time* time);
    using efi_get_wakeup_time_fn               = efi_status_t(EFIAPI*)(bool* enabled, bool* pending, time* time);
    using efi_set_wakeup_time_fn               = efi_status_t(EFIAPI*)(bool enable, time* time);
    using efi_set_virtual_address_map_fn       = efi_status_t(EFIAPI*)(std::uintptr_t memory_map_size, std::uintptr_t descriptor_size, std::uint32_t descriptor_version, memory_descriptor* virtual_map);
    using efi_convert_pointer_fn               = efi_status_t(EFIAPI*)(std::uintptr_t debug_disposition, void** address);
    using efi_get_variable_fn                  = efi_status_t(EFIAPI*)(char16* variable_name, efi_guid* vendor_guid, std::uint32_t* attributes, std::uintptr_t* data_size, void* data);
    using efi_get_next_variable_name_fn        = efi_status_t(EFIAPI*)(std::uintptr_t* variable_name_size, char16* variable_name, efi_guid* vendor_guid);
    using efi_set_variable_fn                  = efi_status_t(EFIAPI*)(char16* variable_name, efi_guid* vendor_guid, std::uint32_t attributes, std::uintptr_t data_size, void* data);
    using efi_get_next_high_monotonic_count_fn = efi_status_t(EFIAPI*)(std::uint32_t* high_count);
    using efi_reset_system_fn                  = void(EFIAPI*)(reset_type reset_type, efi_status_t reset_status, std::uintptr_t data_size, void* reset_data);
    using efi_update_capsule_fn                = efi_status_t(EFIAPI*)(capsule_header** capsule_header_array, std::uintptr_t capsule_count, physical_addr scatter_gather_list);
    using efi_query_capsule_capabilities_fn    = efi_status_t(EFIAPI*)(capsule_header** capsule_header_array, std::uintptr_t capsule_count, std::uint64_t* maximum_capsule_size, reset_type* reset_type);
    using efi_query_variable_info_fn           = efi_status_t(EFIAPI*)(std::uint32_t attributes, std::uint64_t* maximum_variable_storage_size, std::uint64_t* remaining_variable_storage_size, std::uint64_t* maximum_variable_size);

    struct runtime_services {
        table_header hdr;

        efi_get_time_fn get_time;
        efi_set_time_fn set_time;
        efi_get_wakeup_time_fn get_wakeup_time;
        efi_set_wakeup_time_fn set_wakeup_time;

        efi_set_virtual_address_map_fn set_virtual_address_map;
        efi_convert_pointer_fn convert_pointer;

        efi_get_variable_fn get_variable;
        efi_get_next_variable_name_fn get_next_variable_name;
        efi_set_variable_fn set_variable;

        efi_get_next_high_monotonic_count_fn get_next_high_monotonic_count;
        efi_reset_system_fn reset_system;

        efi_update_capsule_fn update_capsule;
        efi_query_capsule_capabilities_fn query_capsule_capabilities;
        efi_query_variable_info_fn query_variable_info;
    };
}
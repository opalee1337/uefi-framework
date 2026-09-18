#pragma once

#include "fw/efi/abi.h"

// https://uefi.org/specs/UEFI/2.11/07_Services_Boot_Services.html

namespace efi {

    enum class memory_type : std::uint32_t {
        reserved,
        loader_code,
        loader_data,
        boot_services_code,
        boot_services_data,
        runtime_services_code,
        runtime_services_data,
        conventional,
        unusable,
        acpi_reclaim,
        acpi_nvs,
        mmio,
        mmio_port_space,
        pal_code,
        persistent,
        max
    };

    enum class allocate_type : std::uint32_t {
        any_pages,
        max_address,
        at_address,
        max
    };

    enum class timer_delay : std::uint32_t {
        cancel,
        periodic,
        relative
    };

    enum class interface_type : std::uint32_t {
        native_interface
    };

    enum class locate_search_type : std::uint32_t {
        all_handles,
        by_register_notify,
        by_protocol
    };

    struct table_header {
        std::uint64_t signature;
        std::uint32_t revision;
        std::uint32_t header_size;
        std::uint32_t crc32;
        std::uint32_t reserved;
    };

    struct memory_descriptor {
        std::uint32_t type;

        physical_addr physical_start;
        virtual_addr virtual_start;

        std::uint64_t number_of_pages;
        std::uint64_t attribute;
    };

    struct open_protocol_information_entry {
        efi_handle agent_handle;
        efi_handle controller_handle;

        std::uint32_t attributes;
        std::uint32_t open_count;
    };

    struct device_path;

    using efi_event_notify_fn                           = void(EFIAPI*)(efi_event event, void* ctx);
    using efi_raise_tpl_fn                              = tpl(EFIAPI*)(tpl new_tpl);
    using efi_restore_tpl_fn                            = void(EFIAPI*)(tpl old_tpl);
    using efi_allocate_pages_fn                         = efi_status_t(EFIAPI*)(allocate_type type, memory_type memory_type, std::uintptr_t pages, physical_addr* memory);
    using efi_free_pages_fn                             = efi_status_t(EFIAPI*)(physical_addr memory, std::uintptr_t pages);
    using efi_get_memory_map_fn                         = efi_status_t(EFIAPI*)(std::uintptr_t* memory_map_size, memory_descriptor* memory_map, std::uintptr_t* map_key, std::uintptr_t* descriptor_size, std::uint32_t* descriptor_version);
    using efi_allocate_pool_fn                          = efi_status_t(EFIAPI*)(memory_type pool_type, std::uintptr_t size, void** buffer);
    using efi_free_pool_fn                              = efi_status_t(EFIAPI*)(void* buffer);
    using efi_create_event_fn                           = efi_status_t(EFIAPI*)(std::uint32_t type, tpl notify_tpl, efi_event_notify_fn notify_fn, void* notify_ctx, efi_event* event);
    using efi_set_timer_fn                              = efi_status_t(EFIAPI*)(efi_event event, timer_delay type, std::uint64_t trigger_time);
    using efi_wait_for_event_fn                         = efi_status_t(EFIAPI*)(std::uintptr_t num_events, efi_event* event, std::uintptr_t* index);
    using efi_signal_event_fn                           = efi_status_t(EFIAPI*)(efi_event event);
    using efi_close_event_fn                            = efi_status_t(EFIAPI*)(efi_event event);
    using efi_check_event_fn                            = efi_status_t(EFIAPI*)(efi_event event);
    using efi_install_protocol_interface_fn             = efi_status_t(EFIAPI*)(efi_handle* handle, const efi_guid* protocol, interface_type interface_type, void* interface);
    using efi_reinstall_protocol_interface_fn           = efi_status_t(EFIAPI*)(efi_handle handle, const efi_guid* protocol, void* old_interface, void* new_interface);
    using efi_uninstall_protocol_interface_fn           = efi_status_t(EFIAPI*)(efi_handle handle, const efi_guid* protocol, void* interface);
    using efi_handle_protocol_fn                        = efi_status_t(EFIAPI*)(efi_handle handle, const efi_guid* protocol, void** interface);
    using efi_register_protocol_notify_fn               = efi_status_t(EFIAPI*)(const efi_guid* protocol, efi_event event, void** registration);
    using efi_locate_handle_fn                          = efi_status_t(EFIAPI*)(locate_search_type search_type, const efi_guid* protocol, void* search_key, std::uintptr_t* buffer_size, efi_handle* buffer);
    using efi_locate_device_path_fn                     = efi_status_t(EFIAPI*)(const efi_guid* protocol, device_path** device_path, efi_handle* device);
    using efi_install_configuration_table_fn            = efi_status_t(EFIAPI*)(const efi_guid* guid, void* table);
    using efi_open_protocol_fn                          = efi_status_t(EFIAPI*)(efi_handle handle, const efi_guid* protocol, void** interface, efi_handle agent_handle, efi_handle controller_handle, std::uint32_t attributes);
    using efi_close_protocol_fn                         = efi_status_t(EFIAPI*)(efi_handle handle, const efi_guid* protocol, efi_handle agent_handle, efi_handle controller_handle);
    using efi_open_protocol_information_fn              = efi_status_t(EFIAPI*)(efi_handle handle, const efi_guid* protocol, open_protocol_information_entry** entry_buffer, std::uintptr_t* entry_count);
    using efi_protocols_per_handle_fn                   = efi_status_t(EFIAPI*)(efi_handle handle, efi_guid*** protocol_buffer, std::uintptr_t* protocol_buffer_count);
    using efi_locate_handle_buffer_fn                   = efi_status_t(EFIAPI*)(locate_search_type search_type, const efi_guid* protocol, void* search_key, std::uintptr_t* no_handles, efi_handle** buffer);
    using efi_locate_protocol_fn                        = efi_status_t(EFIAPI*)(const efi_guid* protocol, void* registration, void** interface);
    using efi_install_multiple_protocol_interfaces_fn   = efi_status_t(EFIAPI*)(efi_handle* handle, ...);
    using efi_uninstall_multiple_protocol_interfaces_fn = efi_status_t(EFIAPI*)(efi_handle handle, ...);
    using efi_load_image_fn                             = efi_status_t(EFIAPI*)(bool boot_policy, efi_handle parent_image_handle, device_path* device_path, void* source_buffer, std::uintptr_t source_size, efi_handle* image_handle);
    using efi_start_image_fn                            = efi_status_t(EFIAPI*)(efi_handle image_handle, std::uintptr_t* exit_data_size, char16** exit_data);
    using efi_exit_fn                                   = efi_status_t(EFIAPI*)(efi_handle image_handle, efi_status_t exit_status, std::uintptr_t exit_data_size, char16* exit_data);
    using efi_unload_image_fn                           = efi_status_t(EFIAPI*)(efi_handle image_handle);
    using efi_exit_boot_services_fn                     = efi_status_t(EFIAPI*)(efi_handle image_handle, std::uintptr_t map_key);
    using efi_get_next_monotonic_count_fn               = efi_status_t(EFIAPI*)(std::uint64_t* count);
    using efi_stall_fn                                  = efi_status_t(EFIAPI*)(std::uintptr_t microseconds);
    using efi_set_watchdog_timer_fn                     = efi_status_t(EFIAPI*)(std::uintptr_t timeout, std::uint64_t watchdog_code, std::uintptr_t data_size, char16* watchdog_data);
    using efi_connect_controller_fn                     = efi_status_t(EFIAPI*)(efi_handle controller_handle, efi_handle* driver_image_handle, device_path* remaining_device_path, bool recursive);
    using efi_disconnect_controller_fn                  = efi_status_t(EFIAPI*)(efi_handle controller_handle, efi_handle driver_image_handle, efi_handle child_handle);
    using efi_calculate_crc32_fn                        = efi_status_t(EFIAPI*)(void* data, std::uintptr_t data_size, std::uint32_t* crc32);
    using efi_copy_mem_fn                               = void(EFIAPI*)(void* dest, void* src, std::uintptr_t length);
    using efi_set_mem_fn                                = void(EFIAPI*)(void* buffer, std::uintptr_t size, std::uint8_t value);
    using efi_create_event_ex_fn                        = efi_status_t(EFIAPI*)(std::uint32_t type, tpl notify_tpl, efi_event_notify_fn notify_fn, const void* notify_ctx, const efi_guid* event_group, efi_event* event);

    struct boot_services {
        table_header hdr;

        efi_raise_tpl_fn raise_tpl;
        efi_restore_tpl_fn restore_tpl;

        efi_allocate_pages_fn allocate_pages;
        efi_free_pages_fn free_pages;

        efi_get_memory_map_fn get_memory_map;

        efi_allocate_pool_fn allocate_pool;
        efi_free_pool_fn free_pool;

        efi_create_event_fn create_event;
        efi_set_timer_fn set_timer;

        efi_wait_for_event_fn wait_for_event;
        efi_signal_event_fn signal_event;

        efi_close_event_fn close_event;
        efi_check_event_fn check_event;

        efi_install_protocol_interface_fn install_protocol_interface;
        efi_reinstall_protocol_interface_fn reinstall_protocol_interface;
        efi_uninstall_protocol_interface_fn uninstall_protocol_interface;

        efi_handle_protocol_fn handle_protocol;

        void* reserved;
        efi_register_protocol_notify_fn register_protocol_notify;
        efi_locate_handle_fn locate_handle;
        efi_locate_device_path_fn locate_device_path;
        efi_install_configuration_table_fn install_configuration_table;

        efi_load_image_fn load_image;
        efi_start_image_fn start_image;

        efi_exit_fn exit;
        efi_unload_image_fn unload_image;
        efi_exit_boot_services_fn exit_boot_services;

        efi_get_next_monotonic_count_fn get_next_monotonic_count;
        efi_stall_fn stall;
        efi_set_watchdog_timer_fn set_watchdog_timer;

        efi_connect_controller_fn connect_controller;
        efi_disconnect_controller_fn disconnect_controller;

        efi_open_protocol_fn open_protocol;
        efi_close_protocol_fn close_protocol;

        efi_open_protocol_information_fn open_protocol_information;
        efi_protocols_per_handle_fn protocols_per_handle;

        efi_locate_handle_buffer_fn locate_handle_buffer;
        efi_locate_protocol_fn locate_protocol;

        efi_install_multiple_protocol_interfaces_fn install_multiple_protocol_interfaces;
        efi_uninstall_multiple_protocol_interfaces_fn uninstall_multiple_protocol_interfaces;

        efi_calculate_crc32_fn  calculate_crc32;
        efi_copy_mem_fn copy_mem;
        efi_set_mem_fn set_mem;

        efi_create_event_ex_fn  create_event_ex;
    };
}
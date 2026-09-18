#pragma once

#include "fw/efi/abi.h"

#include "boot_services.h"
#include "runtime_services.h"

#include "fw/efi/protocol/io/simple_text_input.h"
#include "fw/efi/protocol/io/simple_text_output.h"

// https://uefi.org/specs/UEFI/2.11/04_EFI_System_Table.html

namespace efi {

    struct configuration_table {
        efi_guid vendor_guid;
        void* vendor_table;
    };

    struct system_table {
        table_header hdr;

        char16* firmware_vendor;
        std::uint32_t firmware_revision;

        efi_handle console_in_handle;
        simple_text_input* con_in;

        efi_handle console_out_handle;
        simple_text_output* con_out;

        efi_handle standard_error_handle;
        simple_text_output* std_err;

        runtime_services* runtime_services;
        boot_services* boot_services;

        std::uintptr_t number_of_table_entries;
        configuration_table* configuration_table;
    };

}
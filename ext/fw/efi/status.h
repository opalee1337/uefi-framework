#pragma once

#include <cstdint>

namespace efi {

	// https://uefi.org/specs/UEFI/2.11/Apx_D_Status_Codes.html
	namespace status_codes {
		constexpr std::uintptr_t error_bit = std::uintptr_t(1) << (sizeof(std::uintptr_t) * 8 - 1);

		// success
		constexpr std::uintptr_t success = 0;

        // errors
        constexpr std::uintptr_t load_error           = error_bit | 1;
        constexpr std::uintptr_t invalid_parameter    = error_bit | 2;
        constexpr std::uintptr_t unsupported          = error_bit | 3;
        constexpr std::uintptr_t bad_buffer_size      = error_bit | 4;
        constexpr std::uintptr_t buffer_too_small     = error_bit | 5;
        constexpr std::uintptr_t not_ready            = error_bit | 6;
        constexpr std::uintptr_t device_error         = error_bit | 7;
        constexpr std::uintptr_t write_protected      = error_bit | 8;
        constexpr std::uintptr_t out_of_resources     = error_bit | 9;
        constexpr std::uintptr_t volume_corrupted     = error_bit | 10;
        constexpr std::uintptr_t volume_full          = error_bit | 11;
        constexpr std::uintptr_t no_media             = error_bit | 12;
        constexpr std::uintptr_t media_changed        = error_bit | 13;
        constexpr std::uintptr_t not_found            = error_bit | 14;
        constexpr std::uintptr_t access_denied        = error_bit | 15;
        constexpr std::uintptr_t no_response          = error_bit | 16;
        constexpr std::uintptr_t no_mapping           = error_bit | 17;
        constexpr std::uintptr_t timeout              = error_bit | 18;
        constexpr std::uintptr_t not_started          = error_bit | 19;
        constexpr std::uintptr_t already_started      = error_bit | 20;
        constexpr std::uintptr_t aborted              = error_bit | 21;
        constexpr std::uintptr_t icmp_error           = error_bit | 22;
        constexpr std::uintptr_t tftp_error           = error_bit | 23;
        constexpr std::uintptr_t protocol_error       = error_bit | 24;
        constexpr std::uintptr_t incompatible_version = error_bit | 25;
        constexpr std::uintptr_t security_violation   = error_bit | 26;
        constexpr std::uintptr_t crc_error            = error_bit | 27;
        constexpr std::uintptr_t end_of_media         = error_bit | 28;
        constexpr std::uintptr_t end_of_file          = error_bit | 31;
        constexpr std::uintptr_t invalid_language     = error_bit | 32;
        constexpr std::uintptr_t compromised_data     = error_bit | 33;
        constexpr std::uintptr_t ip_address_conflict  = error_bit | 34;
        constexpr std::uintptr_t http_error           = error_bit | 35;

        // warnings
        constexpr std::uintptr_t warn_unknown_glyph    = 1;
        constexpr std::uintptr_t warn_delete_failure   = 2;
        constexpr std::uintptr_t warn_write_failure    = 3;
        constexpr std::uintptr_t warn_buffer_too_small = 4;
        constexpr std::uintptr_t warn_stale_data       = 5;
        constexpr std::uintptr_t warn_file_system      = 6;
        constexpr std::uintptr_t warn_reset_required   = 7;
	}

	struct status {
		using value_type = std::uintptr_t;

		constexpr status() noexcept
			: m_value(0) { }

		constexpr status(const value_type value) noexcept
			: m_value(value) { }

        [[nodiscard]] constexpr value_type value() const noexcept {
            return m_value;
        }

        // fail && error
        [[nodiscard]] constexpr bool succeeded() const noexcept { return (m_value & status_codes::error_bit) == 0; }
        [[nodiscard]] constexpr bool failed() const noexcept { return (m_value & status_codes::error_bit) != 0; }

        // warning
        [[nodiscard]] constexpr bool is_warning() const noexcept { return !failed() && m_value != 0; }

		constexpr operator std::uintptr_t() const noexcept { return m_value; }

        constexpr bool operator==(const status& s) const noexcept { return m_value == s.m_value; }
        constexpr bool operator!=(const status& s) const noexcept { return m_value != s.m_value; }

    private:
		value_type m_value;
	};

}
using efi_status = efi::status;
using efi_status_t = std::uintptr_t;

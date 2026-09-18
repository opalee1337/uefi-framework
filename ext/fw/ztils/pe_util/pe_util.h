#pragma once

#include <cstdint>
#include "fw/core/stl/types/span.h"

namespace ztils {

	namespace pe {

		enum : std::uint16_t {
			dos_magic = 0x5A4D,
			pe64_magic = 0x020B,
		};

		enum : std::uint32_t {
			nt_magic = 0x00004550,
		};

		enum directory : std::uint32_t {
			dir_export = 0,
			dir_import = 1,
			dir_resource = 2,
			dir_exception = 3,
			dir_security = 4,
			dir_basereloc = 5,
			dir_debug = 6,
			dir_tls = 9,
			dir_load_config = 10,
			dir_iat = 12,
		};

		enum section_flags : std::uint32_t {
			scn_code = 0x00000020,
			scn_initialized = 0x00000040,
			scn_uninitialized = 0x00000080,
			scn_execute = 0x20000000,
			scn_read = 0x40000000,
			scn_write = 0x80000000,
		};

		enum reloc_type : std::uint16_t {
			rel_absolute = 0,
			rel_highlow = 3,
			rel_dir64 = 10,
		};

		struct dos_header {
			std::uint16_t e_magic;
			std::uint16_t e_cblp;
			std::uint16_t e_cp;
			std::uint16_t e_crlc;
			std::uint16_t e_cparhdr;
			std::uint16_t e_minalloc;
			std::uint16_t e_maxalloc;
			std::uint16_t e_ss;
			std::uint16_t e_sp;
			std::uint16_t e_csum;
			std::uint16_t e_ip;
			std::uint16_t e_cs;
			std::uint16_t e_lfarlc;
			std::uint16_t e_ovno;
			std::uint16_t e_res[4];
			std::uint16_t e_oemid;
			std::uint16_t e_oeminfo;
			std::uint16_t e_res2[10];
			std::int32_t  e_lfanew;
		};

        struct file_header {
            std::uint16_t machine;
            std::uint16_t number_of_sections;
            std::uint32_t time_date_stamp;
            std::uint32_t pointer_to_symbol_table;
            std::uint32_t number_of_symbols;
            std::uint16_t size_of_optional_header;
            std::uint16_t characteristics;
        };

        struct data_directory {
            std::uint32_t virtual_address;
            std::uint32_t size;
        };

        struct optional_header64 {
            std::uint16_t    magic;
            std::uint8_t     major_linker_version;
            std::uint8_t     minor_linker_version;
            std::uint32_t    size_of_code;
            std::uint32_t    size_of_initialized_data;
            std::uint32_t    size_of_uninitialized_data;
            std::uint32_t    address_of_entry_point;
            std::uint32_t    base_of_code;
            std::uint64_t    image_base;
            std::uint32_t    section_alignment;
            std::uint32_t    file_alignment;
            std::uint16_t    major_os_version;
            std::uint16_t    minor_os_version;
            std::uint16_t    major_image_version;
            std::uint16_t    minor_image_version;
            std::uint16_t    major_subsystem_version;
            std::uint16_t    minor_subsystem_version;
            std::uint32_t    win32_version_value;
            std::uint32_t    size_of_image;
            std::uint32_t    size_of_headers;
            std::uint32_t    check_sum;
            std::uint16_t    subsystem;
            std::uint16_t    dll_characteristics;
            std::uint64_t    size_of_stack_reserve;
            std::uint64_t    size_of_stack_commit;
            std::uint64_t    size_of_heap_reserve;
            std::uint64_t    size_of_heap_commit;
            std::uint32_t    loader_flags;
            std::uint32_t    number_of_rva_and_sizes;
            data_directory   data_directory[16];
        };

        struct nt_headers64 {
            std::uint32_t     signature;
            file_header       file_header;
            optional_header64 optional_header;
        };

        struct section_header {
            std::uint8_t  name[8];
            std::uint32_t virtual_size;
            std::uint32_t virtual_address;
            std::uint32_t size_of_raw_data;
            std::uint32_t pointer_to_raw_data;
            std::uint32_t pointer_to_relocations;
            std::uint32_t pointer_to_linenumbers;
            std::uint16_t number_of_relocations;
            std::uint16_t number_of_linenumbers;
            std::uint32_t characteristics;
        };

        struct base_relocation {
            std::uint32_t virtual_address;
            std::uint32_t size_of_block;
        };

        struct export_directory {
            std::uint32_t characteristics;
            std::uint32_t time_date_stamp;
            std::uint16_t major_version;
            std::uint16_t minor_version;
            std::uint32_t name;
            std::uint32_t base;
            std::uint32_t number_of_functions;
            std::uint32_t number_of_names;
            std::uint32_t address_of_functions;
            std::uint32_t address_of_names;
            std::uint32_t address_of_name_ordinals;
        };

	}

    bool pe_valid(void* base);

    // helpers

    pe::nt_headers64* nt_headers(void* base);

    pe::section_header* first_section(pe::nt_headers64* nt);
    pe::section_header* first_section(void* base);

    template <typename T = void>
    T* rva_to_ptr(void* base, std::uint32_t rva) {
        return reinterpret_cast<T*>(reinterpret_cast<std::uint8_t*>(base) + rva);
    }

    // image properties
    std::uint32_t size_of_image(void* base);
    std::uint32_t size_of_headers(void* base);
    std::uint32_t entry_rva(void* base);
    std::uint64_t preferred_base(void* base);
    std::uint16_t num_sections(void* base);

    // section

    stl::span<std::uint8_t> get_section(void* base, const char* name);

    pe::section_header* find_section(void* base, const char* name);
    stl::span<pe::section_header> sections(void* base);

    // exports

    void* get_export(void* base, const char* name);
    void* get_export(void* base, std::uint16_t ordinal);

    void* find_export_containing(void* base, const char* substr);

    // file layout -> virtual layout

    bool map_to(void* src, std::uint8_t* dst, std::uint32_t dst_size);

    // relocation

    void apply_relocs(void* base, std::int64_t delta);
}
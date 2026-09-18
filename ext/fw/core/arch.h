#pragma once

#include <cstdint>
#include <intrin.h>

#if defined(_MSC_EXTENSIONS)
#pragma warning(push)
#pragma warning(disable: 4201)
#endif

namespace arch {

    union cr0 {
        struct {
            std::uint64_t protection_enable : 1;
            std::uint64_t monitor_coprocessor : 1;
            std::uint64_t emulate_fpu : 1;
            std::uint64_t task_switched : 1;
            std::uint64_t extension_type : 1;
            std::uint64_t numeric_error : 1;
            std::uint64_t reserved_1 : 10;
            std::uint64_t write_protect : 1;
            std::uint64_t reserved_2 : 1;
            std::uint64_t alignment_mask : 1;
            std::uint64_t reserved_3 : 10;
            std::uint64_t not_write_through : 1;
            std::uint64_t cache_disable : 1;
            std::uint64_t paging_enable : 1;
        };

        std::uint64_t flags;
    };

    union cr3 {
        struct {
            std::uint64_t reserved_1 : 3;
            std::uint64_t page_level_write_through : 1;
            std::uint64_t page_level_cache_disable : 1;
            std::uint64_t reserved_2 : 7;
            std::uint64_t address_of_page_directory : 36;
        };

        std::uint64_t flags;
    };

    union pml4e_64 {
        struct {
            std::uint64_t present : 1;
            std::uint64_t write : 1;
            std::uint64_t supervisor : 1;
            std::uint64_t page_level_write_through : 1;
            std::uint64_t page_level_cache_disable : 1;
            std::uint64_t accessed : 1;
            std::uint64_t reserved_1 : 1;
            std::uint64_t must_be_zero : 1;
            std::uint64_t ignored_1 : 3;
            std::uint64_t restart : 1;
            std::uint64_t page_frame_number : 36;
            std::uint64_t reserved_2 : 4;
            std::uint64_t ignored_2 : 11;
            std::uint64_t execute_disable : 1;
        };

        std::uint64_t flags;
    };

    union pdpte_1gb_64 {
        struct {
            std::uint64_t present : 1;
            std::uint64_t write : 1;
            std::uint64_t supervisor : 1;
            std::uint64_t page_level_write_through : 1;
            std::uint64_t page_level_cache_disable : 1;
            std::uint64_t accessed : 1;
            std::uint64_t dirty : 1;
            std::uint64_t large_page : 1;
            std::uint64_t global : 1;
            std::uint64_t ignored_1 : 2;
            std::uint64_t restart : 1;
            std::uint64_t pat : 1;
            std::uint64_t reserved_1 : 17;
            std::uint64_t page_frame_number : 18;
            std::uint64_t reserved_2 : 4;
            std::uint64_t ignored_2 : 7;
            std::uint64_t protection_key : 4;
            std::uint64_t execute_disable : 1;
        };

        std::uint64_t flags;
    };

    union pdpte_64 {
        struct {
            std::uint64_t present : 1;
            std::uint64_t write : 1;
            std::uint64_t supervisor : 1;
            std::uint64_t page_level_write_through : 1;
            std::uint64_t page_level_cache_disable : 1;
            std::uint64_t accessed : 1;
            std::uint64_t reserved_1 : 1;
            std::uint64_t large_page : 1;
            std::uint64_t ignored_1 : 3;
            std::uint64_t restart : 1;
            std::uint64_t page_frame_number : 36;
            std::uint64_t reserved_2 : 4;
            std::uint64_t ignored_2 : 11;
            std::uint64_t execute_disable : 1;
        };

        std::uint64_t flags;
    };

    union pde_64 {
        struct {
            std::uint64_t present : 1;
            std::uint64_t write : 1;
            std::uint64_t supervisor : 1;
            std::uint64_t page_level_write_through : 1;
            std::uint64_t page_level_cache_disable : 1;
            std::uint64_t accessed : 1;
            std::uint64_t reserved_1 : 1;
            std::uint64_t large_page : 1;
            std::uint64_t ignored_1 : 3;
            std::uint64_t restart : 1;
            std::uint64_t page_frame_number : 36;
            std::uint64_t reserved_2 : 4;
            std::uint64_t ignored_2 : 11;
            std::uint64_t execute_disable : 1;
        };

        std::uint64_t flags;
    };

    union pte_64 {
        struct {
            std::uint64_t present : 1;
            std::uint64_t write : 1;
            std::uint64_t supervisor : 1;
            std::uint64_t page_level_write_through : 1;
            std::uint64_t page_level_cache_disable : 1;
            std::uint64_t accessed : 1;
            std::uint64_t dirty : 1;
            std::uint64_t pat : 1;
            std::uint64_t global : 1;
            std::uint64_t ignored_1 : 2;
            std::uint64_t restart : 1;
            std::uint64_t page_frame_number : 36;
            std::uint64_t reserved_1 : 4;
            std::uint64_t ignored_2 : 7;
            std::uint64_t protection_key : 4;
            std::uint64_t execute_disable : 1;
        };

        std::uint64_t flags;
    };

    union virtual_address {
        std::uint64_t address;

        struct {
            std::uint64_t offset : 12;
            std::uint64_t pt_idx : 9;
            std::uint64_t pd_idx : 9;
            std::uint64_t pdpt_idx : 9;
            std::uint64_t pml4_idx : 9;
            std::uint64_t reserved : 16;
        };
    };

    // read cr3 && cr0
    inline cr0 read_cr0() { return cr0{ .flags = __readcr0() }; }
    inline cr3 read_cr3() { return cr3{ .flags = __readcr3() }; }

    // write cr3 && cr0
    inline void write_cr0(cr0 cr0) { __writecr0(cr0.flags); }
    inline void write_cr3(cr3 cr3) { __writecr3(cr3.flags); }

    // disable write protection
    inline void disable_write_protection(cr0 original_cr0) {
        cr0 new_cr0 = original_cr0;
        new_cr0.write_protect = 0;

        write_cr0(new_cr0);
    }
}

#if defined(_MSC_EXTENSIONS)
#pragma warning(pop)
#endif

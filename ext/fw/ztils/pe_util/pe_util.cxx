#include "pe_util.h"
#include "fw/core/crt/memory/memory.h"

//#include <Zydis/Zydis.h>

using namespace ztils::pe;

namespace {

    bool str_eq(const char* a, const char* b) {
        while (*a && *b && *a == *b) { ++a; ++b; }
        return *a == *b;
    }

    bool str_contains(const char* haystack, const char* needle) {
        for (const char* h = haystack; *h; ++h) {
            const char* hp = h;
            const char* np = needle;
            while (*hp && *np && *hp == *np) { ++hp; ++np; }
            if (*np == '\0') return true;
        }
        return false;
    }

    bool sec_name_eq(const std::uint8_t(&sec)[8], const char* name) {
        for (int i = 0; i < 8; ++i) {
            if (static_cast<char>(sec[i]) != name[i]) return false;
            if (!name[i])                              return true;
        }
        return true;
    }

    bool rva_in_bounds(std::uint32_t image_size, std::uint32_t rva, std::uint32_t len = 1) {
        if (!rva) return false;
        if (rva >= image_size) return false;
        if (len && (image_size - rva) < len) return false;
        return true;
    }

}

namespace ztils {

    pe::nt_headers64* nt_headers(void* base) {
        if (!base) return nullptr;

        auto* dos = reinterpret_cast<dos_header*>(base);
        if (dos->e_magic != dos_magic) return nullptr;

        if (dos->e_lfanew <= 0) return nullptr;
        auto nt_off = static_cast<std::uint32_t>(dos->e_lfanew);

        auto* nt = reinterpret_cast<pe::nt_headers64*>(
            reinterpret_cast<std::uint8_t*>(base) + nt_off);

        if (nt->signature != nt_magic)                 return nullptr;
        if (nt->optional_header.magic != pe64_magic)   return nullptr;
        if (nt->optional_header.size_of_image == 0)    return nullptr;

        return nt;
    }

    bool pe_valid(void* base) {
        return nt_headers(base) != nullptr;
    }

    pe::section_header* first_section(pe::nt_headers64* nt) {
        if (!nt) return nullptr;
        return reinterpret_cast<section_header*>(
            reinterpret_cast<std::uint8_t*>(&nt->optional_header)
            + nt->file_header.size_of_optional_header);
    }

    pe::section_header* first_section(void* base) {
        return first_section(nt_headers(base)); // was nt_hdrs() — typo fixed
    }

    // image

    std::uint32_t size_of_image(void* base) {
        auto* nt = nt_headers(base);
        return nt ? nt->optional_header.size_of_image : 0;
    }

    std::uint32_t size_of_headers(void* base) {
        auto* nt = nt_headers(base);
        return nt ? nt->optional_header.size_of_headers : 0;
    }

    std::uint32_t entry_rva(void* base) {
        auto* nt = nt_headers(base);
        return nt ? nt->optional_header.address_of_entry_point : 0;
    }

    std::uint64_t preferred_base(void* base) {
        auto* nt = nt_headers(base);
        return nt ? nt->optional_header.image_base : 0;
    }

    std::uint16_t num_sections(void* base) {
        auto* nt = nt_headers(base);
        return nt ? nt->file_header.number_of_sections : 0;
    }

    // section

    stl::span<pe::section_header> sections(void* base) {
        auto* nt = nt_headers(base);
        if (!nt) return {};
        auto* sec = first_section(nt);
        return { sec, nt->file_header.number_of_sections };
    }

    pe::section_header* find_section(void* base, const char* name) {
        if (!name) return nullptr;
        auto* nt = nt_headers(base);
        if (!nt)   return nullptr;

        auto* sec = first_section(nt);
        for (std::uint16_t i = 0; i < nt->file_header.number_of_sections; ++i) {
            if (sec_name_eq(sec[i].name, name))
                return &sec[i];
        }
        return nullptr;
    }

    stl::span<std::uint8_t> get_section(void* base, const char* name) {
        auto* sec = find_section(base, name);
        if (!sec) return {};

        auto* nt = nt_headers(base);
        std::uint32_t soi = nt->optional_header.size_of_image;

        if (!rva_in_bounds(soi, sec->virtual_address, sec->virtual_size))
            return {};

        auto* ptr = reinterpret_cast<std::uint8_t*>(base) + sec->virtual_address;
        return { ptr, sec->virtual_size };
    }

    // exports

    void* get_export(void* base, const char* name) {
        if (!name) return nullptr;

        auto* nt = nt_headers(base);
        if (!nt) return nullptr;

        std::uint32_t soi = nt->optional_header.size_of_image;

        auto& dir = nt->optional_header.data_directory[dir_export];
        if (!rva_in_bounds(soi, dir.virtual_address, sizeof(export_directory)))
            return nullptr;

        auto* exp = rva_to_ptr<export_directory>(base, dir.virtual_address);

        if (!rva_in_bounds(soi, exp->address_of_names, exp->number_of_names * sizeof(std::uint32_t))) return nullptr;
        if (!rva_in_bounds(soi, exp->address_of_name_ordinals, exp->number_of_names * sizeof(std::uint16_t))) return nullptr;
        if (!rva_in_bounds(soi, exp->address_of_functions, exp->number_of_functions * sizeof(std::uint32_t))) return nullptr;

        auto* names = rva_to_ptr<std::uint32_t>(base, exp->address_of_names);
        auto* ordinals = rva_to_ptr<std::uint16_t>(base, exp->address_of_name_ordinals);
        auto* funcs = rva_to_ptr<std::uint32_t>(base, exp->address_of_functions);

        for (std::uint32_t i = 0; i < exp->number_of_names; ++i) {
            if (!rva_in_bounds(soi, names[i])) continue;

            const char* fn_name = rva_to_ptr<char>(base, names[i]);
            if (!str_eq(fn_name, name)) continue;

            std::uint16_t ord = ordinals[i];
            if (ord >= exp->number_of_functions) continue;

            std::uint32_t fn_rva = funcs[ord];
            if (!rva_in_bounds(soi, fn_rva)) continue;

            return rva_to_ptr<void>(base, fn_rva);
        }

        return nullptr;
    }

    void* get_export(void* base, std::uint16_t ordinal) {
        auto* nt = nt_headers(base);
        if (!nt) return nullptr;

        std::uint32_t soi = nt->optional_header.size_of_image;

        auto& dir = nt->optional_header.data_directory[dir_export];
        if (!rva_in_bounds(soi, dir.virtual_address, sizeof(export_directory)))
            return nullptr;

        auto* exp = rva_to_ptr<export_directory>(base, dir.virtual_address);

        if (!rva_in_bounds(soi, exp->address_of_functions, exp->number_of_functions * sizeof(std::uint32_t)))
            return nullptr;

        auto* funcs = rva_to_ptr<std::uint32_t>(base, exp->address_of_functions);

        std::uint32_t idx = static_cast<std::uint32_t>(ordinal) - exp->base;
        if (idx >= exp->number_of_functions) return nullptr;

        std::uint32_t fn_rva = funcs[idx];
        if (!rva_in_bounds(soi, fn_rva)) return nullptr;

        return rva_to_ptr<void>(base, fn_rva);
    }

    void* find_export_containing(void* base, const char* substr) {
        if (!substr) return nullptr;

        auto* nt = nt_headers(base);
        if (!nt) return nullptr;

        std::uint32_t soi = nt->optional_header.size_of_image;

        auto& dir = nt->optional_header.data_directory[dir_export];
        if (!rva_in_bounds(soi, dir.virtual_address, sizeof(export_directory)))
            return nullptr;

        auto* exp = rva_to_ptr<export_directory>(base, dir.virtual_address);

        if (!rva_in_bounds(soi, exp->address_of_names, exp->number_of_names * sizeof(std::uint32_t))) return nullptr;
        if (!rva_in_bounds(soi, exp->address_of_name_ordinals, exp->number_of_names * sizeof(std::uint16_t))) return nullptr;
        if (!rva_in_bounds(soi, exp->address_of_functions, exp->number_of_functions * sizeof(std::uint32_t))) return nullptr;

        auto* names = rva_to_ptr<std::uint32_t>(base, exp->address_of_names);
        auto* ordinals = rva_to_ptr<std::uint16_t>(base, exp->address_of_name_ordinals);
        auto* funcs = rva_to_ptr<std::uint32_t>(base, exp->address_of_functions);

        for (std::uint32_t i = 0; i < exp->number_of_names; ++i) {
            if (!rva_in_bounds(soi, names[i])) continue;

            const char* fn_name = rva_to_ptr<char>(base, names[i]);
            if (!str_contains(fn_name, substr)) continue;

            std::uint16_t ord = ordinals[i];
            if (ord >= exp->number_of_functions) continue;

            std::uint32_t fn_rva = funcs[ord];
            if (!rva_in_bounds(soi, fn_rva)) continue;

            return rva_to_ptr<void>(base, fn_rva);
        }

        return nullptr;
    }

    // section mapping

    bool map_to(void* src, std::uint8_t* dst, std::uint32_t dst_size) {
        auto* nt = nt_headers(src);
        if (!nt || !dst) return false;

        std::uint32_t soi = nt->optional_header.size_of_image;
        if (dst_size < soi) return false;

        crt::memset(dst, 0, soi);
        crt::memcpy(dst, src, nt->optional_header.size_of_headers);

        auto* sec = first_section(nt);
        for (std::uint16_t i = 0; i < nt->file_header.number_of_sections; ++i) {
            if (!sec[i].size_of_raw_data || !sec[i].pointer_to_raw_data) continue;
            if (sec[i].virtual_address >= soi) return false;

            std::uint32_t copy_size = sec[i].size_of_raw_data;
            if (sec[i].virtual_size && sec[i].virtual_size < copy_size)
                copy_size = sec[i].virtual_size;
            if (sec[i].virtual_address + copy_size > soi)
                copy_size = soi - sec[i].virtual_address;

            crt::memcpy(
                dst + sec[i].virtual_address,
                reinterpret_cast<std::uint8_t*>(src) + sec[i].pointer_to_raw_data,
                copy_size);
        }

        return true;
    }

    // relocations

    void apply_relocs(void* base, std::int64_t delta) {
        if (!base || delta == 0) return;

        auto* nt = nt_headers(base);
        if (!nt) return;

        std::uint32_t soi = nt->optional_header.size_of_image;

        auto& dir = nt->optional_header.data_directory[dir_basereloc];
        if (!dir.virtual_address || !dir.size) return;
        if (!rva_in_bounds(soi, dir.virtual_address, dir.size)) return;

        auto* block = rva_to_ptr<base_relocation>(base, dir.virtual_address);
        auto* end = reinterpret_cast<std::uint8_t*>(block) + dir.size;

        while (reinterpret_cast<std::uint8_t*>(block) < end && block->size_of_block >= sizeof(base_relocation)) {
            std::uint32_t entry_count = (block->size_of_block - static_cast<std::uint32_t>(sizeof(base_relocation))) / sizeof(std::uint16_t);

            auto* entries = reinterpret_cast<std::uint16_t*>(block + 1);
            for (std::uint32_t i = 0; i < entry_count; ++i) {
                const std::uint16_t type = entries[i] >> 12;
                const std::uint16_t offset = entries[i] & 0x0FFF;

                if (type != rel_dir64) continue;

                std::uint32_t target_rva = block->virtual_address + offset;
                if (!rva_in_bounds(soi, target_rva, sizeof(std::uint64_t))) continue;

                *rva_to_ptr<std::uint64_t>(base, target_rva) += static_cast<std::uint64_t>(delta);
            }

            block = reinterpret_cast<base_relocation*>(reinterpret_cast<std::uint8_t*>(block) + block->size_of_block);
        }
    }

    /* uncomment if you added zydis
    void* resolve_thunk(void* ptr, std::uint32_t max_insns) {
        if (!ptr) return nullptr;

        ZydisDecoder dec;
        ZydisDecoderInit(&dec, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);

        ZydisDecodedInstruction insn;
        ZydisDecodedOperand     ops[ZYDIS_MAX_OPERAND_COUNT];

        auto* ip = reinterpret_cast<std::uint8_t*>(ptr);

        for (std::uint32_t i = 0; i < max_insns; ++i) {
            if (!ZYAN_SUCCESS(ZydisDecoderDecodeFull(&dec, ip, 15, &insn, ops)))
                break;

            if (insn.mnemonic == ZYDIS_MNEMONIC_JMP ||
                insn.mnemonic == ZYDIS_MNEMONIC_CALL)
            {
                // jmp rel8/rel32
                if (ops[0].type == ZYDIS_OPERAND_TYPE_IMMEDIATE && ops[0].imm.is_relative) {
                    ip = ip + insn.length + static_cast<std::int64_t>(ops[0].imm.value.s);
                    continue;
                }

                // jmp [rip + disp]
                if (ops[0].type == ZYDIS_OPERAND_TYPE_MEMORY &&
                    ops[0].mem.base == ZYDIS_REGISTER_RIP)
                {
                    auto* slot = reinterpret_cast<void**>(
                        ip + insn.length + ops[0].mem.disp.value);
                    return *slot;
                }

                // jmp rax
                break;
            }

            ip += insn.length;
        }

        return reinterpret_cast<void*>(ip);
    }

    void* resolve_rip_rel(void* insn_ptr) {
        if (!insn_ptr) return nullptr;

        ZydisDecoder dec;
        ZydisDecoderInit(&dec, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);

        ZydisDecodedInstruction insn;
        ZydisDecodedOperand     ops[ZYDIS_MAX_OPERAND_COUNT];

        auto* ip = reinterpret_cast<std::uint8_t*>(insn_ptr);
        if (!ZYAN_SUCCESS(ZydisDecoderDecodeFull(&dec, ip, 15, &insn, ops)))
            return nullptr;

        for (std::uint8_t o = 0; o < insn.operand_count; ++o) {
            if (ops[o].type == ZYDIS_OPERAND_TYPE_MEMORY &&
                ops[o].mem.base == ZYDIS_REGISTER_RIP &&
                ops[o].mem.disp.has_displacement)
            {
                return reinterpret_cast<void*>(
                    ip + insn.length + ops[o].mem.disp.value);
            }
        }

        return nullptr;
    }
    */
}
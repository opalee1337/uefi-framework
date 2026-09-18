# uefi-framework

A lightweight C++ framework for writing UEFI applications and bootkit-style tools from scratch. It wraps the UEFI spec into straightforward C++ without pulling in EDK2 or any other heavy dependency. It includes EFI services, protocols, architecture utilities, PE parsing, pattern scanning, detour hooking, and serial output.

---

## What this is

Most UEFI development toolkits are either large (EDK2), tied to a specific build system, or designed around standard EFI applications instead of low-level work. This framework provides:

- Full typed bindings to the UEFI 2.11 spec (boot services, runtime services, protocols)
- A hand-rolled CRT so it does not depend on the MSVC runtime in pre-boot
- An STL subset (string_view, span, optional, type_traits) that works without the standard library
- x64 paging structure definitions (PML4E, PDPTE, PDE, PTE) and CR0/CR3 read/write helpers
- PE64 parsing utilities (section lookup, export resolution, relocation application, image mapping)
- An IDA-style byte pattern scanner
- A 14-byte detour hook class (no trampolines needed)
- A serial port logger (COM1 by default) with a printf-style API

The project builds as a UEFI application using Visual Studio. The solution file and .vcxproj are already configured.

---

## Project layout

```
update library/
+-- source/
|   +-- entry.cxx                    # EFI entry point, start here
+-- ext/
    +-- fw/
        +-- fw.h                     # master include for all EFI bindings
        +-- context.h                # global context object (image handle + system table)
        |
        +-- efi/
        |   +-- abi.h                # EFIAPI calling convention, handle/event typedefs
        |   +-- guid.h               # EFI_GUID struct
        |   +-- status.h             # EFI status codes and status wrapper type
        |   +-- service/
        |   |   +-- boot_services.h  # full EFI boot services table
        |   |   +-- runtime_services.h
        |   |   +-- system_table.h
        |   +-- protocol/
        |       +-- device_path.h
        |       +-- file_system.h
        |       +-- loaded_image.h
        |       +-- io/
        |           +-- graphics_output.h
        |           +-- simple_text_input.h
        |           +-- simple_text_output.h
        |
        +-- core/
        |   +-- arch.h               # x64 paging structures, CR0/CR3 helpers
        |   +-- crt.h                # aggregate CRT include
        |   +-- stl.h                # aggregate STL include
        |   +-- crt/
        |   |   +-- memory/
        |   |   |   +-- memory.h     # memcpy/memmove/memset/memcmp/memzero/memchr
        |   |   |   +-- memory.asm   # assembly implementations
        |   |   +-- string.h         # strlen, strcmp, strncmp, etc.
        |   |   +-- new.h            # placement new
        |   +-- stl/
        |       +-- meta/
        |       |   +-- type_traits.h
        |       +-- string/
        |       |   +-- string_view.h  # char and char16_t string_view
        |       +-- types/
        |       |   +-- optional.h
        |       |   +-- span.h
        |       +-- utility/
        |           +-- utility.h
        |
        +-- ztils/
            +-- detour_hook/
            |   +-- detour_hook.h    # c_detour_hook class
            |   +-- detour_hook.cxx
            +-- pattern_scan/
            |   +-- pattern_scan.h   # ztils::scan and scan_as
            |   +-- pattern_scan.cxx
            +-- pe_util/
            |   +-- pe_util.h        # PE64 helpers
            |   +-- pe_util.cxx
            +-- serial/
                +-- serial.h         # c_serial / g_serial
                +-- serial.cxx
```

---

## Getting started

### Requirements

- Visual Studio 2022 (or 2019 with C++20 support)
- MSVC toolchain (the project uses MSVC intrinsics like `__readcr0`)
- A way to run UEFI binaries, QEMU with OVMF works great for testing

### Build

Open `uefi-framework.sln` in Visual Studio and build. The output is a `.efi` binary targeting the UEFI subsystem.

### Entry point

Your code goes in `source/entry.cxx`. The framework gives you a clean starting point:

```cpp
#include "fw/context.h"

efi_status_t EfiMain(efi_handle image_handle, efi::system_table* system_table) {
    g_ctx.init(image_handle, system_table);

    // your code here

    return efi::status_codes::success;
}
```

`g_ctx` is a global `fw::c_context` instance. After calling `init`, you can access EFI services from anywhere:

```cpp
g_ctx.boot_services()     // efi::boot_services*
g_ctx.runtime_services()  // efi::runtime_services*
g_ctx.con_out()           // efi::simple_text_output*
g_ctx.con_in()            // efi::simple_text_input*
```

---

## Modules

### EFI bindings (`fw/fw.h`)

Including `fw/fw.h` brings in the full set of EFI bindings, status codes, GUIDs, the system/boot/runtime service tables, and all the protocols listed above. Everything lives under the `efi` namespace and follows the UEFI 2.11 spec naming closely, so the official PDF can be used as a reference.

The status type (`efi::status`) is a thin wrapper that gives you `succeeded()`, `failed()`, and `is_warning()` predicates so you do not have to mask the high bit manually.

### CRT (`fw/core/crt.h`)

UEFI runs before the OS runtime, so there is no CRT available. This layer provides the basics:

- `memcpy`, `memmove`, `memset`, `memcmp`, `memzero`, `memchr`, implemented in assembly for correctness at this privilege level
- `strlen`, `strcmp`, `strncmp`, `strcpy`, and friends
- Placement `new` so you can construct objects in allocated memory

The assembly implementations in `memory.asm` are credited to @shmurkio.

### STL subset (`fw/core/stl.h`)

A small selection of standard library types reimplemented to work without the standard library:

- `stl::string_view`, works with both `char` and `char16_t` (EFI uses UTF-16 for console output). Supports `find`, `substr`, `starts_with`, `ends_with`, comparisons, and iteration.
- `stl::span<T>`, non-owning view over a contiguous range. Used throughout the library for passing memory regions around safely.
- `stl::optional<T>`, a value-or-nothing type.
- Type traits: `is_same`, `remove_reference`, `remove_cv`, `is_integral`, `is_pointer`, `conditional`, `enable_if`, `decay`.

### Architecture utilities (`fw/core/arch.h`)

Bitfield unions covering the x64 paging hierarchy so you can read and write paging structures by field name rather than shifting and masking manually:

- `arch::cr0`, full field breakdown including `write_protect`, `paging_enable`, etc.
- `arch::cr3`, page directory base and cache control bits
- `arch::pml4e_64`, `pdpte_1gb_64`, `pdpte_64`, `pde_64`, `pte_64`, complete 64-bit paging entry types
- `arch::virtual_address`, decomposes a 64-bit VA into its PML4/PDPT/PD/PT index fields

Helpers:

```cpp
arch::cr0 cr0 = arch::read_cr0();
arch::disable_write_protection(cr0);   // clears WP bit, lets you write to read-only pages
arch::write_cr0(cr0);                  // restore
```

### PE utilities (`ztils/pe_util`)

A full PE64 parser. This is useful for images already loaded into memory or manually mapped. It provides:

```cpp
// check if a base address is a valid PE image
ztils::pe_valid(base);

// get the NT headers
auto* nt = ztils::nt_headers(base);

// look up a section by name
auto text = ztils::get_section(base, ".text");  // returns stl::span<uint8_t>

// resolve an export by name or ordinal
void* fn = ztils::get_export(base, "ExAllocatePool");

// apply relocations after manually loading at a different base
ztils::apply_relocs(base, delta);

// map a file-layout image to a virtual-layout buffer
ztils::map_to(src, dst, dst_size);
```

The header also defines all the raw PE structures you need (`dos_header`, `nt_headers64`, `section_header`, `export_directory`, `base_relocation`, etc.) under the `ztils::pe` namespace.

### Pattern scanner (`ztils/pattern_scan`)

IDA-style pattern scanning over a memory region. Wildcards are `??`.

```cpp
// scan a raw region
void* match = ztils::scan(base, size, "48 8B 05 ?? ?? ?? ?? 48 85 C0");

// scan a span
void* match = ztils::scan(region, "E8 ?? ?? ?? ?? 48 83 C4 28");

// get back a typed pointer
auto* fn = ztils::scan_as<my_fn_t>(base, size, "40 53 48 83 EC 20");
```

### Detour hook (`ztils/detour_hook`)

A simple 14-byte absolute jump hook. It does not generate a trampoline. It saves the original bytes and overwrites them with a `mov rax, <dst>; jmp rax` stub.

```cpp
// declare hooks somewhere accessible across files
namespace hooks {
    inline c_detour_hook my_hook;
}

// install
hooks::my_hook.make(src_fn, my_replacement);

// toggle
hooks::my_hook.disable();
hooks::my_hook.enable();

// query
void* original = hooks::my_hook.get_src();
```

The destructor calls `disable()` automatically, which also makes stack-allocated hooks clean up automatically.

### Serial logger (`ztils/serial`)

Outputs to a serial port (COM1 at `0x3F8` by default). Useful for debugging before you have a usable console. A global instance `g_serial` is provided.

```cpp
g_serial.println("base: {:x}", reinterpret_cast<std::uintptr_t>(image_base));
g_serial.println("sections: {}", section_count);
g_serial.dump(ptr, 0x40);          // hex dump
g_serial.assert(ptr != nullptr, "ptr must not be null");
```

Format specifier `{:x}` prints integers in hex. Supports `const char*`, `const char16_t*`, `bool`, `int`, `unsigned int`, `uint64_t`, `int64_t`, and `const void*`.

---

## Usage tips

- All the utility modules (`serial`, `pe_util`, `detour_hook`, `pattern_scan`) are commented out in `entry.cxx` by default. Uncomment what you need.
- The CRT and STL headers are also opt-in. Include `fw/core/crt.h` and `fw/core/stl.h` when your code needs them.
- If you are hooking something in a write-protected region, use `arch::disable_write_protection` first and restore CR0 after.
- `efi::status::succeeded()` / `failed()` is the intended way to check return values from EFI calls, avoid comparing raw uintptr_t values directly.

---

## Notes and credits

- Memory functions in `crt/memory/memory.asm` are credited to @shmurkio.
- EFI status codes, service table layouts, and protocol definitions follow the UEFI 2.11 specification. The relevant spec URL is referenced in comments throughout the headers.
- The project targets x86-64 only. The ABI header has stubs for i386 but the rest of the code assumes 64-bit.

---

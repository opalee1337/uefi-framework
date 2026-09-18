#include "detour_hook.h"

#include "fw/core/crt.h"
#include "fw/core/arch.h"

void c_detour_hook::make(void* in_src, void* in_dst) {

    // ff 25 00 00 00 00  absolute indirect jmp
    // [6..13]            64bit destination address
    std::uint8_t stub[14] = {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };

    this->src = in_src;
    this->dst = in_dst;

    // save original bytes
    crt::memcpy(code, src, sizeof code);

    // patch destination into stub
    crt::memcpy(stub + 6, &dst, sizeof dst);
    crt::memcpy(jmp, stub, sizeof jmp);
}

void c_detour_hook::enable() {
    if (m_active || !src) return;

    {
        // disable write protection
        arch::cr0 original_cr0 = arch::read_cr0();
        arch::disable_write_protection(original_cr0);

        // disable hook
        crt::memcpy(src, jmp, sizeof jmp);

        // restore
        arch::write_cr0(original_cr0);
    }

    m_active = true;
}

void c_detour_hook::disable() {
    if (!m_active || !src) return;

    {
        // disable write protection
        arch::cr0 original_cr0 = arch::read_cr0();
        arch::disable_write_protection(original_cr0);

        // disable hook
        crt::memcpy(src, code, sizeof code);

        // restore
        arch::write_cr0(original_cr0);
    }

    m_active = false;
}

c_detour_hook::~c_detour_hook() { disable(); }
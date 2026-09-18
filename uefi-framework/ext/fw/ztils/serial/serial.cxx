#include "serial.h"
#include <intrin.h>

c_serial::c_serial(std::uint16_t port) : m_port(port) {
    __outbyte(m_port + 1, 0x00);
    __outbyte(m_port + 3, 0x80);
    __outbyte(m_port + 0, 0x01);
    __outbyte(m_port + 1, 0x00);
    __outbyte(m_port + 3, 0x03);
    __outbyte(m_port + 2, 0xC7);
    __outbyte(m_port + 4, 0x0B);
}

void c_serial::write_char(char c) {
    while (!(__inbyte(m_port + 5) & 0x20)) {

    }
    __outbyte(m_port, c);
}

void c_serial::write_str(const char* s) {
    if (!s) return;

    for (; *s; ++s)
        write_char(*s);
}

void c_serial::write_uint(unsigned long long val, bool hex) {
    if (hex) {
        write_ptr(reinterpret_cast<const void*>(val));
        return;
    }

    if (val == 0) {
        write_char('0');
        return;
    }

    char buf[32];

    int i = 31;
    buf[i] = '\0';

    while (val > 0) {
        buf[--i] = '0' + (val % 10);
        val /= 10;
    }
    write_str(buf + i);
}

void c_serial::write_sint(long long val, bool hex) {
    if (val < 0) {
        write_char('-');
        val = -val;
    }
    write_uint(static_cast<unsigned long long>(val), hex);
}

void c_serial::write_ptr(const void* val) {
    if (!val) {
        write_str("nullptr");
        return;
    }

    static constexpr const char* digits = "0123456789ABCDEF";

    auto v = reinterpret_cast<std::uintptr_t>(val);

    char buf[19];
    buf[0] = '0'; buf[1] = 'x';
    for (int i = 0; i < 16; ++i)
        buf[2 + i] = digits[(v >> (60 - i * 4)) & 0xF];

    buf[18] = '\0';
    write_str(buf);
}

void c_serial::write_arg(const char* val, fmt_spec) {
    write_str(val ? val : "(null)");
}

void c_serial::write_arg(const char16_t* val, fmt_spec) {
    if (!val) {
        write_str("(null)");
        return;
    }

    for (; *val; ++val)
        write_char(static_cast<char>(*val));
}

void c_serial::write_arg(bool val, fmt_spec) {
    write_str(val ? "true" : "false");
}

void c_serial::write_arg(int val, fmt_spec f) {
    write_sint(val, f.hex);
}

void c_serial::write_arg(unsigned int val, fmt_spec f) {
    write_uint(val, f.hex);
}

void c_serial::write_arg(std::uint64_t val, fmt_spec f) {
    write_uint(val, f.hex);
}

void c_serial::write_arg(std::int64_t val, fmt_spec f) {
    write_sint(val, f.hex);
}

void c_serial::write_arg(const void* val, fmt_spec) {
    write_ptr(val);
}

void c_serial::dump(const void* addr, std::uintptr_t size) {
    static constexpr const char* digits = "0123456789ABCDEF";

    auto* p = static_cast<const std::uint8_t*>(addr);
    for (std::uintptr_t i = 0; i < size; i += 16) {
        write_ptr(p + i);

        write_char(' ');

        for (std::uintptr_t j = 0; j < 16 && i + j < size; ++j) {
            write_char(digits[p[i + j] >> 4]);
            write_char(digits[p[i + j] & 0xF]);
            write_char(' ');
        }

        write_char(' ');

        for (std::uintptr_t j = 0; j < 16 && i + j < size; ++j)
            write_char(p[i + j] >= 0x20 && p[i + j] < 0x7F ? p[i + j] : '.');

        write_char('\r');
        write_char('\n');
    }
}

void c_serial::assert(bool cond, stl::string_view msg) {
    if (cond) return;

    write_str("[assert] ");

    for (std::size_t i = 0; i < msg.size(); ++i)
        write_char(msg[i]);

    write_char('\r');
    write_char('\n');

    __debugbreak();
}

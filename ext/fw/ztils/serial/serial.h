#pragma once

#include <cstdint>
#include "fw/core/stl/string/string_view.h"

struct fmt_spec {
    bool hex = false;
};

class c_serial {
public:
    explicit c_serial(std::uint16_t port = 0x3F8); // com1

    template<typename... Args>
    void print(stl::string_view fmt, Args&&... args) {
        vformat(fmt, args...);
    }

    template<typename... Args>
    void println(stl::string_view fmt, Args&&... args) {
        vformat(fmt, args...);
        write_char('\r');
        write_char('\n');
    }

    void dump(const void* addr, std::uintptr_t size);
    void assert(bool cond, stl::string_view msg);

private:
    std::uint16_t m_port;

    void write_char(char c);
    void write_str(const char* s);
    void write_uint(unsigned long long val, bool hex = false);
    void write_sint(long long val, bool hex = false);
    void write_ptr(const void* val);

    // overloads
    void write_arg(const char* val, fmt_spec f = {});
    void write_arg(const char16_t* val, fmt_spec f = {});

    void write_arg(bool val, fmt_spec f = {});

    void write_arg(int          val, fmt_spec f = {});
    void write_arg(unsigned int val, fmt_spec f = {});

    void write_arg(std::uint64_t val, fmt_spec f = {});
    void write_arg(std::int64_t  val, fmt_spec f = {});

    void write_arg(const void* val, fmt_spec f = {});

    static fmt_spec parse_spec(stl::string_view s) {
        fmt_spec f;
        if (!s.empty() && s[0] == ':') {
            for (std::size_t i = 1; i < s.size(); ++i) {
                if (s[i] == 'x' || s[i] == 'X')
                    f.hex = true;
            }
        }
        return f;
    }

    void vformat(stl::string_view fmt) {
        for (std::size_t i = 0; i < fmt.size(); ++i)
            write_char(fmt[i]);
    }

    template<typename T, typename... Args>
    void vformat(stl::string_view fmt, T& arg, Args&... args) {
        const auto pos = fmt.find('{');
        if (pos == stl::string_view::npos) {
            vformat(fmt);
            return;
        }

        for (std::size_t i = 0; i < pos; ++i)
            write_char(fmt[i]);

        const auto close = fmt.find('}', pos);
        write_arg(arg, parse_spec(fmt.substr(pos + 1, close - pos - 1)));
        vformat(fmt.substr(close + 1), args...);
    }
};

inline c_serial g_serial;
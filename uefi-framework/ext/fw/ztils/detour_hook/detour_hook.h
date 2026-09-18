#pragma once

#include <cstdint>

class c_detour_hook {
public:
	c_detour_hook() = default;
	~c_detour_hook();
	c_detour_hook(const c_detour_hook&) = delete;
	c_detour_hook& operator=(const c_detour_hook&) = delete;

	void make(void* in_src, void* in_dst);
	void disable();
	void enable();

	void* get_src() { return src; };
	void* get_dst() { return dst; };

private:
	std::uint8_t  code[14]{};  // saved original bytes
	std::uint8_t  jmp[14]{};   // jmp stub

	void* dst{};
	void* src{};

	bool m_active{};
};

/* how i like using it all at one place cross file use :)
namespace detour_hooks {
	inline c_detour_hook bootmgfw{};
}
*/
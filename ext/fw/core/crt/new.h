#pragma once

#include <cstddef>

inline void* operator new  (std::size_t, void* p) noexcept { return p; }
inline void* operator new[](std::size_t, void* p) noexcept { return p; }
inline void  operator delete  (void*, void*) noexcept {}
inline void  operator delete[](void*, void*) noexcept {}
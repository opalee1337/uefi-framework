#pragma once

#include "fw/core/stl/meta/type_traits.h"

namespace stl {

    template<typename T>
    constexpr remove_reference_t<T>&& move(T&& t)  {
        return static_cast<remove_reference_t<T>&&>(t);
    }

    template<typename T>
    constexpr T&& forward(remove_reference_t<T>& t)  {
        return static_cast<T&&>(t);
    }

    template<typename T>
    constexpr T&& forward(remove_reference_t<T>&& t)  {
        static_assert(!is_lvalue_reference<T>::value, "Can not forward an rvalue as an lvalue.");
        return static_cast<T&&>(t);
    }

    template<typename T>
    constexpr void swap(T& a, T& b)  {
        T temp = move(a);
        a = move(b);
        b = move(temp);
    }

    template<typename T, typename U = T>
    constexpr T exchange(T& obj, U&& new_val)  {
        T old_val = move(obj);
        obj = forward<U>(new_val);
        return old_val;
    }

}

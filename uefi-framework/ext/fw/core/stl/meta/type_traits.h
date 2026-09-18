#pragma once

namespace stl {

    template<typename T, T v>
    struct integral_constant {
        static constexpr T value = v;
        using value_type = T;
        using type = integral_constant;

        constexpr operator value_type() const { return value; }
        constexpr value_type operator()() const { return value; }
    };

    template<bool B>
    using bool_constant = integral_constant<bool, B>;

    using true_type = bool_constant<true>;
    using false_type = bool_constant<false>;

    template<typename T, typename U> struct is_same : false_type {};
    template<typename T>             struct is_same<T, T> : true_type {};

    template<typename T>   struct remove_reference { using type = T; };
    template<typename T>   struct remove_reference<T&> { using type = T; };
    template<typename T>   struct remove_reference<T&&> { using type = T; };

    template<typename T>
    using remove_reference_t = typename remove_reference<T>::type;

    template<typename T>   struct remove_const { using type = T; };
    template<typename T>   struct remove_const<const T> { using type = T; };

    template<typename T>   struct remove_volatile { using type = T; };
    template<typename T>   struct remove_volatile<volatile T> { using type = T; };

    template<typename T>
    struct remove_cv {
        using type = typename remove_volatile<typename remove_const<T>::type>::type;
    };

    template<typename T>
    using remove_cv_t = typename remove_cv<T>::type;

    template<bool B, typename T = void> struct enable_if {};
    template<typename T>                struct enable_if<true, T> { using type = T; };

    template<bool B, typename T = void>
    using enable_if_t = typename enable_if<B, T>::type;

    template<bool B, typename T, typename F> struct conditional { using type = T; };
    template<typename T, typename F>         struct conditional<false, T, F> { using type = F; };

    template<bool B, typename T, typename F>
    using conditional_t = typename conditional<B, T, F>::type;

    template<typename T>   struct is_lvalue_reference : false_type {};
    template<typename T>   struct is_lvalue_reference<T&> : true_type {};

}
#ifndef ZTD_HPP_INCLUDED
#define ZTD_HPP_INCLUDED

#include <type_traits>

namespace ztd
{
    template <typename T>
    struct type_identity
    {
        using type = T;
    };

    enum class Ref
    {
        none,
        lvalue,
        rvalue
    };

    template <typename T, Ref = Ref::lvalue>
    struct add_reference : type_identity<T&>
    {
    };

    template <typename T>
    struct add_reference<T, Ref::rvalue> : type_identity<T&&>
    {
    };

    template <typename T>
    struct add_reference<T, Ref::none> : type_identity<T>
    {
    };

    template <typename T, bool = true>
    struct add_const : type_identity<const T>
    {
    };

    template <typename T>
    struct add_const<T, false> : type_identity<T>
    {
    };

    template <typename T, bool = true>
    struct add_volatile : type_identity<volatile T>
    {
    };

    template <typename T>
    struct add_volatile<T, false> : type_identity<T>
    {
    };

    template <typename T, Ref R = Ref::lvalue>
    using add_reference_t = typename add_reference<T, R>::type;
    template <typename T, bool C = true>
    using add_const_t = typename add_const<T, C>::type;
    template <typename T, bool V = true>
    using add_volatile_t = typename add_volatile<T, V>::type;

    template <typename T, bool C = true, bool V = true>
    struct add_cv : type_identity<add_const_t<add_volatile_t<T, V>, C>>
    {
    };

    template <typename T, bool C = true, bool V = true>
    using add_cv_t = typename add_cv<T, C, V>::type;

    template <typename T, bool C = true, bool V = true, Ref R = Ref::lvalue>
    struct add_cvref : type_identity<add_reference_t<add_cv_t<T, C, V>, R>>
    {
    };

    template <typename T, bool C = true, bool V = true, Ref R = Ref::lvalue>
    using add_cvref_t = typename add_cvref<T, C, V, R>::type;

    template <typename T>
    struct cvref
    {
      private:
        using rT = std::remove_reference_t<T>;

      public:
        static constexpr auto const_value = std::is_const_v<rT>;
        static constexpr auto volatile_value = std::is_volatile_v<rT>;
        static constexpr Ref reference_value =
            Ref(std::is_reference_v<T> * 2 - std::is_lvalue_reference_v<T>);
    };

    template <typename T, typename U>
    struct match_cvref : cvref<T>
    {
      private:
        using base = cvref<T>;
        using rU = std::remove_cv_t<std::remove_reference_t<U>>;
      public:
        using type = add_cvref_t<rU, base::C, base::V, base::R>;
    };

    template <typename T, typename U>
    using match_cvref_t = typename match_cvref<T, U>::type;

    template <typename>
    struct func_traits;

#define DEFINE_FUNC_TRAITS(qs, c, v, r, n)                                     \
    template <typename Ret, typename... Args>                                  \
    struct func_traits<Ret(Args...) qs> : type_identity<Ret(Args...) qs>       \
    {                                                                          \
        using base_type = Ret(Args...);                                        \
        using return_type = Ret;                                               \
        static constexpr bool const_value = c;                                 \
        static constexpr bool volatile_value = v;                              \
        static constexpr Ref reference_value = Ref::r;                         \
        static constexpr bool noexcept_value = n;                              \
    };

#define DEFINE_FUNC_TRAITS2(qs, c, v, r)                                       \
    DEFINE_FUNC_TRAITS(qs, c, v, r, false)                                     \
    DEFINE_FUNC_TRAITS(qs noexcept, c, v, r, true)

    DEFINE_FUNC_TRAITS2(, false, false, none)
    DEFINE_FUNC_TRAITS2(const, true, false, none)
    DEFINE_FUNC_TRAITS2(volatile, false, true, none)
    DEFINE_FUNC_TRAITS2(const volatile, true, true, none)
    DEFINE_FUNC_TRAITS2(const&, true, false, lvalue)
    DEFINE_FUNC_TRAITS2(volatile&, false, true, lvalue)
    DEFINE_FUNC_TRAITS2(const volatile&, true, true, lvalue)
    DEFINE_FUNC_TRAITS2(const&&, true, false, rvalue)
    DEFINE_FUNC_TRAITS2(volatile&&, false, true, rvalue)
    DEFINE_FUNC_TRAITS2(const volatile&&, true, true, rvalue)

#undef DEFINE_FUNC_TRAITS2
#undef DEFINE_FUNC_TRAITS

    template <typename>
    struct mfp_traits;

    template <typename Class, typename Ret, typename... Args>
    struct mfp_traits<Ret (Class::*)(Args...)> : func_traits<Ret(Args...)>
    {
        using class_type = Class;
    };

#define DEFINE_MFP_TRAITS(qs)                                                  \
    template <typename Class, typename Ret, typename... Args>                  \
    struct mfp_traits<Ret (Class::*)(Args...) qs>                              \
        : func_traits<Ret(Args...) qs>                                         \
    {                                                                          \
        using class_type = Class;                                              \
    };

#define DEFINE_MFP_TRAITS2(qs)                                                 \
    DEFINE_MFP_TRAITS(qs)                                                      \
    DEFINE_MFP_TRAITS(qs noexcept)

    DEFINE_MFP_TRAITS2(const)
    DEFINE_MFP_TRAITS2(volatile)
    DEFINE_MFP_TRAITS2(const volatile)
    DEFINE_MFP_TRAITS2(const&)
    DEFINE_MFP_TRAITS2(volatile&)
    DEFINE_MFP_TRAITS2(const volatile&)
    DEFINE_MFP_TRAITS2(const&&)
    DEFINE_MFP_TRAITS2(volatile&&)
    DEFINE_MFP_TRAITS2(const volatile&&)

#undef DEFINE_MFP_TRAITS2
#undef DEFINE_MFP_TRAITS

} // namespace ztd

#endif

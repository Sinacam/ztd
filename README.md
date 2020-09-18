# ztd

Common useful tools not in the C++ standard library.

# Installation

Just include the relevant header files. Requires C++17.

# Usage

## add_reference add_const add_volatile add_cv add_cvref

Has extra parameters specifiying the qualifiers to be added.

````c++
add_reference_t<T, Ref::lvalue>  // T&
add_reference_t<T, Ref::rvalue>  // T&&
add_reference_t<T, Ref::none>    // T
add_const_t<T, true>   // const T
add_const_t<T, false>  // T
add_cvref_t<T, false, true, Ref::rvalue>  // volatile T&&
````

## match_cvref

Given two types `T1 = cv1 U1 ref1` and `T2 = cv2 U2 ref2`, where cv is the const volatile qualifiers and ref is the reference qualifiers,
`match_cvref_t<T1, T2>` is `cv1 U2 ref1`.

## func_traits mfp_traits

`func_traits` and `mfp_traits` are traits for functions and member function pointers.
Given a function or a pointer to member function with the signature `R(Args...) cv ref nx`,
where cv is the const volatile qualifiers, ref is the reference qualifier and nx is the exception specification, it has the members

* `base_type`: `R(Args...)`
* `return_type`: `R`
* `const_value`, `volatile_value`, `reference_value`, `noexcept_value`: the values of the qualifiers.
* `class_type`: Only for `mfp_traits`, the class of the member.

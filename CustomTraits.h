#pragma once

template <bool val, typename IfTrue, typename IfFalse>
struct Conditional;

template <typename IfTrue, typename IfFalse>
struct Conditional<true, IfTrue, IfFalse> {
    using type = IfTrue;
};

template <typename IfTrue, typename IfFalse>
struct Conditional<false, IfTrue, IfFalse> {
    using type = IfFalse;
};

template <bool val, typename IfTrue, typename IfFalse>
using ConditionalT = typename Conditional<val, IfTrue, IfFalse>::type;

template <bool val>
struct EnableIf {};

template <>
struct EnableIf<true> {
    using type = char;
};

template <bool val>
using EnableIfT = typename EnableIf<val>::type;

template <typename T>
struct RemoveReference {
    using type = T;
};

template <typename T>
struct RemoveReference<T&> {
    using type = T;
};

template <typename T>
struct RemoveReference<T&&> {
    using type = T;
};

template <typename T>
using RemoveReferenceT = typename RemoveReference<T>::type;

template <typename T>
struct RemoveConst {
    using type = T;
};

template <typename T>
struct RemoveConst<const T> {
    using type = T;
};

template <typename T>
struct RemoveConst<const T&> {
    using type = T&;
};

template <typename T>
struct RemoveConst<const T&&> {
    using type = T;
};

template <typename T>
using RemoveConstT = typename RemoveConst<T>::type;

template <typename T>
struct RemoveVolatile {
    using type = T;
};

template <typename T>
struct RemoveVolatile<volatile T> {
    using type = T;
};

template <typename T>
using RemoveVolatileT = typename RemoveVolatile<T>::type;

template <typename T>
using RemoveCV = RemoveVolatile<RemoveConstT<T>>;

template <typename T>
using RemoveCVT = typename RemoveCV<T>::type;

template <typename T>
using ClearType = RemoveReference<RemoveCVT<T>>;

template <typename T>
using ClearTypeT = typename ClearType<T>::type;

template <typename T, T a>
struct ValueHolder {
    static constexpr T value = a;
};

template <typename T, typename U>
struct IsSame {
    static constexpr bool value = false;
};

template <typename T>
struct IsSame<T, T> {
    static constexpr bool value = true;
};

template <typename T, typename U>
static constexpr bool IsSameV = IsSame<T, U>::value;

template <typename T>
constexpr T DeclVal();

template <typename T>
constexpr void TryParse(T);

template <typename T, typename U, typename = void>
struct IsBaseOf {
    static constexpr bool value = false;
};

template <typename T, typename U>
struct IsBaseOf<T, U, decltype(TryParse<T>(DeclVal<U>()))> {
    static constexpr bool value = true;
};
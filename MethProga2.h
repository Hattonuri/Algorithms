#pragma once

struct Object {
    virtual ~Object() = default;
};
struct Chair : Object {};
struct Table : Object {};
struct Sofa : Object {};
struct WoodenChair : Chair {};
struct WoodenTable : Table {};
struct WoodenSofa : Sofa {};
struct MetalChair : Chair {};
struct MetalTable : Table {};
struct MetalSofa : Sofa {};
struct MetalRichChair : MetalChair {};
struct MetalRichTable : MetalTable {};
struct MetalRichSofa : MetalSofa {};
struct MetalPoorChair : MetalChair {};
struct MetalPoorTable : MetalTable {};
struct MetalPoorSofa : MetalSofa {};

template <bool val>
struct EnableIf {};

template <>
struct EnableIf<true> {
    using type = char;
};

template <typename T, typename U>
struct IsSame {
    static constexpr bool value = false;
};

template <typename T>
struct IsSame<T, T> {
    static constexpr bool value = true;
};

template <typename T>
constexpr T DeclVal();

template <typename T>
constexpr void TryParse(T&);

template <typename T, T a>
struct ValueHolder {
    static constexpr T value = a;
};

template<typename T>
class IsConstructible {
 private:
    template<typename C_T>
    static C_T testFun(C_T);
    template<typename C_T>
    static bool test(typename EnableIf<sizeof(C_T) == sizeof(testFun(C_T()))>::type*);

    template<typename>
    static int test( ... );

 public:
    static constexpr bool value = sizeof(test<T>(nullptr)) == sizeof(bool);
};

template <typename T, typename U, typename = void>
struct IsBaseOf {
    static constexpr bool value = false;
};

template <typename T, typename U>
struct IsBaseOf<T, U, decltype(TryParse<T>(DeclVal<U&>()))> {
    static constexpr bool value = true;
};

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

template <typename... Args>
struct Typelist;

template <>
struct Typelist<> {};

template <typename _Head, typename... _Args>
struct Typelist<_Head, _Args...> : Typelist<_Args...> {
    using Head = _Head;
    using Tail = Typelist<_Args...>;
};

template <typename, typename, typename>
struct AddIfGood;

template <typename Need, typename Type, typename... Args>
struct AddIfGood<Need, Type, Typelist<Args...>> {
    using type = ConditionalT<IsConstructible<Type>::value && IsBaseOf<Type, Need>::value, Typelist<Args..., Type>, Typelist<Args...>>;
};

template <typename Need, typename TL1, typename TL2>
struct MergeGood;

template <typename Need, typename TL1, typename Head, typename... Args>
struct MergeGood<Need, TL1, Typelist<Head, Args...>> {
    using type = typename MergeGood<Need, typename AddIfGood<Need, Head, TL1>::type, Typelist<Args...>>::type;
};

template <typename Need, typename TL1>
struct MergeGood<Need, TL1, Typelist<>> {
    using type = TL1;
};

template <typename Need, typename... Args>
struct MergeLists;

template <typename Need>
struct MergeLists<Need> {
    using type = Typelist<>;
};

template <typename Need, typename Head, typename... Args>
struct MergeLists<Need, Head, Args...> {
    using type = typename MergeGood<Need, Head, typename MergeLists<Need, Args...>::type>::type;
};

template <typename TL>
struct GetBestParentInList;

template <typename Head, typename... Args>
struct GetBestParentInList<Typelist<Head, Args...>> {
    using NextType = typename GetBestParentInList<Typelist<Args...>>::type;
    using type = ConditionalT<IsBaseOf<NextType, Head>::value, Head, NextType>;
};

template <>
struct GetBestParentInList<Typelist<>> {
    using type = Object;
};

template <typename Base, typename... Hierarchy>
struct ConcreteFactory {
    template <typename NeedType>
    typename GetBestParentInList<typename MergeLists<NeedType, Typelist<>, Hierarchy...>::type>::type* Get() {
        return new typename GetBestParentInList<typename MergeLists<NeedType, Typelist<>, Hierarchy...>::type>::type;
    }
};

template <typename... Groups>
struct GetAbstractFactory {
    template <typename InstanceType>
    struct GetConcreteFactory {
        using result = ConcreteFactory<InstanceType, Groups...>;
    };
};

// Because ADL doesn't work with pointers
// If you want to clear that line -> use ConcreteFactory MyFactory = MyFactoryHierarchy::GetConcreteFactory<MetalRichChair>::result();
#define Factory auto

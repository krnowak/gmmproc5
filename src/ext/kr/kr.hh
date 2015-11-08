#pragma once

#include <cstdlib>

#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

namespace Kr
{

// type list

template <typename... Types>
class TypeList;

// to std::tuple

template <typename List>
class ToStdTuple;

template <typename... Types>
class ToStdTuple<TypeList<Types...>>
{
public:
  using Type = std::tuple<Types...>;
};

// len

template <std::size_t value>
using IndexType = std::integral_constant<std::size_t, value>;

using ZeroIndexType = IndexType<0>;
using MaxIndexType = IndexType<std::numeric_limits<std::size_t>::max ()>;

template <typename List>
class Len;

template <typename... Types>
class Len<TypeList<Types...>>
{
public:
  using Type = IndexType<sizeof...(Types)>;
};

// first

template <typename List>
class First;

template <typename Head, typename... Tail>
class First<TypeList<Head, Tail...>>
{
public:
  using Type = Head;
};

// rest

template <typename List>
class Rest;

template <typename Head, typename... Tail>
class Rest<TypeList<Head, Tail...>>
{
public:
  using Type = TypeList<Tail...>;
};

template <>
class Rest<TypeList<>>
{
public:
  using Type = TypeList<>;
};

// nth

namespace NthDetails
{

template <typename List, typename IdxType>
class Nth
{
  using T = typename Rest<List>::Type;
  using IdxOneLess = IndexType<IdxType::value - 1>;

public:
  using Type = typename Nth<T, IdxOneLess>::Type;
};

template <typename List>
class Nth<List, ZeroIndexType>
{
public:
  using Type = typename First<List>::Type;
};

} // namespace NthDetails

template <typename List, typename Idx>
class Nth
{
  static_assert (Idx::value >= 0, "index is positive");
  static_assert (Idx::value < Len<List>::Type::value, "index is in bounds");

public:
  using Type = typename NthDetails::Nth<List, Idx>::Type;
};

// generic finder

// TODO: not necessary in C++17
template <bool value>
using BoolConstant = std::integral_constant<bool, value>;

namespace GenericFinderDetails
{

template <typename Found, typename FoundType>
class GenericFindResults
{
public:
  using Type = Found;
  using Breaker = FoundType;
};

template <typename Found, typename FoundType, typename Generate>
class GenerateGFR
{
public:
  using Type = GenericFindResults<Found, FoundType>;
};

template <typename Found, typename FoundType>
class GenerateGFR<Found, FoundType, std::false_type>
{
public:
  class Type;
};

template <typename List, template <typename Type> class Predicate, typename BreakOn>
class GenericFinder;

template <typename List, template <typename Type> class Predicate, typename BreakOn, typename Stop>
class GFContinue
{
public:
  class Type;
};

template <typename List, template <typename Type> class Predicate, typename BreakOn>
class GFContinue<List, Predicate, BreakOn, std::false_type>
{
  using Tail = typename Rest<List>::Type;

public:
  using Type = typename GenericFinder<Tail, Predicate, BreakOn>::Type;
};

template <typename List, template <typename Type> class Predicate, typename BreakOn>
class GenericFinder
{
  using Head = typename First<List>::Type;
  using PredicateResult = typename Predicate<Head>::Type;
  using Found = BoolConstant<std::is_same<BreakOn, PredicateResult>::value>;
  using Results = typename GenerateGFR<BreakOn, Head, Found>::Type;
  using Continuation = typename GFContinue<List, Predicate, BreakOn, Found>::Type;

public:
  using Type = typename std::conditional<Found::value,
                                         Results,
                                         Continuation>::type;
};

template <template <typename Type> class Predicate, bool BreakOnValue>
class GenericFinder<TypeList<>, Predicate, std::integral_constant<bool, BreakOnValue>>
{
  class NotFound;

public:
  using Type = GenericFindResults<BoolConstant<!BreakOnValue>, NotFound>;
};

} // namespace GenericFinderDetails

template <typename List, template <typename Type> class Predicate, typename BreakOn>
class GenericFinder
{
  using Results = typename GenericFinderDetails::GenericFinder<List, Predicate, BreakOn>::Type;

public:
  using Breaker = typename Results::Breaker;
  using Type = typename Results::Type;
};

// any

template <typename List, template <typename Type> class Predicate>
using Any = GenericFinder<List, Predicate, std::true_type>;

// all

template <typename List, template <typename Type> class Predicate>
using All = GenericFinder<List, Predicate, std::false_type>;

// index

namespace IndexDetails
{

template <typename Needle, typename Haystack, typename IdxType>
class Index;

template <typename Needle, typename Haystack, typename IdxType, typename Stop>
class IContinue
{
public:
  class Type;
};

template <typename Needle, typename Haystack, typename IdxType>
class IContinue<Needle, Haystack, IdxType, std::false_type>
{
  using Tail = typename Rest<Haystack>::Type;
  using NextIndex = IndexType<IdxType::value + 1>;

public:
  using Type = typename Index<Needle,
                              Tail,
                              NextIndex>::Type;
};

template <typename Needle, typename Haystack, typename IdxType>
class Index
{
  static_assert (!std::is_same<IdxType, MaxIndexType>::value, "index is not overflowing");

  using Head = typename First<Haystack>::Type;
  using Found = BoolConstant<std::is_same<Needle, Head>::value>;
  using Continuation = typename IContinue<Needle, Haystack, IdxType, Found>::Type;

public:
  using Type = typename std::conditional<Found::value,
                                         IdxType,
                                         Continuation>::type;
};

template <typename Needle, typename IdxType>
class Index<Needle, TypeList<>, IdxType>
{
  static_assert (!std::is_same<IdxType, MaxIndexType>::value, "index is not overflowing");

public:
  using Type = MaxIndexType;
};

} // namespace IndexDetails

template <typename Needle, typename Haystack>
class Index
{
public:
  using Type = typename IndexDetails::Index<Needle, Haystack, ZeroIndexType>::Type;
};

// in list

template <typename Needle, typename Haystack>
class IsInList
{
  using Idx = typename Index<Needle, Haystack>::Type;

public:
  using Type = BoolConstant<!std::is_same<Idx, MaxIndexType>::value>;
};

// concat

template <typename TL1, typename TL2>
class Concat;

template <typename... TL1, typename... TL2>
class Concat<TypeList<TL1...>, TypeList<TL2...>>
{
public:
  using Type = TypeList<TL1..., TL2...>;
};

// foreach

template <typename List, template <typename> class F>
class ForEach
{
  using Head = typename F<typename First<List>::Type>::Type;
  using Rest = typename ForEach<typename Rest<List>::Type, F>::Type;

public:
  using Type = typename Concat<TypeList<Head>,
                               Rest>::Type;
};

template <template <typename> class F>
class ForEach<TypeList<>, F>
{
public:
  using Type = TypeList<>;
};

// sieve

namespace SieveDetails
{

template <typename List, typename Item, typename Do>
class DoConcat
{
public:
  using Type = typename Concat<List, TypeList<Item>>::Type;
};

template <typename List, typename Item>
class DoConcat<List, Item, std::false_type>
{
public:
  class Type;
};

template <typename List, typename NewList, template <typename Item> class Predicate>
class Sieve
{
  using H = typename First<List>::Type;
  using T = typename Rest<List>::Type;
  using IsValid = BoolConstant<Predicate<H>::Type::value>;
  using IfValidList = typename DoConcat<NewList, H, IsValid>::Type;
  using NewerList = typename std::conditional<IsValid::value,
                                              IfValidList,
                                              NewList>::type;

public:
  using Type = typename Sieve<T, NewerList, Predicate>::Type;
};

template <typename NewList, template <typename Item> class Predicate>
class Sieve<TypeList<>, NewList, Predicate>
{
public:
  using Type = NewList;
};

} // namespace SieveDetails

template <typename List, template <typename Item> class Predicate>
class Sieve
{
public:
  using Type = typename SieveDetails::Sieve<List, TypeList<>, Predicate>::Type;
};

// accumulate

namespace AccumulateDetails
{

template <typename List, template <typename, typename> class Op>
class Accumulate
{
  using T1 = typename Rest<List>::Type;
  using T2 = typename Rest<T1>::Type;
  using H1 = typename First<List>::Type;
  using H2 = typename First<T1>::Type;
  using Result = typename Op<H1, H2>::Type;
  using NewList = typename Concat<TypeList<Result>, T2>::Type;

public:
  using Type = typename Accumulate<NewList, Op>::Type;
};

template <typename Item, template <typename, typename> class Op>
class Accumulate<TypeList<Item>, Op>
{
public:
  using Type = Item;
};

} // namespace AccumulateDetails

template <typename List, typename F>
class Accumulate
{
  using PreparedTypeList = typename Concat<TypeList<typename F::Start>, List>::Type;

public:
  using Type = typename AccumulateDetails::Accumulate<PreparedTypeList, F::template Op>::Type;
};

// flatten

namespace FlattenDetails
{

template <typename Item1, typename Item2>
class FlattenOp;

template <typename... Types, typename Item>
class FlattenOp<TypeList<Types...>, Item>
{
public:
  using Type = TypeList<Types..., Item>;
};

template <typename List>
class RealFlatten;

template <typename... Types1, typename... Types2>
class FlattenOp<TypeList<Types1...>, TypeList<Types2...>>
{
  using Flattened = typename RealFlatten<TypeList<Types2...>>::Type;

public:
  using Type = typename Concat<TypeList<Types1...>, Flattened>::Type;
};

class FlattenF
{
public:
  using Start = TypeList<>;
  template <typename L1, typename L2>
  using Op = FlattenOp<L1, L2>;
};

template <typename List>
class RealFlatten
{
public:
  using Type = typename Accumulate<List, FlattenF>::Type;
};

} // namespace FlattenDetails

template <typename List>
class Flatten
{
public:
  using Type = typename FlattenDetails::RealFlatten<List>::Type;
};

// is unique

namespace IsUniqueDetails
{

template <typename List, typename MetList>
class IsUnique;

template<typename List, typename MetList, typename Stop>
class IUContinue
{
public:
  class Type;
};

template<typename List, typename MetList>
class IUContinue<List, MetList, std::false_type>
{
  using Head = typename First<List>::Type;
  using Rest = typename Rest<List>::Type;
  using NewMetList = typename Concat<MetList, TypeList<Head>>::Type;

public:
  using Type = typename IsUnique<Rest, NewMetList>::Type;
};

template <typename List, typename MetList>
class IsUnique
{
  using AlreadyMet = typename IsInList<typename First<List>::Type, MetList>::Type;
  using Continuation = typename IUContinue<List, MetList, AlreadyMet>::Type;

public:
  using Type = typename std::conditional<AlreadyMet::value,
                                         std::false_type,
                                         Continuation>::type;
};

template <typename MetList>
class IsUnique<TypeList<>, MetList>
{
public:
  using Type = std::true_type;
};

} // namespace IsUniqueDetails

template <typename List>
class IsUnique
{
public:
  using Type = typename IsUniqueDetails::IsUnique<List, TypeList<>>::Type;
};

// filter generic

namespace FilterGenericDetails
{

template <typename Words, typename List, typename In>
class FilterGeneric
{
  using InType = typename IsInList<typename First<List>::Type, Words>::Type;
  using Head = typename First<List>::Type;
  using Tail = typename Rest<List>::Type;
  using List1 = typename std::conditional<InType::value == In::value,
                                          TypeList<Head>,
                                          TypeList<>>::type;
  using List2 = typename FilterGeneric<Words, Tail, In>::Type;

public:
  using Type = typename Concat<List1, List2>::Type;
};

template <typename Words, typename In>
class FilterGeneric<Words, TypeList<>, In>
{
public:
  using Type = TypeList<>;
};

} // namespace FilterGenericDetails

// filter

template <typename Words, typename List>
using Filter = FilterGenericDetails::FilterGeneric<Words, List, std::true_type>;

// filter out

template <typename Words, typename List>
using FilterOut = FilterGenericDetails::FilterGeneric<Words, List, std::false_type>;

// trtypes

template <template <typename, bool> class Traits, typename List, bool isa>
class TrTypes;

template <template <typename, bool> class Traits, typename... Types, bool isa>
class TrTypes<Traits, TypeList<Types...>, isa>
{
public:
  using Type = TypeList<typename Traits<Types, isa>::Type...>;
};

// convert

template <typename AllList, template <typename, bool> class Tr, typename... Types>
class Convert
{
  using InTypes = typename Filter<TypeList<Types...>, AllList>::Type;
  using TrInTypes = typename TrTypes<Tr, InTypes, true>::Type;
  using OutTypes = typename FilterOut<TypeList<Types...>, AllList>::Type;
  using TrOutTypes = typename TrTypes<Tr, OutTypes, false>::Type;

public:
  using Type = typename Concat<TrInTypes, TrOutTypes>::Type;
};

// subclass

template <typename T>
class Subclass : public T
{};

template <typename... Types>
class Subclass<TypeList<Types...>> : public Types...
{};

// common

template <typename AllList, template <typename, bool> class Tr, typename... Types>
class Common : public Subclass<typename Convert<AllList, Tr, Types...>::Type>
{
public:
  using BaseTypes = typename Convert<AllList, Tr, Types...>::Type;
};

// multicall

template <template <typename> class Wrapper, typename List>
class MultiCall
{
public:
  template <typename... ParamTypes>
  static void call(ParamTypes&&... p)
  {
    Wrapper<typename First<List>::Type>::call(std::forward<ParamTypes> (p)...);
    MultiCall<Wrapper, typename Rest<List>::Type>::call(std::forward<ParamTypes> (p)...);
  }
};

template <template <typename> class Wrapper>
class MultiCall<Wrapper, TypeList<>>
{
public:
  template <typename... ParamTypes>
  static void call(ParamTypes&&...)
  {}
};

} // namespace Kr

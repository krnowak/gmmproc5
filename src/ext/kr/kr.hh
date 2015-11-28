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

template <typename List>
using ToStdTupleT = typename ToStdTuple<List>::Type;

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

template <typename List>
using LenT = typename Len<List>::Type;

// first

template <typename List>
class First;

template <typename Head, typename... Tail>
class First<TypeList<Head, Tail...>>
{
public:
  using Type = Head;
};

template <typename List>
using FirstT = typename First<List>::Type;

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

template <typename List>
using RestT = typename Rest<List>::Type;

// nth

namespace NthDetails
{

template <typename List, typename IdxType>
class Nth
{
  using IdxOneLess = IndexType<IdxType::value - 1>;

public:
  using Type = typename Nth<RestT<List>, IdxOneLess>::Type;
};

template <typename List>
class Nth<List, ZeroIndexType>
{
public:
  using Type = FirstT<List>;
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

template <typename List, typename Idx>
using NthT = typename Nth<List, Idx>::Type;

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
public:
  using Type = typename GenericFinder<RestT<List>, Predicate, BreakOn>::Type;
};

template <typename List, template <typename Type> class Predicate, typename BreakOn>
class GenericFinder
{
  using PredicateResult = typename Predicate<FirstT<List>>::Type;
  using Found = BoolConstant<std::is_same<BreakOn, PredicateResult>::value>;
  using Results = typename GenerateGFR<BreakOn, FirstT<List>, Found>::Type;
  using Continuation = typename GFContinue<List, Predicate, BreakOn, Found>::Type;

public:
  using Type = std::conditional_t<Found::value,
                                  Results,
                                  Continuation>;
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
  using NextIndex = IndexType<IdxType::value + 1>;

public:
  using Type = typename Index<Needle,
                              RestT<Haystack>,
                              NextIndex>::Type;
};

template <typename Needle, typename Haystack, typename IdxType>
class Index
{
  static_assert (!std::is_same<IdxType, MaxIndexType>::value, "index is not overflowing");

  using Found = BoolConstant<std::is_same<Needle, FirstT<Haystack>>::value>;
  using Continuation = typename IContinue<Needle, Haystack, IdxType, Found>::Type;

public:
  using Type = std::conditional_t<Found::value,
                                  IdxType,
                                  Continuation>;
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

template <typename Needle, typename Haystack>
using IndexT = typename Index<Needle, Haystack>::Type;

// in list

template <typename Needle, typename Haystack>
class IsInList
{
public:
  using Type = BoolConstant<!std::is_same<IndexT<Needle, Haystack>, MaxIndexType>::value>;
};

template <typename Needle, typename Haystack>
using IsInListT = typename IsInList<Needle, Haystack>::Type;

// concat

template <typename TL1, typename TL2>
class Concat;

template <typename... TL1, typename... TL2>
class Concat<TypeList<TL1...>, TypeList<TL2...>>
{
public:
  using Type = TypeList<TL1..., TL2...>;
};

template <typename TL1, typename TL2>
using ConcatT = typename Concat<TL1, TL2>::Type;

// foreach

template <typename List, template <typename> class F>
class ForEach
{
  using Head = typename F<FirstT<List>>::Type;
  using Rest = typename ForEach<RestT<List>, F>::Type;

public:
  using Type = ConcatT<TypeList<Head>, Rest>;
};

template <template <typename> class F>
class ForEach<TypeList<>, F>
{
public:
  using Type = TypeList<>;
};

template <typename List, template <typename> class F>
using ForEachT = typename ForEach<List, F>::Type;

// sieve

namespace SieveDetails
{

template <typename List, typename Item, typename Do>
class DoConcat
{
public:
  using Type = ConcatT<List, TypeList<Item>>;
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
  using IsValid = BoolConstant<Predicate<FirstT<List>>::Type::value>;
  using IfValidList = typename DoConcat<NewList, FirstT<List>, IsValid>::Type;
  using NewerList = std::conditional_t<IsValid::value,
                                       IfValidList,
                                       NewList>;

public:
  using Type = typename Sieve<RestT<List>, NewerList, Predicate>::Type;
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

template <typename List, template <typename Item> class Predicate>
using SieveT = typename Sieve<List, Predicate>::Type;

// accumulate

namespace AccumulateDetails
{

template <typename List, template <typename, typename> class Op>
class Accumulate
{
  using Tail = RestT<List>;
  using Result = typename Op<FirstT<List>, FirstT<Tail>>::Type;
  using NewList = typename Concat<TypeList<Result>, RestT<Tail>>::Type;

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

template <typename List, typename F>
using AccumulateT = typename Accumulate<List, F>::Type;

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
  using Type = ConcatT<TypeList<Types1...>, Flattened>;
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
  using Type = AccumulateT<List, FlattenF>;
};

} // namespace FlattenDetails

template <typename List>
class Flatten
{
public:
  using Type = typename FlattenDetails::RealFlatten<List>::Type;
};

template <typename List>
using FlattenT = typename Flatten<List>::Type;

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
  using NewMetList = ConcatT<MetList, TypeList<FirstT<List>>>;

public:
  using Type = typename IsUnique<RestT<List>, NewMetList>::Type;
};

template <typename List, typename MetList>
class IsUnique
{
  using AlreadyMet = IsInListT<FirstT<List>, MetList>;
  using Continuation = typename IUContinue<List, MetList, AlreadyMet>::Type;

public:
  using Type = std::conditional_t<AlreadyMet::value,
                                  std::false_type,
                                  Continuation>;
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

template <typename List>
using IsUniqueT = typename IsUnique<List>::Type;

// filter generic

namespace FilterGenericDetails
{

template <typename Words, typename List, typename In>
class FilterGeneric
{
  using Head = FirstT<List>;
  using InType = IsInListT<Head, Words>;
  using List1 = std::conditional_t<InType::value == In::value,
                                   TypeList<Head>,
                                   TypeList<>>;
  using List2 = typename FilterGeneric<Words, RestT<List>, In>::Type;

public:
  using Type = ConcatT<List1, List2>;
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

template <typename Words, typename List>
using FilterT = typename Filter<Words, List>::Type;

// filter out

template <typename Words, typename List>
using FilterOut = FilterGenericDetails::FilterGeneric<Words, List, std::false_type>;

template <typename Words, typename List>
using FilterOutT = typename FilterOut<Words, List>::Type;

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

#include <type_traits> // conditional, is_same

#include "kr.hh"

namespace Kr
{

namespace Test
{

// to std::tuple tests

static_assert (std::is_same<ToStdTupleT<TypeList<>>, std::tuple<>>::value, "to std::tuple works");
static_assert (std::is_same<ToStdTupleT<TypeList<int>>, std::tuple<int>>::value, "to std::tuple works");
static_assert (std::is_same<ToStdTupleT<TypeList<int, char>>, std::tuple<int, char>>::value, "to std::tuple works");

// len tests

static_assert (LenT<TypeList<int, int, int>>::value == 3, "len works");
static_assert (LenT<TypeList<int, int>>::value == 2, "len works");
static_assert (LenT<TypeList<int>>::value == 1, "len works");
static_assert (LenT<TypeList<>>::value == 0, "len works");

// first tests

static_assert (std::is_same<FirstT<TypeList<char>>, char>::value, "first works");
static_assert (std::is_same<FirstT<TypeList<int, char>>, int>::value, "first works");

// rest tests

static_assert (std::is_same<RestT<TypeList<>>, TypeList<>>::value, "rest works");
static_assert (std::is_same<RestT<TypeList<int>>, TypeList<>>::value, "rest works");
static_assert (std::is_same<RestT<TypeList<int, char>>, TypeList<char>>::value, "rest works");
static_assert (std::is_same<RestT<TypeList<int, char, double>>, TypeList<char, double>>::value, "rest works");

// nth tests

static_assert (std::is_same<NthT<TypeList<char>, ZeroIndexType>, char>::value, "nth works");
static_assert (std::is_same<NthT<TypeList<char, double>, ZeroIndexType>, char>::value, "nth works");
static_assert (std::is_same<NthT<TypeList<char, double, int>, IndexType<1>>, double>::value, "nth works");
static_assert (std::is_same<NthT<TypeList<char, double, int>, IndexType<2>>, int>::value, "nth works");

// any tests

namespace TestAny
{

template <typename T>
class Predicate
{
public:
  using Type = BoolConstant<std::is_same<bool, T>::value>;
};

} // namespace TestAny

static_assert (std::is_same<Any<TypeList<>, TestAny::Predicate>::Type, std::false_type>::value, "any works");
static_assert (std::is_same<Any<TypeList<int>, TestAny::Predicate>::Type, std::false_type>::value, "any works");
static_assert (std::is_same<Any<TypeList<int, char>, TestAny::Predicate>::Type, std::false_type>::value, "any works");
static_assert (std::is_same<Any<TypeList<bool>, TestAny::Predicate>::Type, std::true_type>::value, "any works");
static_assert (std::is_same<Any<TypeList<bool>, TestAny::Predicate>::Breaker, bool>::value, "any works");
static_assert (std::is_same<Any<TypeList<int, bool>, TestAny::Predicate>::Type, std::true_type>::value, "any works");
static_assert (std::is_same<Any<TypeList<int, bool>, TestAny::Predicate>::Breaker, bool>::value, "any works");
static_assert (std::is_same<Any<TypeList<bool, bool>, TestAny::Predicate>::Type, std::true_type>::value, "any works");
static_assert (std::is_same<Any<TypeList<bool, bool>, TestAny::Predicate>::Breaker, bool>::value, "any works");

// all tests

namespace TestAll
{

template <typename T>
class Predicate
{
public:
  using Type = BoolConstant<std::is_same<bool, T>::value>;
};

} // namespace TestAll

static_assert (std::is_same<All<TypeList<>, TestAll::Predicate>::Type, std::true_type>::value, "all works");
static_assert (std::is_same<All<TypeList<int>, TestAll::Predicate>::Type, std::false_type>::value, "all works");
static_assert (std::is_same<All<TypeList<int>, TestAll::Predicate>::Breaker, int>::value, "all works");
static_assert (std::is_same<All<TypeList<int, char>, TestAll::Predicate>::Type, std::false_type>::value, "all works");
static_assert (std::is_same<All<TypeList<int, char>, TestAll::Predicate>::Breaker, int>::value, "all works");
static_assert (std::is_same<All<TypeList<bool>, TestAll::Predicate>::Type, std::true_type>::value, "all works");
static_assert (std::is_same<All<TypeList<int, bool>, TestAll::Predicate>::Type, std::false_type>::value, "all works");
static_assert (std::is_same<All<TypeList<int, bool>, TestAll::Predicate>::Breaker, int>::value, "all works");
static_assert (std::is_same<All<TypeList<bool, bool>, TestAll::Predicate>::Type, std::true_type>::value, "all works");

// index tests

static_assert (IndexT<char, TypeList<char>>::value == 0, "index works");
static_assert (IndexT<char, TypeList<char, int>>::value == 0, "index works");
static_assert (IndexT<int, TypeList<char, int>>::value == 1, "index works");
static_assert (IndexT<double, TypeList<char, int>>::value == MaxIndexType::value, "index works");

// is in list tests

static_assert (IsInListT<int, TypeList<int, double>>::value, "is in list works");
static_assert (IsInListT<double, TypeList<int, double>>::value, "is in list works");
static_assert (!IsInListT<char, TypeList<int, double>>::value, "is in list works");
static_assert (!IsInListT<char, TypeList<>>::value, "is in list works");

// concat tests

static_assert (std::is_same<ConcatT<TypeList<int, char>, TypeList<float, double>>, TypeList<int, char, float, double>>::value, "concat works");
static_assert (std::is_same<ConcatT<TypeList<>, TypeList<int, char>>, TypeList<int, char>>::value, "concat works");
static_assert (std::is_same<ConcatT<TypeList<int, char>, TypeList<>>, TypeList<int, char>>::value, "concat works");
static_assert (std::is_same<ConcatT<TypeList<>, TypeList<>>, TypeList<>>::value, "concat works");

// for each tests

namespace TestForEach
{

template <typename T>
class A{};

template <typename T>
class F
{
public:
  using Type = A<T>;
};

} // namespace TestForEach

static_assert (std::is_same<ForEachT<TypeList<>, TestForEach::F>, TypeList<>>::value, "for each works");
static_assert (std::is_same<ForEachT<TypeList<int>, TestForEach::F>, TypeList<TestForEach::A<int>>>::value, "for each works");
static_assert (std::is_same<ForEachT<TypeList<int, char>, TestForEach::F>, TypeList<TestForEach::A<int>, TestForEach::A<char>>>::value, "for each works");

// sieve tests

namespace TestSieve
{

template <typename Int>
class Even
{
public:
  using Type = BoolConstant<Int::value % 2 == 0>;
};

using One = IndexType<1>;
using Two = IndexType<2>;
using Three = IndexType<3>;
using Four = IndexType<4>;

} // namespace TestSieve

static_assert (std::is_same<SieveT<TypeList<>, TestSieve::Even>, TypeList<>>::value, "sieve works");
static_assert (std::is_same<SieveT<TypeList<TestSieve::One>, TestSieve::Even>, TypeList<>>::value, "sieve works");
static_assert (std::is_same<SieveT<TypeList<TestSieve::Two>, TestSieve::Even>, TypeList<TestSieve::Two>>::value, "sieve works");
static_assert (std::is_same<SieveT<TypeList<TestSieve::One, TestSieve::Two>, TestSieve::Even>, TypeList<TestSieve::Two>>::value, "sieve works");
static_assert (std::is_same<SieveT<TypeList<TestSieve::One, TestSieve::Two, TestSieve::Three>, TestSieve::Even>, TypeList<TestSieve::Two>>::value, "sieve works");
static_assert (std::is_same<SieveT<TypeList<TestSieve::One, TestSieve::Two, TestSieve::Three, TestSieve::Four>, TestSieve::Even>, TypeList<TestSieve::Two, TestSieve::Four>>::value, "sieve works");

// accumulate tests

namespace TestAccumulate
{

template <typename N1, typename N2>
class Add
{
  static_assert (std::is_same<typename N1::value_type, typename ZeroIndexType::value_type>::value, "first operand is of index type");
  static_assert (std::is_same<typename N2::value_type, typename ZeroIndexType::value_type>::value, "first operand is of index type");

public:
  using Type = IndexType<N1::value + N2::value>;
};

using Zero = IndexType<0>;
using One = IndexType<1>;
using Two = IndexType<2>;
using Three = IndexType<3>;
using Four = IndexType<4>;
using Ten = IndexType<10>;

class AddF
{
public:
  using Start = Zero;
  template <typename N1, typename N2>
  using Op = Add<N1, N2>;
};

} // namespace TestAccumulate

static_assert (std::is_same<AccumulateT<TypeList<>, TestAccumulate::AddF>, TestAccumulate::Zero>::value, "accumulate works");
static_assert (std::is_same<AccumulateT<TypeList<TestAccumulate::One>, TestAccumulate::AddF>, TestAccumulate::One>::value, "accumulate works");
static_assert (std::is_same<AccumulateT<TypeList<TestAccumulate::One, TestAccumulate::One>, TestAccumulate::AddF>, TestAccumulate::Two>::value, "accumulate works");
static_assert (std::is_same<AccumulateT<TypeList<TestAccumulate::One, TestAccumulate::Two>, TestAccumulate::AddF>, TestAccumulate::Three>::value, "accumulate works");
static_assert (std::is_same<AccumulateT<TypeList<TestAccumulate::One, TestAccumulate::Two, TestAccumulate::Three, TestAccumulate::Four>, TestAccumulate::AddF>, TestAccumulate::Ten>::value, "accumulate works");

// flatten tests

namespace TestFlatten
{

class A;
class B;
class C;
class D;
class E;
class F;
class G;
class H;
class I;
class J;

} // namespace TestFlatten

// empty list
static_assert (std::is_same<FlattenT<TypeList<>>, TypeList<>>::value, "flatten works");
// <A>
static_assert (std::is_same<FlattenT<TypeList<TestFlatten::A>>, TypeList<TestFlatten::A>>::value, "flatten works");
// <A, B>
static_assert (std::is_same<FlattenT<TypeList<TestFlatten::A, TestFlatten::B>>, TypeList<TestFlatten::A, TestFlatten::B>>::value, "flatten works");
// <A, B, <C, D>>
static_assert (std::is_same<FlattenT<TypeList<TestFlatten::A, TestFlatten::B, TypeList<TestFlatten::C, TestFlatten::D>>>, TypeList<TestFlatten::A, TestFlatten::B, TestFlatten::C, TestFlatten::D>>::value, "flatten works");
// <A, B, <C, D, <E, F>>>
static_assert (std::is_same<FlattenT<TypeList<TestFlatten::A, TestFlatten::B, TypeList<TestFlatten::C, TestFlatten::D, TypeList<TestFlatten::E, TestFlatten::F>>>>, TypeList<TestFlatten::A, TestFlatten::B, TestFlatten::C, TestFlatten::D, TestFlatten::E, TestFlatten::F>>::value, "flatten works");
// <A, B, <C, D, <E, F>, G, H>>
static_assert (std::is_same<FlattenT<TypeList<TestFlatten::A, TestFlatten::B, TypeList<TestFlatten::C, TestFlatten::D, TypeList<TestFlatten::E, TestFlatten::F>, TestFlatten::G, TestFlatten::H>>>, TypeList<TestFlatten::A, TestFlatten::B, TestFlatten::C, TestFlatten::D, TestFlatten::E, TestFlatten::F, TestFlatten::G, TestFlatten::H>>::value, "flatten works");
// <A, B, <C, D, <E, F>, G, H>, I, J>
static_assert (std::is_same<FlattenT<TypeList<TestFlatten::A, TestFlatten::B, TypeList<TestFlatten::C, TestFlatten::D, TypeList<TestFlatten::E, TestFlatten::F>, TestFlatten::G, TestFlatten::H>, TestFlatten::I, TestFlatten::J>>, TypeList<TestFlatten::A, TestFlatten::B, TestFlatten::C, TestFlatten::D, TestFlatten::E, TestFlatten::F, TestFlatten::G, TestFlatten::H, TestFlatten::I, TestFlatten::J>>::value, "flatten works");
// <<A, B>, <<C, D>>, <E, F>, <G, H>>
static_assert (std::is_same<FlattenT<TypeList<TypeList<TestFlatten::A, TestFlatten::B>, TypeList<TypeList<TestFlatten::C, TestFlatten::D> >, TypeList<TestFlatten::E, TestFlatten::F>, TypeList<TestFlatten::G, TestFlatten::H>>>, TypeList<TestFlatten::A, TestFlatten::B, TestFlatten::C, TestFlatten::D, TestFlatten::E, TestFlatten::F, TestFlatten::G, TestFlatten::H> >::value, "flatten works");

// is unique tests

static_assert (IsUniqueT<TypeList<>>::value, "is unique works");
static_assert (IsUniqueT<TypeList<char>>::value, "is unique works");
static_assert (IsUniqueT<TypeList<char, int>>::value, "is unique works");
static_assert (!IsUniqueT<TypeList<char, int, char>>::value, "is unique works");
static_assert (!IsUniqueT<TypeList<char, int, char, double>>::value, "is unique works");

// filter tests

static_assert (std::is_same<FilterT<TypeList<int, double, float>, TypeList<int, double, char>>, TypeList<int, double>>::value, "filter works");
static_assert (std::is_same<FilterT<TypeList<int, double>, TypeList<char, float>>, TypeList<>>::value, "filter works");
static_assert (std::is_same<FilterT<TypeList<int, int, int>, TypeList<char>>, TypeList<>>::value, "filter works");
static_assert (std::is_same<FilterT<TypeList<int, int, int>, TypeList<int>>, TypeList<int>>::value, "filter works");
static_assert (std::is_same<FilterT<TypeList<int>, TypeList<char, char, char>>, TypeList<>>::value, "filter works");
static_assert (std::is_same<FilterT<TypeList<int>, TypeList<int, int, int>>, TypeList<int, int, int>>::value, "filter works");
static_assert (std::is_same<FilterT<TypeList<>, TypeList<int, double>>, TypeList<>>::value, "filter works");
static_assert (std::is_same<FilterT<TypeList<int, double>, TypeList<>>, TypeList<>>::value, "filter works");
static_assert (std::is_same<FilterT<TypeList<>, TypeList<>>, TypeList<>>::value, "filter works");

// filter out tests

static_assert (std::is_same<FilterOutT<TypeList<int, double, float>, TypeList<int, double, char>>, TypeList<char>>::value, "filter out works");
static_assert (std::is_same<FilterOutT<TypeList<int, double>, TypeList<int, double>>, TypeList<>>::value, "filter out works");
static_assert (std::is_same<FilterOutT<TypeList<int, int, int>, TypeList<char>>, TypeList<char>>::value, "filter out works");
static_assert (std::is_same<FilterOutT<TypeList<int, int, int>, TypeList<int>>, TypeList<>>::value, "filter out works");
static_assert (std::is_same<FilterOutT<TypeList<int>, TypeList<char, char, char>>, TypeList<char, char, char>>::value, "filter out works");
static_assert (std::is_same<FilterOutT<TypeList<int>, TypeList<int, int, int>>, TypeList<>>::value, "filter out works");
static_assert (std::is_same<FilterOutT<TypeList<>, TypeList<int, double>>, TypeList<int, double>>::value, "filter out works");
static_assert (std::is_same<FilterOutT<TypeList<int, double>, TypeList<>>, TypeList<>>::value, "filter out works");
static_assert (std::is_same<FilterOutT<TypeList<>, TypeList<>>, TypeList<>>::value, "filter out works");

} // namespace Test

} // namespace Kr

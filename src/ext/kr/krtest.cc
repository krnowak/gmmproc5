#include <type_traits> // conditional, is_same

#include "kr.hh"

namespace Kr
{

namespace Test
{

// to std::tuple tests

static_assert (std::is_same<ToStdTuple<TypeList<>>::Type, std::tuple<>>::value, "to std::tuple works");
static_assert (std::is_same<ToStdTuple<TypeList<int>>::Type, std::tuple<int>>::value, "to std::tuple works");
static_assert (std::is_same<ToStdTuple<TypeList<int, char>>::Type, std::tuple<int, char>>::value, "to std::tuple works");

// len tests

static_assert (Len<TypeList<int, int, int>>::Type::value == 3, "len works");
static_assert (Len<TypeList<int, int>>::Type::value == 2, "len works");
static_assert (Len<TypeList<int>>::Type::value == 1, "len works");
static_assert (Len<TypeList<>>::Type::value == 0, "len works");

// first tests

static_assert (std::is_same<First<TypeList<char>>::Type, char>::value, "first works");
static_assert (std::is_same<First<TypeList<int, char>>::Type, int>::value, "first works");

// rest tests

static_assert (std::is_same<Rest<TypeList<>>::Type, TypeList<>>::value, "rest works");
static_assert (std::is_same<Rest<TypeList<int>>::Type, TypeList<>>::value, "rest works");
static_assert (std::is_same<Rest<TypeList<int, char>>::Type, TypeList<char>>::value, "rest works");
static_assert (std::is_same<Rest<TypeList<int, char, double>>::Type, TypeList<char, double>>::value, "rest works");

// nth tests

static_assert (std::is_same<Nth<TypeList<char>, ZeroIndexType>::Type, char>::value, "nth works");
static_assert (std::is_same<Nth<TypeList<char, double>, ZeroIndexType>::Type, char>::value, "nth works");
static_assert (std::is_same<Nth<TypeList<char, double, int>, IndexType<1>>::Type, double>::value, "nth works");
static_assert (std::is_same<Nth<TypeList<char, double, int>, IndexType<2>>::Type, int>::value, "nth works");

// any tests

namespace TestAny
{

template <typename T>
class Predicate
{
public:
  using Type = typename std::conditional<std::is_same<bool, T>::value,
                                         std::true_type,
                                         std::false_type>::type;
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
  using Type = typename std::conditional<std::is_same<bool, T>::value,
                                         std::true_type,
                                         std::false_type>::type;
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

static_assert (Index<char, TypeList<char>>::Type::value == 0, "index works");
static_assert (Index<char, TypeList<char, int>>::Type::value == 0, "index works");
static_assert (Index<int, TypeList<char, int>>::Type::value == 1, "index works");
static_assert (Index<double, TypeList<char, int>>::Type::value == MaxIndexType::value, "index works");

// is in list tests

static_assert (IsInList<int, TypeList<int, double>>::Type::value, "is in list works");
static_assert (IsInList<double, TypeList<int, double>>::Type::value, "is in list works");
static_assert (!IsInList<char, TypeList<int, double>>::Type::value, "is in list works");
static_assert (!IsInList<char, TypeList<>>::Type::value, "is in list works");

// concat tests

static_assert (std::is_same<Concat<TypeList<int, char>, TypeList<float, double>>::Type, TypeList<int, char, float, double>>::value, "concat works");
static_assert (std::is_same<Concat<TypeList<>, TypeList<int, char>>::Type, TypeList<int, char>>::value, "concat works");
static_assert (std::is_same<Concat<TypeList<int, char>, TypeList<>>::Type, TypeList<int, char>>::value, "concat works");
static_assert (std::is_same<Concat<TypeList<>, TypeList<>>::Type, TypeList<>>::value, "concat works");

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

static_assert (std::is_same<ForEach<TypeList<>, TestForEach::F>::Type, TypeList<>>::value, "for each works");
static_assert (std::is_same<ForEach<TypeList<int>, TestForEach::F>::Type, TypeList<TestForEach::A<int>>>::value, "for each works");
static_assert (std::is_same<ForEach<TypeList<int, char>, TestForEach::F>::Type, TypeList<TestForEach::A<int>, TestForEach::A<char>>>::value, "for each works");

// sieve tests

namespace TestSieve
{

template <typename Int>
class Even
{
public:
  using Type = typename std::conditional<Int::value % 2 == 0,
                                         std::true_type,
                                         std::false_type>::type;
};

using One = std::integral_constant<int, 1>;
using Two = std::integral_constant<int, 2>;
using Three = std::integral_constant<int, 3>;
using Four = std::integral_constant<int, 4>;

} // namespace TestSieve

static_assert (std::is_same<Sieve<TypeList<>, TestSieve::Even>::Type, TypeList<>>::value, "sieve works");
static_assert (std::is_same<Sieve<TypeList<TestSieve::One>, TestSieve::Even>::Type, TypeList<>>::value, "sieve works");
static_assert (std::is_same<Sieve<TypeList<TestSieve::Two>, TestSieve::Even>::Type, TypeList<TestSieve::Two>>::value, "sieve works");
static_assert (std::is_same<Sieve<TypeList<TestSieve::One, TestSieve::Two>, TestSieve::Even>::Type, TypeList<TestSieve::Two>>::value, "sieve works");
static_assert (std::is_same<Sieve<TypeList<TestSieve::One, TestSieve::Two, TestSieve::Three>, TestSieve::Even>::Type, TypeList<TestSieve::Two>>::value, "sieve works");
static_assert (std::is_same<Sieve<TypeList<TestSieve::One, TestSieve::Two, TestSieve::Three, TestSieve::Four>, TestSieve::Even>::Type, TypeList<TestSieve::Two, TestSieve::Four>>::value, "sieve works");

// accumulate tests

namespace TestAccumulate
{

template <typename N1, typename N2>
class Add
{
  static_assert (std::is_same<typename N1::value_type, typename N2::value_type>::value, "both integral types are the same");
public:
  using Type = std::integral_constant<typename N1::value_type, N1::value + N2::value>;
};

using Zero = std::integral_constant<int, 0>;
using One = std::integral_constant<int, 1>;
using Two = std::integral_constant<int, 2>;
using Three = std::integral_constant<int, 3>;
using Four = std::integral_constant<int, 4>;
using Ten = std::integral_constant<int, 10>;

class AddF
{
public:
  using Start = Zero;
  template <typename N1, typename N2>
  using Op = Add<N1, N2>;
};

} // namespace TestAccumulate

static_assert (std::is_same<typename Accumulate<TypeList<>, TestAccumulate::AddF>::Type, TestAccumulate::Zero>::value, "accumulate works");
static_assert (std::is_same<typename Accumulate<TypeList<TestAccumulate::One>, TestAccumulate::AddF>::Type, TestAccumulate::One>::value, "accumulate works");
static_assert (std::is_same<typename Accumulate<TypeList<TestAccumulate::One, TestAccumulate::One>, TestAccumulate::AddF>::Type, TestAccumulate::Two>::value, "accumulate works");
static_assert (std::is_same<typename Accumulate<TypeList<TestAccumulate::One, TestAccumulate::Two>, TestAccumulate::AddF>::Type, TestAccumulate::Three>::value, "accumulate works");
static_assert (std::is_same<typename Accumulate<TypeList<TestAccumulate::One, TestAccumulate::Two, TestAccumulate::Three, TestAccumulate::Four>, TestAccumulate::AddF>::Type, TestAccumulate::Ten>::value, "accumulate works");

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
static_assert (std::is_same<typename Flatten<TypeList<>>::Type, TypeList<>>::value, "flatten works");
// <A>
static_assert (std::is_same<typename Flatten<TypeList<TestFlatten::A>>::Type, TypeList<TestFlatten::A>>::value, "flatten works");
// <A, B>
static_assert (std::is_same<typename Flatten<TypeList<TestFlatten::A, TestFlatten::B>>::Type, TypeList<TestFlatten::A, TestFlatten::B>>::value, "flatten works");
// <A, B, <C, D>>
static_assert (std::is_same<typename Flatten<TypeList<TestFlatten::A, TestFlatten::B, TypeList<TestFlatten::C, TestFlatten::D>>>::Type, TypeList<TestFlatten::A, TestFlatten::B, TestFlatten::C, TestFlatten::D>>::value, "flatten works");
// <A, B, <C, D, <E, F>>>
static_assert (std::is_same<typename Flatten<TypeList<TestFlatten::A, TestFlatten::B, TypeList<TestFlatten::C, TestFlatten::D, TypeList<TestFlatten::E, TestFlatten::F>>>>::Type, TypeList<TestFlatten::A, TestFlatten::B, TestFlatten::C, TestFlatten::D, TestFlatten::E, TestFlatten::F>>::value, "flatten works");
// <A, B, <C, D, <E, F>, G, H>>
static_assert (std::is_same<typename Flatten<TypeList<TestFlatten::A, TestFlatten::B, TypeList<TestFlatten::C, TestFlatten::D, TypeList<TestFlatten::E, TestFlatten::F>, TestFlatten::G, TestFlatten::H>>>::Type, TypeList<TestFlatten::A, TestFlatten::B, TestFlatten::C, TestFlatten::D, TestFlatten::E, TestFlatten::F, TestFlatten::G, TestFlatten::H>>::value, "flatten works");
// <A, B, <C, D, <E, F>, G, H>, I, J>
static_assert (std::is_same<typename Flatten<TypeList<TestFlatten::A, TestFlatten::B, TypeList<TestFlatten::C, TestFlatten::D, TypeList<TestFlatten::E, TestFlatten::F>, TestFlatten::G, TestFlatten::H>, TestFlatten::I, TestFlatten::J>>::Type, TypeList<TestFlatten::A, TestFlatten::B, TestFlatten::C, TestFlatten::D, TestFlatten::E, TestFlatten::F, TestFlatten::G, TestFlatten::H, TestFlatten::I, TestFlatten::J>>::value, "flatten works");
// <<A, B>, <<C, D>>, <E, F>, <G, H>>
static_assert (std::is_same<typename Flatten<TypeList<TypeList<TestFlatten::A, TestFlatten::B>, TypeList<TypeList<TestFlatten::C, TestFlatten::D> >, TypeList<TestFlatten::E, TestFlatten::F>, TypeList<TestFlatten::G, TestFlatten::H>>>::Type, TypeList<TestFlatten::A, TestFlatten::B, TestFlatten::C, TestFlatten::D, TestFlatten::E, TestFlatten::F, TestFlatten::G, TestFlatten::H> >::value, "flatten works");

// is unique tests

static_assert (IsUnique<TypeList<>>::Type::value, "is unique works");
static_assert (IsUnique<TypeList<char>>::Type::value, "is unique works");
static_assert (IsUnique<TypeList<char, int>>::Type::value, "is unique works");
static_assert (!IsUnique<TypeList<char, int, char>>::Type::value, "is unique works");
static_assert (!IsUnique<TypeList<char, int, char, double>>::Type::value, "is unique works");

// filter tests

static_assert (std::is_same<Filter<TypeList<int, double, float>, TypeList<int, double, char>>::Type, TypeList<int, double>>::value, "filter works");
static_assert (std::is_same<Filter<TypeList<int, double>, TypeList<char, float>>::Type, TypeList<>>::value, "filter works");
static_assert (std::is_same<Filter<TypeList<int, int, int>, TypeList<char>>::Type, TypeList<>>::value, "filter works");
static_assert (std::is_same<Filter<TypeList<int, int, int>, TypeList<int>>::Type, TypeList<int>>::value, "filter works");
static_assert (std::is_same<Filter<TypeList<int>, TypeList<char, char, char>>::Type, TypeList<>>::value, "filter works");
static_assert (std::is_same<Filter<TypeList<int>, TypeList<int, int, int>>::Type, TypeList<int, int, int>>::value, "filter works");
static_assert (std::is_same<Filter<TypeList<>, TypeList<int, double>>::Type, TypeList<>>::value, "filter works");
static_assert (std::is_same<Filter<TypeList<int, double>, TypeList<>>::Type, TypeList<>>::value, "filter works");
static_assert (std::is_same<Filter<TypeList<>, TypeList<>>::Type, TypeList<>>::value, "filter works");

// filter out tests

static_assert (std::is_same<FilterOut<TypeList<int, double, float>, TypeList<int, double, char>>::Type, TypeList<char>>::value, "filter out works");
static_assert (std::is_same<FilterOut<TypeList<int, double>, TypeList<int, double>>::Type, TypeList<>>::value, "filter out works");
static_assert (std::is_same<FilterOut<TypeList<int, int, int>, TypeList<char>>::Type, TypeList<char>>::value, "filter out works");
static_assert (std::is_same<FilterOut<TypeList<int, int, int>, TypeList<int>>::Type, TypeList<>>::value, "filter out works");
static_assert (std::is_same<FilterOut<TypeList<int>, TypeList<char, char, char>>::Type, TypeList<char, char, char>>::value, "filter out works");
static_assert (std::is_same<FilterOut<TypeList<int>, TypeList<int, int, int>>::Type, TypeList<>>::value, "filter out works");
static_assert (std::is_same<FilterOut<TypeList<>, TypeList<int, double>>::Type, TypeList<int, double>>::value, "filter out works");
static_assert (std::is_same<FilterOut<TypeList<int, double>, TypeList<>>::Type, TypeList<>>::value, "filter out works");
static_assert (std::is_same<FilterOut<TypeList<>, TypeList<>>::Type, TypeList<>>::value, "filter out works");

// trtypes tests

namespace TestTrTypes
{

template <typename TheType, bool isa>
class FooTraits
{
public:
  using Type = typename std::conditional<isa,
                                         typename TheType::First,
                                         typename TheType::Second>::type;
};

class Foo
{
public:
  class First {};
  class Second {};
};

} // namespace TestTrTypes

static_assert (std::is_same<TrTypes<TestTrTypes::FooTraits, TypeList<TestTrTypes::Foo>, true>::Type, TypeList<TestTrTypes::Foo::First>>::value, "trtypes work");
static_assert (std::is_same<TrTypes<TestTrTypes::FooTraits, TypeList<TestTrTypes::Foo>, false>::Type, TypeList<TestTrTypes::Foo::Second>>::value, "trtypes work");

// convert tests

namespace TestConvert
{

template <typename TheType, bool isa>
class FooTraits
{
public:
  using Type = typename std::conditional<isa,
                                         typename TheType::First,
                                         typename TheType::Second>::type;
};

class Foo1
{
public:
  class First {};
  class Second {};
};

class Foo2
{
public:
  class First {};
  class Second {};
};

using AllTypes = TypeList<Foo1, Foo2>;

} // namespace TestConvert

static_assert (std::is_same<Convert<TestConvert::AllTypes, TestConvert::FooTraits, TestConvert::Foo1>::Type, TypeList<TestConvert::Foo1::First, TestConvert::Foo2::Second>>::value, "convert works");

// subclass tests

namespace TestSubclass
{

class Foo1 {};
class Foo2 {};

} // namespace TestSubclass

static_assert (std::is_base_of<TestSubclass::Foo1, Subclass<TestSubclass::Foo1>>::value, "subclass works");
static_assert (std::is_base_of<TestSubclass::Foo1, Subclass<TypeList<TestSubclass::Foo1, TestSubclass::Foo2>>>::value, "subclass works");
static_assert (std::is_base_of<TestSubclass::Foo2, Subclass<TypeList<TestSubclass::Foo1, TestSubclass::Foo2>>>::value, "subclass works");

} // namespace Test

} // namespace Kr

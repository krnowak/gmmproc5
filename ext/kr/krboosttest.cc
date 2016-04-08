#include "krboost.hh"

#include <boost/mpl/equal.hpp>

#include <type_traits> // conditional, is_same

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

// nth tests

static_assert (std::is_same<NthT<TypeList<char>, ZeroIndexType>, char>::value, "nth works");
static_assert (std::is_same<NthT<TypeList<char, double>, ZeroIndexType>, char>::value, "nth works");
static_assert (std::is_same<NthT<TypeList<char, double, int>, IndexType<1>>, double>::value, "nth works");
static_assert (std::is_same<NthT<TypeList<char, double, int>, IndexType<2>>, int>::value, "nth works");

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

static_assert (mpl::equal<ConcatT<TypeList<int, char>, TypeList<float, double>>, TypeList<int, char, float, double>>::value, "concat works");
static_assert (mpl::equal<ConcatT<TypeList<>, TypeList<int, char>>, TypeList<int, char>>::value, "concat works");
static_assert (mpl::equal<ConcatT<TypeList<int, char>, TypeList<>>, TypeList<int, char>>::value, "concat works");
static_assert (mpl::equal<ConcatT<TypeList<>, TypeList<>>, TypeList<>>::value, "concat works");

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

static_assert (mpl::equal<ForEachT<TypeList<>, TestForEach::F>, TypeList<>>::value, "for each works");
static_assert (mpl::equal<ForEachT<TypeList<int>, TestForEach::F>, TypeList<TestForEach::A<int>>>::value, "for each works");
static_assert (mpl::equal<ForEachT<TypeList<int, char>, TestForEach::F>, TypeList<TestForEach::A<int>, TestForEach::A<char>>>::value, "for each works");

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

static_assert (mpl::equal<SieveT<TypeList<>, TestSieve::Even>, TypeList<>>::value, "sieve works");
static_assert (mpl::equal<SieveT<TypeList<TestSieve::One>, TestSieve::Even>, TypeList<>>::value, "sieve works");
static_assert (mpl::equal<SieveT<TypeList<TestSieve::Two>, TestSieve::Even>, TypeList<TestSieve::Two>>::value, "sieve works");
static_assert (mpl::equal<SieveT<TypeList<TestSieve::One, TestSieve::Two>, TestSieve::Even>, TypeList<TestSieve::Two>>::value, "sieve works");
static_assert (mpl::equal<SieveT<TypeList<TestSieve::One, TestSieve::Two, TestSieve::Three>, TestSieve::Even>, TypeList<TestSieve::Two>>::value, "sieve works");
static_assert (mpl::equal<SieveT<TypeList<TestSieve::One, TestSieve::Two, TestSieve::Three, TestSieve::Four>, TestSieve::Even>, TypeList<TestSieve::Two, TestSieve::Four>>::value, "sieve works");

// is unique tests

static_assert (IsUniqueT<TypeList<>>::value, "is unique works");
static_assert (IsUniqueT<TypeList<char>>::value, "is unique works");
static_assert (IsUniqueT<TypeList<char, int>>::value, "is unique works");
static_assert (!IsUniqueT<TypeList<char, int, char>>::value, "is unique works");
static_assert (!IsUniqueT<TypeList<char, int, char, double>>::value, "is unique works");

} // namespace Test

} // namespace Kr

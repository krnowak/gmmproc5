#include <boost/mpl/at.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/filter_view.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/joint_view.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/transform_view.hpp>
#include <boost/mpl/unique.hpp>
#include <boost/mpl/vector.hpp>

#include <type_traits>

namespace Kr
{

namespace mpl = boost::mpl;

// typelist

template <typename... Types>
using TypeList = mpl::vector<Types...>;

// tostdtuple

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

// indextype

template <std::size_t value>
using IndexType = std::integral_constant<std::size_t, value>;

using ZeroIndexType = IndexType<0>;
using MaxIndexType = IndexType<std::numeric_limits<std::size_t>::max ()>;

// len

template <typename List>
class Len
{
  using Type = IndexType<mpl::size<List>::value>;
};

template <typename List>
using LenT = typename Len<List>::Type;

// nth

template <typename List, typename Idx>
class Nth
{
  static_assert (Idx::value >= 0, "index is positive");
  static_assert (Idx::value < LenT<List>::value, "index is in bounds");

public:
  using Type = typename mpl::at<List, Idx>::type;
};

template <typename List, typename Idx>
using NthT = typename Nth<List, Idx>::Type;


template <typename Needle, typename Haystack>
class Index
{
  using Pos = typename mpl::find<Haystack, Needle>::type::pos;
public:
  using Type = std::conditional_t<Pos::value < LenT<Haystack>::value,
                                  IndexType<Pos::value>,
                                  MaxIndexType>;
};

// TODO: not necessary in C++17
// bool constant

template <bool value>
using BoolConstant = std::integral_constant<bool, value>;

// isinlist

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
{
public:
  using Type = mpl::joint_view<TL1..., TL2...>;
};

template <typename TL1, typename TL2>
using ConcatT = typename Concat<TL1, TL2>::Type;

// foreach

template <typename List, template <typename> class F>
class ForEach
{
  class MF
  {
    template <typename T>
    class apply
    {
    public:
      using type = typename F<T>::Type;
    };
  };

public:
  using Type = mpl::transform_view<List, MF>;
};

template <typename List, template <typename> class F>
using ForEachT = typename ForEach<List, F>::Type;

// sieve

template <typename List, template <typename Item> class Predicate>
class Sieve
{
  class MF
  {
  public:
    template <typename T>
    class apply
    {
    public:
      using type = BoolConstant<Predicate<T>::Type::value>;
    };
  };

public:
  using Type = mpl::filter_view<List, MF>;
};

template <typename List, template <typename Item> class Predicate>
using SieveT = typename Sieve<List, Predicate>::Type;

template <typename List>
class IsUnique
{
  using Unique = typename mpl::unique<List, std::is_same<mpl::_1, mpl::_2>>::type;
public:
  using Type = std::conditional_t<mpl::equal<Unique, List>::value,
                                  std::true_type,
                                  std::false_type>;
};

template <typename List>
using IsUniqueT = typename IsUnique<List>::Type;

} // namespace Kr

// typelist
// tostdtuple
// indextype
// len
// nth
// index
// isinlist
// concat
// foreach
// sieve
// isunique

// multicall

int
main ()
{}

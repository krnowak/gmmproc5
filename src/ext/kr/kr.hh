#ifndef EXT_KR_KR_HH
#define EXT_KR_KR_HH

#include <boost/mpl/at.hpp>
#include <boost/mpl/begin.hpp>
#include <boost/mpl/distance.hpp>
#include <boost/mpl/filter_view.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/joint_view.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/transform_view.hpp>
#include <boost/mpl/vector.hpp>

#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

#include <cstdlib>

namespace Kr
{

namespace mpl = boost::mpl;

// typelist

template <typename... Types>
using TypeList = mpl::vector<Types...>;

// tostdtuple

template <typename List>
class ToStdTuple
{
  template <typename, typename>
  class FoldOp;

  template <typename... Types, typename T>
  class FoldOp<std::tuple<Types...>, T>
  {
  public:
    using type = std::tuple<Types..., T>;
  };

public:
  using Type = typename mpl::fold<List, std::tuple<>, FoldOp<mpl::_1, mpl::_2>>::type;
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
public:
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
  using Begin = typename mpl::begin<Haystack>::type;
  using Iter = typename mpl::find<Haystack, Needle>::type;
  using Pos = typename mpl::distance<Begin, Iter>::type;

public:
  using Type = std::conditional_t<Pos::value < LenT<Haystack>::value,
                                  IndexType<Pos::value>,
                                  MaxIndexType>;
};

template <typename Needle, typename Haystack>
using IndexT = typename Index<Needle, Haystack>::Type;

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
class Concat
{
public:
  using Type = mpl::joint_view<TL1, TL2>;
};

template <typename TL1, typename TL2>
using ConcatT = typename Concat<TL1, TL2>::Type;

// foreach

template <typename List, template <typename> class F>
class ForEach
{
  class MF
  {
  public:
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
  using Set = typename mpl::fold<List, mpl::set<>, mpl::insert<mpl::_1, mpl::_2>>::type;

public:
  using Type = BoolConstant<mpl::size<Set>::value == mpl::size<List>::value>;
};

template <typename List>
using IsUniqueT = typename IsUnique<List>::Type;

// TODO: Is there some boost helper for this?
// multicall

template <template <typename> class Wrapper, typename List>
class MultiCall
{
public:
  template <typename... ParamTypes>
  static void call (ParamTypes&&... p)
  {
    Wrapper<typename mpl::front<List>::type>::call (std::forward<ParamTypes> (p)...);
    MultiCall<Wrapper, typename mpl::pop_front<List>::type>::call (std::forward<ParamTypes> (p)...);
  }
};

template <template <typename> class Wrapper>
class MultiCall<Wrapper, TypeList<>>
{
public:
  template <typename... ParamTypes>
  static void call (ParamTypes&&...)
  {}
};

} // namespace Kr

#endif // EXT_KR_KR_HH

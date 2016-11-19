#ifndef GMMPROC_XML_STRUCTURED_DETAIL_UTILS_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_UTILS_HH

#include <boost/hana/append.hpp>
#include <boost/hana/first.hpp>
#include <boost/hana/fold_left.hpp>
#include <boost/hana/length.hpp>
#include <boost/hana/map.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/second.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>

#include <utility>

namespace Gmmproc::Xml::Structured::Detail
{

namespace NoADL = ::Gmmproc::Xml::Structured::Detail;
namespace hana = ::boost::hana;

// TODO: move it outside the Detail namespace
template <typename TupleP, typename MapP>
class TupleAndMap
{
public:
  using Tuple = TupleP;
  using Map = MapP;

  /*
  constexpr
  TupleAndMap (Tuple t, Map m)
    : tuple {std::move (t)},
      map {std::move (m)}
  {}
  */

  Tuple tuple;
  Map map;

  //static_assert (hana::length (std::declval<TupleP> ()) == hana::length (std::declval<MapP> ()));
};

template <typename TupleP, typename MapP>
constexpr auto
make_tuple_and_map (TupleP tuple, MapP map)
{
  return TupleAndMap<TupleP, MapP> {std::forward<TupleP> (tuple),
                                    std::forward<MapP> (map)};
}

template <typename TupleP>
constexpr auto
tuple_for_map (TupleP tuple)
{
  return hana::fold_left
    (tuple,
     hana::make_tuple (),
     [](auto new_tuple, auto pair)
     {
       auto key {hana::first (pair)};
       auto value {hana::second (pair)};
       auto new_key {hana::make_type (key)};
       auto new_pair {hana::make_pair (new_key, value)};

       return hana::append (new_tuple, new_pair);
     });
}

template <typename TupleP>
constexpr auto
make_tuple_and_map (TupleP tuple)
{
  auto map = hana::to_map (NoADL::tuple_for_map (tuple));
  return NoADL::make_tuple_and_map (std::forward<TupleP> (tuple), std::move (map));
}

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_UTILS_HH

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

template <typename TupleAndMapsTupleP>
constexpr auto
merge_exclusive_tuple_and_maps (TupleAndMapsTupleP tam_tuple)
{
  return hana::fold_left
    (tam_tuple,
     NoADL::make_tuple_and_map (hana::make_tuple ()),
     [](auto merge_tam, auto tam)
     {
       auto all_keys = hana::concat (hana::keys (merge_tam.map), hana::keys (tam.map));
       auto all_keys_set = hana::to_set (all_keys);

       static_assert (hana::length (all_keys) == hana::length (all_keys_set), "keys are unique");

       auto merged_tuple = hana::concat (merge_tam.tuple, tam.tuple);
       return make_tuple_and_map (merged_tuple, hana::to_map (merged_tuple));
     });
}

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_UTILS_HH

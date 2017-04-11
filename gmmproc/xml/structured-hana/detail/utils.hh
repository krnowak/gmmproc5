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
// TODO: or just get rid of it and do searches in tuples
template <typename TupleP,
          typename MapP>
class TupleAndMap
{
public:
  using Tuple = TupleP;
  using Map = MapP;

  // This holds pairs of T1 and T2
  Tuple tuple;
  // This is a map of hana::type<T1> to T2
  Map map;

  constexpr auto
  keys() const {
    return hana::fold_left (hana::keys (this->map),
                            hana::make_tuple (),
                            [] (auto keys, auto key_hana_type)
                            {
                              return hana::append (keys, hana::declval (key_hana_type));
                            });
  }

  constexpr auto
  values () const
  {
    return hana::values (this->map);
  }
};

// explicitly specified deduction guide
template <typename TupleP,
          typename MapP>
TupleAndMap (TupleP, MapP) -> TupleAndMap<TupleP, MapP>;

template <typename TupleP>
constexpr auto
tuple_for_tam (TupleP tuple)
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
  auto map = hana::to_map (NoADL::tuple_for_tam (tuple));

  return TupleAndMap {tuple, map};
}

// TODO: probably use stable_uniq somewhere
template <typename TupleAndMapsTupleP>
constexpr auto
merge_disjoint_tuple_and_maps (TupleAndMapsTupleP tam_tuple)
{
  return hana::fold_left
    (tam_tuple,
     NoADL::make_tuple_and_map (hana::make_tuple ()),
     [](auto merge_tam, auto tam)
     {
       auto all_keys {hana::concat (merge_tam.keys (), tam.keys ())};

       static_assert (hana::length (all_keys) == hana::length (hana::to_set (all_keys)),
                      "keys are unique");

       auto merged_tuple {hana::concat (merge_tam.tuple,
                                        tam.tuple)};
       return NoADL::make_tuple_and_map (merged_tuple);
     });
}

template <typename... TypeP>
class Subclass : public TypeP...
{};

template <typename... TypeP>
constexpr auto
get_subclass (hana::tuple<TypeP...>)
{
  return hana::type_c<Subclass<typename TypeP::type...>>;
}

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_UTILS_HH

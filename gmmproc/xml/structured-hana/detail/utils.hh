#ifndef GMMPROC_XML_STRUCTURED_DETAIL_UTILS_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_UTILS_HH

#include <boost/hana/map.hpp>
#include <boost/hana/length.hpp>

#include <utility>

namespace Gmmproc::Xml::Structured::Detail
{

namespace NoADL = ::Gmmproc::Xml::Structured::Detail;
namespace hana = ::boost::hana;

template <typename TupleP, typename MapP>
class TupleAndMap
{
public:
  using Tuple = TupleP;
  using Map = MapP;

  constexpr
  TupleAndMap (Tuple&& t, Map&& m)
    : tuple {std::move (t)},
      map {std::move (m)}
  {}

  Tuple tuple;
  Map map;

  static_assert (hana::length (std::declval (Tuple)) == hana::length (std::declval (Map)));
};

template <typename TupleP, typename MapP>
constexpr auto
make_tuple_and_map (TupleP&& tuple, MapP&& map)
{
  return TupleAndMap<TupleP, MapP> {tuple, map};
}

template <typename TupleP>
constexpr auto
make_tuple_and_map (TupleP&& tuple)
{
  auto map = hana::to_map (tuple);
  return NoADL::make_tuple_and_map (std::forward<TupleP> (tuple), std::move (map));
}

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_UTILS_HH

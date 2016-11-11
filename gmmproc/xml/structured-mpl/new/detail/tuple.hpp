#ifndef GMMPROC_XML_STRUCTURED_DETAIL_TUPLE_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_TUPLE_HH

#include <gmmproc/xml/structured/detail/mpl.hh>

#include <tuple>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Detail
{

template <typename TupleP, typename TypeP>
struct AppendToStdTuple;

template <typename... TupleTypesP, typename TypeP>
struct AppendToStdTuple<std::tuple<TupleTypesP...>, TypeP>
{
  using Type = std::tuple< TupleTypesP..., TypeP >;
  using type = Type;
};

template <typename ListP>
using GetStdTupleTypeT = Mpl::FoldT<ListP, std::tuple<>, AppendToStdTuple<Mpl::_1, Mpl::_2>>;

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_TUPLE_HH

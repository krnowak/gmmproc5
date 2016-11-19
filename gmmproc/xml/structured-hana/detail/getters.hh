#ifndef GMMPROC_XML_STRUCTURED_DETAIL_GETTERS_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_GETTERS_HH

#include <gmmproc/xml/structured/detail/utils.hh>

#include <boost/hana/append.hpp>
#include <boost/hana/fold_left.hpp>
#include <boost/hana/traits.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>

namespace Gmmproc::Xml::Structured::Detail
{

template <typename... TypeP>
class Subclass : public TypeP...
{};

template <typename... HanaTypeP>
constexpr auto
get_subclass (hana::tuple<HanaTypeP...>)
{
  return hana::type_c<Subclass<typename HanaTypeP::type...>>;
}

template <typename GettersInfoP>
constexpr auto
get_getters_subclass_type (GettersInfoP getters_info)
{
  auto node_info = getters_info.node_info;
  auto unique_getters = hana::fold_left
    (getters_info.unique_getter_tags,
     hana::make_tuple (),
     [node_info](auto tuple, auto getter_tag)
     {
       auto getter = get_getter (hana::traits::declval (getter_tag), node_info);

       return hana::append (tuple, getter);
     });

  return NoADL::get_subclass (unique_getters);
}

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_GETTERS_HH

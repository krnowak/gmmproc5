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

template <typename... TypeP>
constexpr auto
get_subclass (hana::tuple<TypeP...>)
{
  return hana::type_c<Subclass<typename TypeP::type...>>;
}

template <typename GettersInfoP>
constexpr auto
get_getters_subclass_type (GettersInfoP getters_info)
{
  auto container_info = getters_info.container_info;
  auto unique_getter_types = hana::fold_left
    (getters_info.unique_getter_tags,
     hana::make_tuple (),
     [container_info](auto tuple,
                      auto getter_tag)
     {
       auto getter_type {API::Convenience::get_getter_type (getter_tag,
                                                            container_info)};

       return hana::append (tuple,
                            getter_type);
     });

  return NoADL::get_subclass (unique_getter_types);
}

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_GETTERS_HH

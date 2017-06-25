#ifndef GMMPROC_XML_STRUCTURED_CHILD_HH
#define GMMPROC_XML_STRUCTURED_CHILD_HH

#include <gmmproc/xml/structured/detail/utils.hh>

#include <gmmproc/xml/structured/api.hh>
#include <gmmproc/xml/structured/getters.hh>
#include <gmmproc/xml/structured/part.hh>

#include <boost/hana/pair.hpp>
#include <boost/hana/type.hpp>
#include <boost/hana/tuple.hpp>

namespace Gmmproc::Xml::Structured
{

// single

class Single
{};

template <typename RealTypeP>
constexpr auto
get_child_type (Single,
                RealTypeP real_type)
{
  return real_type;
}

constexpr auto
get_child_getter_tags_and_policies (Single)
{
  return Detail::get_simple_getter_tags_and_policies (SingleGetterTag {},
                                                      PassThroughPolicy {});
}

template <typename KindTypeP,
          typename StructuredTagTypeP,
          typename... InputP>
decltype(auto)
generate_child_part_member (Single,
                            KindTypeP kind_type,
                            StructuredTagTypeP structured_tag_type,
                            Input&&... input)
{
  constexpr auto kind {NoADL::unwrap (kind_type)};

  return API::Convenience::generate_kind_part_member (kind,
                                                      structured_tag_type,
                                                      std::forward<InputP> (input)...);
}

// vector

class Vector
{};

template <typename RealTypeP>
constexpr auto
get_child_type (Vector,
                RealTypeP)
{
  return hana::type_c<std::vector<typename RealTypeP::type>>;
}

constexpr auto
get_child_getter_tags_and_policies (Vector)
{
  return Detail::get_simple_getter_tags_and_policies (VectorGetterTag {},
                                                      PassThroughPolicy {});
}

} // namespace Gmmproc::Xml::Structured

#endif // GMMPROC_XML_STRUCTURED_CHILD_HH

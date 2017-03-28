#ifndef GMMPROC_XML_STRUCTURED_VALUE_HH
#define GMMPROC_XML_STRUCTURED_VALUE_HH

#include <gmmproc/xml/structured/detail/utils.hh>

#include <gmmproc/xml/structured/api.hh>
#include <gmmproc/xml/structured/getters.hh>
#include <gmmproc/xml/structured/part.hh>

#include <boost/hana/pair.hpp>
#include <boost/hana/type.hpp>
#include <boost/hana/tuple.hpp>

namespace Gmmproc::Xml::Structured
{

class Single
{};

template <typename NodeTagTypeP>
constexpr auto
get_child_type (Single,
                NodeTagTypeP node_tag_type)
{
  // TODO: create and use the API::Convenience::foo function, so the
  // "using API::foo" clause is used only in the API::Convenience
  // namespace
  using API::get_node_info;

  auto node_tag {boost::hana::declval (node_tag_type)};
  auto registered_node_info {get_node_info (node_tag)};

  return registered_node_info.node_hana_type;
}

constexpr auto
get_child_getters (Single)
{
  auto single_pair {boost::hana::make_pair (Getters::SingleGetterTag {}, Getters::PassThroughPolicy {})};

  return Detail::make_tuple_and_map (boost::hana::make_tuple (single_pair));
}

class ValueTag
{};

template <typename AccessKeyP,
          typename PrimitiveP,
          typename WrapperP>
class Value : public Part<ValueTag>
{
public:
  using AccessKey = AccessKeyP;
  using Primitive = PrimitiveP;
  using Wrapper = WrapperP;

  AccessKey access_key;
  Primitive primitive;
  Wrapper wrapper;
};

template <typename ValueTypeP>
constexpr auto
get_type (ValueTag,
          ValueTypeP value_type)
{
  auto value {boost::hana::traits::declval (value_type)};

  return API::Convenience::get_wrapper_type (value.wrapper,
                                             value.primitive);
}

template <typename ValueTypeP>
constexpr auto
get_access_info (ValueTag,
                 ValueTypeP value_type)
{
  auto value {boost::hana::traits::declval (value_type)};
  auto getter_tags_and_policies {API::Convenience::get_wrapper_getter_tags_and_policies
    (value.wrapper)};

  return Detail::get_simple_access_info (value.access_key,
                                         getter_tags_and_policies);
}

} // namespace Gmmproc::Xml::Structured

#endif // GMMPROC_XML_STRUCTURED_VALUE_HH

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
get_child_getter_tags_and_policies (Single)
{
  return Detail::get_simple_getter_tags_and_policies (SingleGetterTag {},
                                                      PassThroughPolicy {});
}

class ChildTag
{};

template <typename AccessKeyP,
          typename BasicP,
          typename NodeTagP>
class Child : public Part<ChildTag>
{
public:
  using AccessKey = AccessKeyP;
  using Basic = BasicP;
  using NodeTag = NodeTagP;

  AccessKey access_key;
  Basic basic;
  NodeTag node_tag;
};

template <typename ChildTypeP>
constexpr auto
get_type (ChildTag,
          ChildTypeP child_type)
{
  auto child {boost::hana::traits::declval (child_type)};

  return API::Convenience::get_child_type (child.basic,
                                           child.node_tag);
}

template <typename ChildTypeP>
constexpr auto
get_access_info (ChildTag,
                 ChildTypeP child_type)
{
  auto child {boost::hana::traits::declval (child_type)};
  auto getters {API::Convenience::get_child_getters (child.basic)};
  auto access_info_pair {boost::hana::make_pair (child.access_key,
                                                 getters)};

  return Detail::make_tuple_and_map (boost::hana::make_tuple (access_info_pair));
}

} // namespace Gmmproc::Xml::Structured

#endif // GMMPROC_XML_STRUCTURED_CHILD_HH

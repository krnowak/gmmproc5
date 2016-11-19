#ifndef GMMPROC_XML_STRUCTURED_CHILD_HH
#define GMMPROC_XML_STRUCTURED_CHILD_HH

#include <gmmproc/xml/structured/detail/utils.hh>

#include <gmmproc/xml/structured/api.hh>
#include <gmmproc/xml/structured/getters.hh>

#include <boost/hana/pair.hpp>
#include <boost/hana/type.hpp>
#include <boost/hana/tuple.hpp>

namespace Gmmproc::Xml::Structured::Basic
{

class Single
{};

template <typename NodeTagP>
constexpr auto
get_contained_child_type (Single,
                          NodeTagP node_tag)
{
  using API::get_node_info;

  auto registered_node_info = get_node_info (node_tag);

  return registered_node_info.node_hana_type;
}

constexpr auto
get_child_getters (Single)
{
  auto single_pair = boost::hana::make_pair (Getters::SingleGetterTag {}, Getters::PassThroughPolicy {});

  return Detail::make_tuple_and_map (boost::hana::make_tuple (single_pair));
}

/*

template <typename NodeTagP>
constexpr auto
get_child_generator (Single,
                     boost::hana::basic_type<NodeTagP>)
{
  return boost::hana::type_c<Generators::SingleGenerator<typename Registry::template NodeInfo<NodeTagP>::Type>>;
}

*/

class ChildTag {};

// TODO: document that Child must have a public Tag type and public tag member
// variable.
template <typename AccessKeyP, typename BasicP, typename NodeTagP>
class Child
{
public:
  using Tag = ChildTag;
  using AccessKey = AccessKeyP;
  using Basic = BasicP;
  using NodeTag = NodeTagP;

  Tag tag;
  AccessKey access_key;
  Basic basic;
  NodeTag node_tag;
};

template <typename ChildP>
constexpr auto
get_contained_type (ChildTag, ChildP child)
{
  return get_contained_child_type (child.basic, child.node_tag);
}

template <typename ChildP>
constexpr auto
get_access_info (ChildTag, ChildP child)
{
  auto getters {get_child_getters (child.basic)};
  auto access_info_pair {boost::hana::make_pair (child.access_key, getters)};

  return Detail::make_tuple_and_map (boost::hana::make_tuple (access_info_pair));
}

/*

template <typename AccessKeyP, typename BasicP, typename NodeTagP, typename StorageP>
constexpr auto
get_generator (Child<AccessKeyP, BasicP, NodeTagP> child,
               boost::hana::basic_type<StorageP> s)
{
  return get_child_generator (c.basic, c.node_tag);
}

*/

} // namespace Gmmproc::Xml::Structured::Basic

#endif // GMMPROC_XML_STRUCTURED_CHILD_HH

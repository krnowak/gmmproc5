#ifndef GMMPROC_XML_STRUCTURED_CHILD_HH
#define GMMPROC_XML_STRUCTURED_CHILD_HH

#include <gmmproc/xml/structured/detail/mpl.hh>
#include <gmmproc/xml/structured/registry.hh>
#include <gmmproc/xml/structured/generators.hh>

namespace Gmmproc::Xml::Structured::Basic
{

class Single
{};

template <typename NodeTagP>
constexpr auto
get_contained_child_type (Single, NodeTagP)
{
  return hana::type_c<typename Registry::template NodeInfo<NodeTagP>::Type>;
}

template <typename ResolvedStorageTagP>
constexpr auto
get_child_getters (Single, ResolvedStorageTagP)
{
  
  return hana::make_map (hana::make_pair ());
  return hana::type_c<Detail::SingleGetterWithDefaultPolicy<StorageTagP>>;
}

template <typename NodeTagP>
constexpr auto
get_child_generator (Single, hana::basic_type<NodeTagP>)
{
  return hana::type_c<Generators::SingleGenerator<typename Registry::template NodeInfo<NodeTagP>::Type>>;
}

template <typename BasicP, typename NodeTagP>
class Child
{
public:
  using Basic = BasicP;
  using NodeTag = NodeTagP;

  Basic basic;
  NodeTag node_tag;
};

template <typename BasicP, typename NodeTagP>
constexpr auto
get_contained_type (Child<BasicP, NodeTagP> c)
{
  return get_contained_child_type (c.basic, c.node_tag);
}

template <typename BasicP, typename NodeTagP, typename ResolvedStorageTagP>
constexpr auto
get_access_info (Child<BasicP, NodeTagP> c, ResolvedStorageTagP s)
{
  using Registry::get_node_info;
  auto access_key = hana::second (get_node_info (c.node_tag));
  auto getters_map = get_child_getters (c.basic, s);

  return get_contained_child_type (c.basic, c.node_tag);
}

template <typename BasicP, typename NodeTagP, typename StorageP>
constexpr auto
get_generator (Child<BasicP, NodeTagP> c, hana::basic_type<StorageP> s)
{
  return get_child_generator (c.basic, c.node_tag);
}

} // namespace Gmmproc::Xml::Structured::Basic

#endif // GMMPROC_XML_STRUCTURED_CHILD_HH

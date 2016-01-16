#include "node-common.hh"
#include "short.hh"

#include <boost/range/iterator_range_core.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include <functional>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

namespace
{

bool
is_a_leaf (Xml::Base::Node const& node)
{
  auto r = node.children ();
  return std::distance (r.begin (), r.end ()) == 0;
}

LeafType
update_leaf_type (Xml::Base::Node const& node,
                  LeafType const& previous_type)
{
  switch (previous_type)
  {
  case LeafType::UNDETERMINED:
    return (is_a_leaf (node) ? LeafType::ALWAYS_A_LEAF : LeafType::NEVER_A_LEAF);

  case LeafType::NEVER_A_LEAF:
    return (is_a_leaf (node) ? LeafType::SOMETIMES_A_LEAF : LeafType::NEVER_A_LEAF);

  case LeafType::SOMETIMES_A_LEAF:
    return LeafType::SOMETIMES_A_LEAF;

  case LeafType::ALWAYS_A_LEAF:
    return (is_a_leaf (node) ? LeafType::ALWAYS_A_LEAF : LeafType::SOMETIMES_A_LEAF);
  }

  // invalid case
  return LeafType::UNDETERMINED;
}

void
process_element (ShortNode& data,
                 Xml::Base::Node const& data_node)
{
  ++data.count;
  data.has_text = (data.has_text || !data_node.text ().empty ());
  data.is_leaf = update_leaf_type (data_node, data.is_leaf);
}

void
process_attributes (ShortNode& data,
                    Xml::Base::Node const& data_node)
{
  for (auto attr : data_node.attributes ())
  {
    auto const name = attr.name ();
    auto pair = data.attributes.emplace (name, name);
    auto& attr_data = pair.first->second;

    ++attr_data.count;
    attr_data.update (attr.value ());
  }
}

void
process_children (ShortNode& data,
                  Xml::Base::Node const& data_node)
{
  for (auto child_node : data_node.children ())
  {
    auto const name = child_node.name ();
    data.children.emplace (name, name);
  }
}

// implements NodeWrapperModel
template <typename Node>
class ShortNodeWrapper
{
public:
  using NodeType = Node;
  template <typename N>
  using ThisTemplateType = ShortNodeWrapper<N>;

  using MutableNode = std::remove_const_t<NodeType>;
  using MutableNodeMap = StrMap<MutableNode>;
  using NodeMap = add_const_if_const_t<MutableNodeMap, NodeType>;

  ShortNodeWrapper (Node& node,
                    NodeMap& nodes)
    : impl {node},
      all_impls {nodes},
      parent_impl {nullptr}
  {}

  ShortNodeWrapper (Node& node,
                    NodeMap& nodes,
                    Node& parent)
    : impl {node},
      all_impls {nodes},
      parent_impl {std::addressof (parent)}
  {}

  auto
  attributes ()
  {
    return get_map_range<1> (impl.get ().attributes);
  }

  auto
  attributes () const
  {
    return get_map_range<1> (impl.get ().attributes);
  }

  auto
  children ()
  {
    return get_node_wrapper_children (*this);
  }

  auto
  children () const
  {
    return get_node_wrapper_children (*this);
  }

  add_const_if_const_t<Named, Node>&
  as_named()
  {
    return impl.get ();
  }

  Named const&
  as_named() const
  {
    return impl.get ();
  }

  add_const_if_const_t<WithOccurences, Node>&
  as_with_occurences ()
  {
    ensure_with_parent ();
    return must_get (parent_impl->children, impl.get ().name);
  }

  WithOccurences const&
  as_with_occurences () const
  {
    ensure_with_parent ();
    return must_get (parent_impl->children, impl.get ().name);
  }

  add_const_if_const_t<Counted, Node>&
  as_counted ()
  {
    return impl.get ();
  }

  Counted const&
  as_counted () const
  {
    return impl.get ();
  }

private:
  template <typename FromNodeWrapper, typename ToNodeWrapper>
  class ChildToWrapper
  {
  public:
    using ChildMapType = typename FromNodeWrapper::NodeType::ChildMap;
    using NodeMapType = typename FromNodeWrapper::NodeMap;
    using ParentType = typename FromNodeWrapper::NodeType;
    using PairType = add_const_if_const_t<typename ChildMapType::value_type, ParentType>;

    // Default construct is provided only because
    // boost::transform_iterator requires the functor to be default
    // constructible.
    ChildToWrapper () = default;
    ChildToWrapper (NodeMapType& n,
                    ParentType& p)
      : nodes {std::addressof (n)},
        parent {std::addressof (p)}
    {}

    ToNodeWrapper
    operator() (PairType& pair) const
    {
      auto& node = must_get (*nodes, pair.second.name);

      return ToNodeWrapper {node, *nodes, *parent};
    }

    // These are pointers only because references are not default
    // constructible.
    NodeMapType* nodes = nullptr;
    ParentType* parent = nullptr;
  };

  template <typename FromNodeWrapper>
  static auto
  get_node_wrapper_children (FromNodeWrapper& node_wrapper)
  {
    using WrapperNodeType = typename FromNodeWrapper::NodeType;
    using NodeType = add_const_if_const_t<WrapperNodeType, FromNodeWrapper>;
    using WrapperType = typename FromNodeWrapper::template ThisTemplateType<NodeType>;
    using Transformer = ChildToWrapper<FromNodeWrapper, WrapperType>;

    auto& node = node_wrapper.impl.get ();
    auto& children = node.children;
    auto& all_nodes = node_wrapper.all_impls.get ();
    Transformer transformer {all_nodes, node};

    return boost::make_iterator_range (boost::make_transform_iterator (children.begin (), transformer),
                                       boost::make_transform_iterator (children.end (), transformer));
  }

  void
  ensure_with_parent () const
  {
    if (!parent_impl)
    {
      throw std::runtime_error ("Cannot use with parent node");
    }
  }

  std::reference_wrapper<Node> impl;
  std::reference_wrapper<NodeMap> all_impls;
  Node* parent_impl;
};

} // anonymous namespace

void
Short::process_node_vfunc (Xml::Base::Node const& data_node,
                           int)
{
  auto const name = data_node.name ();
  auto pair = nodes.emplace (name, name);
  auto& data = pair.first->second;

  process_element (data, data_node);
  process_attributes (data, data_node);
  process_children (data, data_node);
}

void
Short::postprocess_node_vfunc (Xml::Base::Node const& data_node,
                               int)
{
  auto& data = must_get (nodes, data_node.name ());
  ShortNodeWrapper<ShortNode> data_wrapper {data, nodes};

  get_unique_attributes (data_wrapper, data_node);
  get_occurences (data_wrapper, data_node);
}

StrMap<ShortNode>&&
Short::steal ()
{
  return std::move (nodes);
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

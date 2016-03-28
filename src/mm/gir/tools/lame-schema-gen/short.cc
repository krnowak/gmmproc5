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

  auto parent = data_node.parent ();
  if (parent)
  {
    data.parents.emplace (parent->name (), false);
  }
}

StrSortedSet
get_diff (StrSortedSet const& first, StrSortedSet const& second)
{
  StrSortedSet output;

  std::set_difference (first.cbegin (), first.cend (),
                       second.cbegin (), second.cend (),
                       std::inserter (output, output.begin ()));
  return output;
}

void
process_sets (StrSortedSet&& fresh_set, ShortNode::Sets& sets)
{
  auto iter = sets.begin ();

  for (; iter != sets.cend (); ++iter)
  {
    auto& set = *iter;
    StrSortedSet only_in_fresh = get_diff (fresh_set, set);

    if (only_in_fresh.empty ())
    {
      // fresh set has nothing new, nothing to do
      return;
    }

    StrSortedSet only_in_set = get_diff (set, fresh_set);
    if (only_in_set.empty ())
    {
      // fresh has some new elements, update the set
      set.insert (only_in_fresh.cbegin (), only_in_fresh.cend ());
      break;
    }
    // fresh set and set have exclusive elements, try another set
  }
  if (iter != sets.cend ())
  {
    auto const& updated_set = *iter;
    sets.erase (std::remove_if (sets.begin (), sets.end (),
                                [&updated_set](auto const& set)
                                {
                                  return &updated_set != &set &&
                                    get_diff (set, updated_set).empty ();
                                }), sets.end ());
    return;
  }
  else
  {
    sets.push_back (std::move (fresh_set));
  }
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
  StrSortedSet children;

  for (auto child_node : data_node.children ())
  {
    auto const name = child_node.name ();
    data.children.emplace (name, name);
    children.insert (name);
  }
  process_sets (std::move (children), data.children_sets);
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

class Path
{
public:
  void
  add (Str const& name)
  {
    mset.insert (name);
    stack.push (name);
  }

  bool
  visited (Str const &name)
  {
    return mset.find (name) != mset.cend ();
  }

  void
  truncate(std::size_t depth) {
    while (!stack.empty() && stack.size() >= depth)
    {
      auto& name = stack.top ();
      auto iter = mset.find (name);

      if (iter == mset.cend ()) {
        throw std::runtime_error ("map and stack in path should have the same contents");
      }
      mset.erase (iter);
      stack.pop ();
    }
  }

private:
  StrMultiSet mset;
  std::stack<Str> stack;
};

void
find_recursion_points (StrMap<ShortNode>& nodes,
                       Str const& root_name)
{
  using NodeRef = std::reference_wrapper<ShortNode>;
  using StrRef = std::reference_wrapper<Str>;
  using NodeTuple = std::tuple<NodeRef, StrRef, std::size_t>;
  Path path;
  Str empty;
  std::stack<NodeTuple> node_stack;
  auto& root = must_get (nodes, root_name);

  node_stack.emplace (root, empty, 0);
  while (!node_stack.empty ())
  {
    auto& t = node_stack.top ();
    auto& node = std::get<0> (t).get ();
    auto const& parent_name = std::get<1> (t).get ();
    auto const depth = std::get<2> (t);
    node_stack.pop ();

    path.truncate (depth);
    if (path.visited (node.name))
    {
      node.parents[parent_name] = true;
      continue;
    }
    path.add (node.name);
    for (auto const& p : node.children)
    {
      auto& child = must_get (nodes, p.first);
      node_stack.emplace (child, node.name, depth + 1);
    }
  }
}

} // anonymous namespace

void
Short::process_node_vfunc (Xml::Base::Node const& data_node,
                           int depth)
{
  auto const name = data_node.name ();
  auto pair = nodes.emplace (name, name);
  auto& data = pair.first->second;

  if (!depth)
  {
    if (toplevel_name.empty ())
    {
      toplevel_name = name;
    }
    else if (toplevel_name != name)
    {
      std::ostringstream oss;

      oss << "different toplevel names, '" << toplevel_name << "' and '" << name << "'";
      throw std::runtime_error (oss.str ());
    }
  }

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

void
Short::wrap_up_vfunc ()
{
  if (!toplevel_name.empty ())
  {
    find_recursion_points (nodes, toplevel_name);
  }
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

#include "long.hh"
#include "node-common.hh"
#include "utils.hh"

#include <boost/range/iterator_range_core.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include <stdexcept>
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

template <typename NodeWrapper>
auto
get_node_wrapper_children (NodeWrapper& node)
{
  using WrapperNodeType = typename NodeWrapper::NodeType;
  using NodeType = add_const_if_const_t<WrapperNodeType, NodeWrapper>;
  using WrapperType = typename NodeWrapper::ThisTemplateType<NodeType>;

  auto get_as_wrapper = [](auto& pair) { return WrapperType {*pair.second}; };

  return boost::make_iterator_range (boost::make_transform_iterator (impl.children.begin (), get_as_wrapper),
                                     boost::make_transform_iterator (impl.children.end (), get_as_wrapper));
}

// implements NodeWrapperModel
template <typename Node>
class LongNodeWrapper
{
public:
  using NodeType = Node;
  template <typename N>
  using ThisTemplateType = template LongNodeWrapper<N>;

  NodeWrapper (Node& node)
    : impl {node}
  {}

  template
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
    return impl.get ();
  }

  WithOccurences const&
  as_with_occurences () const
  {
    return impl.get ();
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
  std::reference_wrapper<Node> impl;
};

void
get_attributes (LongNode& data,
                Xml::Base::Node const& data_node)
{
  for (auto const& at : data_node.attributes ())
  {
    auto const name = at.name ();
    auto attr_pair = data.attributes.emplace (name, name);
    auto& attr = attr_pair.first->second;

    ++attr.count;
    attr.update (at.value ());
  }
}

void
process_single_element (LongNode& data,
                        Xml::Base::Node const& data_node)
{
  get_attributes (data, data_node);
  ++data.count;
}

void
postprocess_single_element (LongNode& data,
                            Xml::Base::Node const& data_node)
{
  LongNodeWrapper<LongNode> data_wrapper {data};

  get_unique_attributes (data_wrapper, data_node);
  get_occurences (data_wrapper, data_node);
}

} // anonymous namespace

void
Long::process_toplevel (Xml::Base::Node const& node,
                        int depth)
{
  if (depth > 0)
  {
    throw std::logic_error ("no toplevel node, but the depth is greater than zero");
  }
  if (!node_stack.empty ())
  {
    throw std::logic_error ("no toplevel node, but node stack is not empty");
  }
  toplevel = std::make_unique<LongNode> (node.name ());
  toplevel->min_occurences = 1;
  toplevel->max_occurences = 1;
}

void
Long::process_element (Xml::Base::Node const& node,
                       int depth)
{
  if (depth == 0)
  {
    throw std::logic_error ("there is a toplevel node, but the depth is zero");
  }
  if (node_stack.empty ())
  {
    throw std::logic_error ("there is a toplevel node, but node stack is empty");
  }
  setup_node_stack_for_depth (depth);
  auto name = node.name ();
  auto pair = node_stack.top ().get ().children.emplace (name, std::make_unique<LongNode> (name));
  auto& child = *(pair.first->second);
  if (pair.second)
  {
    child.depth = depth;
  }
  process_single_element (child, node);
  node_stack.push (child);
}

void
Long::setup_node_stack_for_depth (int depth)
{
  if (depth - node_stack.top ().get ().depth > 1)
  {
    throw std::runtime_error ("sudden large jump in depth, expected a depth greater by only one");
  }

  while (node_stack.top ().get ().depth >= depth)
  {
    node_stack.pop ();
    if (node_stack.empty ())
    {
      throw std::runtime_error ("something went wrong when getting parent for depth");
    }
  }
}

void
Long::process_node_vfunc (Xml::Base::Node const& node,
                          int depth)
{
  if (!toplevel)
  {
    process_toplevel (node, depth);
  }
  else
  {
    process_element (node, depth);
  }
}

void
Long::postprocess_node_vfunc (Xml::Base::Node const& node,
                              int depth)
{
  {
    auto& child = node_stack.top ().get ();
    if (child.depth != depth)
    {
      throw std::runtime_error ("invalid node on stack to postprocess");
    }
    postprocess_single_element (child, node);
  }
  node_stack.pop ();
}

std::unique_ptr<LongNode>&&
Long::steal ()
{
  return std::move (toplevel);
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

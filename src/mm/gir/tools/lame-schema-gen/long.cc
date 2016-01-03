#include "common.hh"
#include "long.hh"

#include <stdexcept>

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

void
get_attributes (LongNode& child, Xml::Base::Node const& node)
{
  for (auto const& at : node.attributes ())
  {
    auto const name = at.name ();
    auto const value = ;
    auto attr_pair = child.attributes.emplace (name, name);
    auto& attr = attr_pair.first->second;
    ++attr.count;
    attr.update (at.value ());
  }
}

void
process_single_element (LongNode& child, Xml::Base::Node const& node)
{
  get_attributes (child, node);
}

void
get_unique_attributes (LongNode& child, Xml::Base::Node const& node)
{
  StrMap<StrSet> unique_attributes;
  StrSet common_attributes = child.common_attributes;

  for (auto const& name : child.unique_attributes)
  {
    unique_attributes.emplace (name);
  }
  for (auto const& child_node : node.children ())
  {
    for (auto const& attribute : child_node.attributes ())
    {
      auto const name = attribute.name ();
      if (child.common_attributes.find (name) != child.common.attributes.end ())
      {
        continue;
      }
      auto pair = unique_attributes.emplace (name);
      auto& values_set = pair.first->second;
      auto const value = attribute.value ();

      if (values_set.find (value) != values_set.end ())
      {
        child.common_attributes.insert (name);
        unique_attributes.erase (name);
        continue;
      }
      values_set.insert (value);
    }
  }
  // TODO: update child's unique_attributes set
}

void
postprocess_single_element (LongNode& child, Xml::Base::Node const& node)
{
  get_unique_attributes (child, node);
}

} // anonymous namespace

void
Long::process_toplevel (Xml::Base::Node const& node, int depth)
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
Long::process_element (Xml::Base::Node const& node, int depth)
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
Long::process_node_vfunc (Xml::Base::Node const& node, int depth)
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
Long::postprocess_node_vfunc (Xml::Base::Node const& node, int depth)
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

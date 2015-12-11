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
process_single_element (Long::Node& child, pugi::xml_node const& node)
{
  // TODO
}


} // anonymous namespace

void
Long::process_toplevel (std::string const& name, pugi::xml_node const& node, int depth)
{
  if (depth > 0)
  {
    throw std::logic_error ("no toplevel node, but the depth is greater than zero");
  }
  if (!node_stack.empty ())
  {
    throw std::logic_error ("no toplevel node, but node stack is not empty");
  }
  toplevel = std::make_unique (name);
  toplevel->min_occurences = 1;
  toplevel->max_occurences = 1;
}

void
Long::process_element (std::string const& name, pugi::xml_node const& node, int depth)
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
  auto pair = node_stack.top ().children.emplace (name, name);
  auto& child = pair.first->second;
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
  auto depth_diff = depth - current_node->depth;

  if (depth_diff > 0)
  {
    if (depth_diff > 1)
    {
      throw std::runtime_error ("sudden large jump in depth, expected a depth greater by one");
    }
  }
  else
  {
    while (node_stack.top ().depth >= depth)
    {
      node_stack.pop ();
      if (node_stack.empty ())
      {
        throw std::runtime_error ("something went wrong when getting parent for depth");
      }
    }
  }
}

void
Long::process_node_vfunc (std::string const& name, pugi::xml_node const& node, int depth)
{
  if (!toplevel)
  {
    process_toplevel (name, node, depth);
  }
  else
  {
    process_element (name, node, depth);
  }
}

std::unique_ptr<Long::Node>&&
Long::steal ()
{
  return std::move (nodes);
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

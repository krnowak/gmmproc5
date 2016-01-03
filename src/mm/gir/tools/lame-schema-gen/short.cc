#include "common.hh"
#include "short.hh"

#include <cctype>

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
process_element (ShortNode& data, Xml::Base::Node const& node)
{
  ++data.count;
  data.has_text = (data.has_text || !node.text ().empty ());
  data.is_leaf = update_leaf_type (node, data.is_leaf);
}

void
process_attributes (ShortNode& data, Xml::Base::Node const& node)
{
  for (auto attr : node.attributes ())
  {
    auto const name = attr.name ();
    auto pair = data.attributes.emplace (name, name);
    auto& attr_data = pair.first->second;

    ++attr_data.count;
    attr_data.update (attr.value ());
  }
}

void
process_children (ShortNode& data, Xml::Base::Node const& node)
{
  struct MetData
  {
    MetData (ShortNode::Child& c)
      : child {c},
        occurences {0}
    {}

    ShortNode::Child& child;
    std::size_t occurences;
  };
  StrMap<MetData> met_children;

  for (auto child : node.children ())
  {
    if (!child.is_tag ())
    {
      continue;
    }
    auto const name = child.name ();
    auto pair = data.children.emplace (name, name);
    auto& child_data = pair.first->second;
    auto met_iter = met_children.find (name);

    if (met_iter == met_children.end ())
    {
      ++child_data.count;
      if (data.count > child_data.count)
      {
        child_data.min_occurences = 0;
      }
      met_iter = met_children.emplace (name, child_data).first;
    }
    ++met_iter->second.occurences;
  }

  for (auto& pair : met_children)
  {
    auto occurences = pair.second.occurences;
    auto& child = pair.second.child;

    if (occurences > child.max_occurences)
    {
      child.max_occurences = occurences;
    }
    if (occurences < child.min_occurences)
    {
      child.min_occurences = occurences;
    }
  }
}

} // anonymous namespace

void
Short::process_node_vfunc (Xml::Base::Node const& node, int)
{
  auto const name = node.name ();
  auto pair = nodes.emplace (name, name);
  auto& data = pair.first->second;

  process_element (data, node);
  process_attributes (data, node);
  process_children (data, node);
}

void
Short::postprocess_node_vfunc (Xml::Base::Node const&, int)
{}

StrMap<ShortNode>&&
Short::steal ()
{
  return std::move (nodes);
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

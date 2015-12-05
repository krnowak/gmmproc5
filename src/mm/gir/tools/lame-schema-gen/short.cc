#include "short.hh"

#include <pugixml.hpp>

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

bool
is_a_leaf (pugi::xml_node const& node)
{
  for (auto n : node.children ())
  {
    if (n.type () == pugi::node_element)
    {
      return false;
    }
  }
  return true;
}

void
process_leaves (Short::Leafed& leafed, pugi::xml_node const& node)
{
  if (leafed.leaf == Short::Leaf::SOMETIMES_A_LEAF)
  {
    return;
  }

  auto is_leaf = is_a_leaf (node);

  switch (leafed.leaf)
  {
  case Short::Leaf::UNDETERMINED:
    leafed.leaf = (is_leaf ? Short::Leaf::ALWAYS_A_LEAF : Short::Leaf::NEVER_A_LEAF);
    break;

  case Short::Leaf::NEVER_A_LEAF:
    leafed.leaf = (is_leaf ? Short::Leaf::SOMETIMES_A_LEAF : Short::Leaf::NEVER_A_LEAF);
    break;

  case Short::Leaf::SOMETIMES_A_LEAF:
    break;

  case Short::Leaf::ALWAYS_A_LEAF:
    leafed.leaf = (is_leaf ? Short::Leaf::ALWAYS_A_LEAF : Short::Leaf::SOMETIMES_A_LEAF);
    break;
  }
}

void
process_node (Short::Node& data, pugi::xml_node const& node)
{
  ++data.count;
  data.has_text = (data.has_text || !node.text ().empty ());
  process_leaves (data, node);
}

void
process_attributes (Short::Node& data, pugi::xml_node const& node)
{
  for (auto attr : node.attributes ())
  {
    std::string name = attr.name ();
    auto pair = data.attributes.emplace (name, name);
    auto& attr_data = pair.first->second;

    ++attr_data.count;
    switch (attr_data.type)
    {
    case Short::Attribute::Type::UNDETERMINED:
    case Short::Attribute::Type::NUMERIC:
      for (auto ptr = attr.value (); *ptr; ++ptr)
      {
        if (!std::isdigit (*ptr))
        {
          attr_data.type = Short::Attribute::Type::STRING;
          break;
        }
      }
      if (attr_data.type != Short::Attribute::Type::STRING)
      {
        attr_data.type = Short::Attribute::Type::NUMERIC;
      }
      break;

    case Short::Attribute::Type::STRING:
      break;
    }
  }
}

void
process_children (Short::Node& data, pugi::xml_node const& node)
{
  struct MetData
  {
    MetData(Short::Child& c)
      : child {c},
        occurences {0}
    {}

    Short::Child& child;
    std::size_t occurences;
  };
  StrMap<MetData> met_children;

  for (auto child : node.children ())
  {
    if (child.type () != pugi::node_element)
    {
      continue;
    }
    std::string name = child.name ();
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

void
process_named_node (StrMap<Short::Node>& nodes, std::string const& name, pugi::xml_node const& node)
{
  auto pair = nodes.emplace (name, name);
  auto& data = pair.first->second;

  process_node (data, node);
  process_attributes (data, node);
  process_children (data, node);
}

} // anonymous namespace

void
Short::process_node (pugi::xml_node const& node)
{
  process_named_node (nodes, node.name (), node);
}

void
Short::process_document (pugi::xml_node const& document)
{
  process_named_node (nodes, "!@#$_DOCROOT_$#@!", document);
}

StrMap<Short::Node>&&
Short::steal ()
{
  return std::move (nodes);
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#include "short.hh"

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
  if (leafed.leaf == Leaf::SOMETIMES_A_LEAF)
  {
    return;
  }

  auto is_leaf = is_a_leaf (node);

  switch (leafed.leaf)
  {
  case Leaf::UNDETERMINED:
    leafed.leaf = (is_leaf ? Leaf::ALWAYS_A_LEAF : Leaf::NEVER_A_LEAF);
    break;

  case Leaf::NEVER_A_LEAF:
    leafed.leaf = (is_leaf ? Leaf::SOMETIMES_A_LEAF : Leaf::NEVER_A_LEAF);
    break;

  case Leaf::SOMETIMES_A_LEAF:
    break;

  case Leaf::ALWAYS_A_LEAF:
    leafed.leaf = (is_leaf ? Leaf::ALWAYS_A_LEAF : Leaf::SOMETIMES_A_LEAF);
    break;
  }
}

void
process_node (Short::Node& data, pugi::xml_node const& node)
{
  ++data.count;
  data.has_text ||= !node.text ().empty ();
  process_leaves (data, node);
}

void
process_attributes (Short::Node& data, pugi::xml_node const& node)
{
  for (auto attr : node.attributes ())
  {
    auto pair = data.attributes.emplace (attr.name ());
    auto& attr_data = *pair->first;

    ++attr_data.count;
    switch (attr_data.type)
    {
    case AttributeData::Type::UNDETERMINED:
    case AttributeData::Type::NUMERIC:
      for (auto ptr = attr.value (); *ptr; ++ptr)
      {
        if (!std::is_digit (*ptr))
        {
          attr_data.type = AttributeData::Type::STRING;
          break;
        }
      }
      if (attr_data.type != AttributeData::Type::STRING)
      {
        attr_data.type = AttributeData::Type::NUMERIC;
      }
      break;

    case AttributeData::Type::STRING:
      break;
    }
  }
}

void
process_children (Short::Node& data, pugi::xml_node const& node)
{
  struct MetData
  {
    MetData(ChildData& c)
      : child {c},
        occurences {0}
    {}

    ChildData& child;
    std::size_t occurences;
  };
  StrMap<MetData> met_children;

  for (auto child : node.children ())
  {
    if (child.type () != pugi::node_element)
    {
      continue;
    }
    auto name = child.name ();
    auto pair = data.children.emplace (name);
    auto& child_data = *pair->first;
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
process_named_node (NamedSet<Node>& nodes, char const* name, pugi::xml_node const& node)
{
  auto pair = nodes.emplace (name));
  auto& data = *pair->first;

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

NamedSet<Short::Node>&&
Short::steal ()
{
  return std::move (nodes);
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_SHORT_HH
